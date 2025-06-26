#include"mutex.h"

void mutexInit(mutexTypedef* mutex){
  mutex->isLocked = 0;
  mutex->ownerTask = invalidTask;
  mutex->originalPriority = PRIORITY_NORMAL;
  mutex->waitingCount = 0;
  for(int i = 0; i < maxTask; i++){
    mutex->waitingTasks[i] = 0xFF;
  }
}

void mutexLock(mutexTypedef* mutex){
  __asm volatile("cpsid i"); // Disable interrupts
  
  if(mutex->isLocked == 0){
    mutex->isLocked = 1;
    mutex->ownerTask = currentTask;
    mutex->originalPriority = task[currentTask].priority;
    __asm volatile("cpsie i"); // Enable interrupts
    return;
  }
  else{
    mutex->waitingTasks[mutex->waitingCount] = currentTask;
    mutex->waitingCount++;
    
    task[currentTask].state = taskBlockedMutex; // block current task;
    
    //Priority inheriance if current task has higher priority than owner task
     mutexInheritPriority(mutex);
    
    __asm volatile("cpsie i"); // Enable interrupts
    
    // trigger context switch
    SCB_ICSR |= PENDSV_SET;
    return;
  }
}

void mutexUnlock(mutexTypedef* mutex){
  __asm volatile("cpsid i");
  if(mutex->ownerTask != currentTask){
    return;
  }
  
  // Restore original priority of current task
  mutexRestorePriority(mutex);
  
  if(mutex->waitingCount > 0){
    
     // Find highest priority waiting task
    unsigned char highestPriorityTask = mutex->waitingTasks[0];
    unsigned char highestPriorityIndex = 0;
    for(int i = 1; i < mutex->waitingCount; i++) {
      unsigned char waitingTask = mutex->waitingTasks[i];
      if(task[waitingTask].priority < task[highestPriorityTask].priority) {
        highestPriorityTask = waitingTask;
        highestPriorityIndex = i;
      }
    }
    
    // Give mutex to highest priority waiting task
    mutex->ownerTask = highestPriorityTask;
    mutex->originalPriority = task[highestPriorityTask].priority;
        
    // Remove from waiting queue
    for(int i = highestPriorityIndex; i < mutex->waitingCount - 1; i++) {
      mutex->waitingTasks[i] = mutex->waitingTasks[i + 1];
    }
    mutex->waitingCount--;
    mutex->waitingTasks[mutex->waitingCount] = invalidTask;
    
    task[highestPriorityTask].state = taskReady; //// Wake up task
    
    __asm volatile("cpsie i");
    SCB_ICSR |= PENDSV_SET;
    return;
  }
  else{
    // No waiting tasks, just unlock
    mutex->isLocked = 0;
    mutex->ownerTask = invalidTask;
    __asm volatile("cpsie i");
  }
}

void mutexInheritPriority(mutexTypedef* mutex){
  task[mutex->ownerTask].priority = getHighestWaitingTasksPriority(mutex);
}
void mutexRestorePriority(mutexTypedef* mutex){
  task[mutex->ownerTask].priority = mutex->originalPriority;
}
unsigned char getHighestWaitingTasksPriority(mutexTypedef* mutex){
  unsigned char highestPriority = 255;
  for(int i = 0; i < mutex->waitingCount; i++){
    if(task[mutex->waitingTasks[i]].priority < highestPriority){
      highestPriority = task[mutex->waitingTasks[i]].priority;
    }
  }
  return highestPriority;
}
