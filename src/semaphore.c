#include"semaphore.h"
#include"hplRTOS.h"

void semaphoreInit(semaphoreTypedef* sem, unsigned char count, unsigned char maxCount){
  sem->count = count;
  sem->maxCount = maxCount;
  sem->waitingCount = 0;
  for(int i = 0; i < maxTask; i++){
    sem->waitingTasks[i] = invalidTask; 
  }
}
void semaphoreTake(semaphoreTypedef* sem){
  //available
  __asm volatile("cpsid i"); //disenable interrupt
  if(sem->count > 0){
    sem->count--;
    __asm volatile("cpsie i"); //enable interrupts
    return;
  }
  
  //not available
  else{
    task[currentTask].state = taskBlockedSemaphore;
    sem->waitingTasks[sem->waitingCount] = currentTask;
    sem->waitingCount++;
    __asm volatile("cpsie i");
    
    // Trigger context switch via PendSV
    SCB_ICSR |= PENDSV_SET;
  }
}
void semaphoreGive(semaphoreTypedef* sem){
  __asm volatile("cpsid i");
  
  if(sem->waitingCount > 0){
    //first task need be waked up
    unsigned char taskToWake = sem->waitingTasks[0];
    
    //Remove from waiting tasks
    for(int i = 0; i < sem->waitingCount - 1; i++){
      sem->waitingTasks[i] = sem->waitingTasks[i + 1];
    }
    sem->waitingCount--;
    sem->waitingTasks[sem->waitingCount] = invalidTask;
    
    //Wake up
    if(taskToWake < maxTask && task[taskToWake].isUsed){
      task[taskToWake].state = taskReady;
      SCB_ICSR |= PENDSV_SET; // Trigger scheduler
    }
  }
  else{
        if(sem->count < sem->maxCount){
            sem->count++;
        }
    }
 
  __asm volatile("cpsie i"); 
}
