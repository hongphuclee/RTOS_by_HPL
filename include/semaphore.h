#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_
#include"rtos_config.h"
typedef struct{
  unsigned char count;                  //current number of tasks remaining
  unsigned char maxCount;               //max task can use resource
  unsigned char waitingTasks[maxTask];  // queue of waiting tasks
  unsigned char waitingCount;           //number of waiting tasks
} semaphoreTypedef;

void semaphoreInit(semaphoreTypedef* sem, unsigned char count, unsigned char maxCount);
void semaphoreTake(semaphoreTypedef* sem);
void semaphoreGive(semaphoreTypedef* sem);


#endif