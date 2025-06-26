#include"queue.h"
#include<string.h>

//Management functions
void queueInit(queueTypedef* queue, unsigned char maxSize){
  if(maxSize > QUEUE_MAX_SIZE) maxSize = QUEUE_MAX_SIZE; 
  queue->head = 0;
  queue->tail = 0;
  queue->count = 0; 
  queue->maxSize = maxSize;
  
  queue->sendWaitingCount = 0;
  queue->receiveWaitingCount = 0;
  for(int i = 0; i < maxTask; i++){
    queue->sendWaitingTasks[i] = invalidTask;
    queue->receiveWaitingTasks[i] = invalidTask;
  }
  
  // Clear buffer
  memset(queue->buffer, 0, sizeof(queue->buffer));
}

queueResult queueSend(queueTypedef* queue, void* item, unsigned long timeout){
  __asm volatile("cpsid i"); // Disable interrupts
  
  //if queue is not full, send immediately
  if(queue->count < queue->maxSize){
   //copy data to queue
   memcpy(queue->buffer[queue->tail], item, QUEUE_ITEM_SIZE);
   queue->tail = (queue->tail + 1) % queue->maxSize;
   queue->count++;
   //wake up any tasks waiting to receive
   if(queue->receiveWaitingCount > 0) {
       queueWakeWaitingTasks(queue->receiveWaitingTasks, &queue->receiveWaitingCount);
   } 
   __asm volatile("cpsie i"); // Enable interrupts
   return QUEUE_OK;
  }
  
  //queue is full
  if(timeout == 0) {
    __asm volatile("cpsie i");
    return QUEUE_FULL;
  }
  task[currentTask].state = taskDelayed;
  task[currentTask].delayTicks = timeout;
  task[currentTask].wakeupTick = systemTick + timeout;
  //add to waiting list
  queueAddWaitingTask(queue->sendWaitingTasks, &queue->sendWaitingCount, currentTask);  
  __asm volatile("cpsie i");
  SCB_ICSR |= PENDSV_SET;
  
  //When we get here, either timeout or queue has space
  __asm volatile("cpsid i");
    
  //Check if we can send now (after being woken up)
  if(queue->count < queue->maxSize) {
    memcpy(queue->buffer[queue->tail], item, QUEUE_ITEM_SIZE);
    queue->tail = (queue->tail + 1) % queue->maxSize;
    queue->count++;
        
    //Wake up any tasks waiting to receive
    if(queue->receiveWaitingCount > 0) {
      queueWakeWaitingTasks(queue->receiveWaitingTasks, &queue->receiveWaitingCount);
    }
        
    __asm volatile("cpsie i");
    return QUEUE_OK;
  }
    
    __asm volatile("cpsie i");
    return QUEUE_FULL; // Timeout occurred
}

