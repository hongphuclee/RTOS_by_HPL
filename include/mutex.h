#ifndef MUTEX_H_
#define MUTEX_H_

#include"rtos_config.h"
#include"hplRTOS.h"

typedef struct{
  unsigned char isLocked;               // 0 = unlocked, 1 = locked
  unsigned char ownerTask;              // task index that owns the mutex
  unsigned char originalPriority;       // original prority of owner task
  unsigned char waitingTasks[maxTask];   // queue
  unsigned char waitingCount;           // number of queue
}mutexTypedef;

void mutexInit(mutexTypedef* mutex);
void mutexLock(mutexTypedef* mutex);
void mutexUnlock(mutexTypedef* mutex);

void mutexInheritPriority(mutexTypedef* mutex);
void mutexRestorePriority(mutexTypedef* mutex);
unsigned char getHighestWaitingTasksPriority(mutexTypedef* mutex);

#endif