queueResult queueReceive(queueTypedef* queue, void* item, unsigned long timeout){
  __asm volatile("cpsid i"); 
  // If queue is not empty, receive immediately
  if(queue->count > 0) {
    //copy data from queue
    memcpy(item, queue->buffer[queue->head], QUEUE_ITEM_SIZE);
    queue->head = (queue->head + 1) % queue->maxSize;
    queue->count--;
    // Wake up any tasks waiting to send
    if(queue->sendWaitingCount > 0) {
      queueWakeWaitingTasks(queue->sendWaitingTasks, &queue->sendWaitingCount);
    }
        
    __asm volatile("cpsie i"); // Enable interrupts
    return QUEUE_OK;
  }
    
  //queue is empty
  if(timeout == 0) {
    __asm volatile("cpsie i");
    return QUEUE_EMPTY;
  } 
  //block current task with timeout
  task[currentTask].state = taskDelayed;
  task[currentTask].delayTicks = timeout;
  task[currentTask].wakeupTick = systemTick + timeout;
  //add to waiting list
  queueAddWaitingTask(queue->receiveWaitingTasks, &queue->receiveWaitingCount, currentTask);
  __asm volatile("cpsie i");
  
  //trigger context switch
  SCB_ICSR |= PENDSV_SET;
  
  //when we get here, either timeout or queue has data
  __asm volatile("cpsid i");
    
  //check if can receive now (after being woken up)
  if(queue->count > 0) {
    memcpy(item, queue->buffer[queue->head], QUEUE_ITEM_SIZE);
    queue->head = (queue->head + 1) % queue->maxSize;
    queue->count--;
  
  // Wake up any tasks waiting to send
    if(queue->sendWaitingCount > 0) {
      queueWakeWaitingTasks(queue->sendWaitingTasks, &queue->sendWaitingCount);
    }  
    __asm volatile("cpsie i");
    return QUEUE_OK;
  }
    
  __asm volatile("cpsie i");
  return QUEUE_EMPTY; // Timeout occurred
}
queueResult queueSendFromISR(queueTypedef* queue, void* item){
  // ISR version - no blocking, no timeout
   if(queue->count >= queue->maxSize) {
     return QUEUE_FULL;
   }
   
   // Copy data to queue
   memcpy(queue->buffer[queue->tail], item, QUEUE_ITEM_SIZE);
   queue->tail = (queue->tail + 1) % queue->maxSize;
   queue->count++;
   
   // Wake up any tasks waiting to receive
   if(queue->receiveWaitingCount > 0) {
     queueWakeWaitingTasks(queue->receiveWaitingTasks, &queue->receiveWaitingCount);
     needContextSwitch = 1; // Request context switch
   }
   
   return QUEUE_OK;
}
queueResult queueReceiveFromISR(queueTypedef* queue, void* item){
    // ISR version - no blocking, no timeout
  if(queue->count == 0) {
    return QUEUE_EMPTY;
  }
  
  // Copy data from queue
  memcpy(item, queue->buffer[queue->head], QUEUE_ITEM_SIZE);
  queue->head = (queue->head + 1) % queue->maxSize;
  queue->count--;
  
  // Wake up any tasks waiting to send
  if(queue->sendWaitingCount > 0) {
    queueWakeWaitingTasks(queue->sendWaitingTasks, &queue->sendWaitingCount);
    needContextSwitch = 1; // Request context switch
  }
  
  return QUEUE_OK;
}

//status functions
unsigned char queueGetCount(queueTypedef* queue){
  return queue->count;
}
unsigned char queueGetFreeSpace(queueTypedef* queue){
  return queue->maxSize - queue->count;
}
unsigned char queueIsFull(queueTypedef* queue){
  if(queue->count >= queue->maxSize) return 1;
  return 0;
}
unsigned char queueIsEmpty(queueTypedef* queue){
  return queue->count == 0;
}

// Helper functions
void queueWakeWaitingTasks(unsigned char* waitingTasks, unsigned char* waitingCount){
  unsigned char taskToWake = waitingTasks[0];
  queueRemoveWaitingTask(waitingTasks, waitingCount, taskToWake);
  if(taskToWake < maxTask && task[taskToWake].isUsed){
    task[taskToWake].state = taskReady;
    task[taskToWake].delayTicks = 0;
    task[taskToWake].wakeupTick = 0;
    SCB_ICSR |= PENDSV_SET;
  }     
}
void queueAddWaitingTask(unsigned char* waitingTasks, unsigned char* waitingCount, unsigned char taskId){
  waitingTasks[*waitingCount] = taskId;
  *waitingCount++;
}
void queueRemoveWaitingTask(unsigned char* waitingTasks, unsigned char* waitingCount, unsigned char taskId){
  for(int i = 0; i < *waitingCount; i++){
    if(waitingTasks[i] == taskId) {
      // Shift remaining tasks
      for(int j = i; j < *waitingCount - 1; j++){
        waitingTasks[j] = waitingTasks[j + 1];
      }
      (*waitingCount)--;
      waitingTasks[*waitingCount] = invalidTask;
      break;
    }
  }
}
