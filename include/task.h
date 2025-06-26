#ifndef _TASK_H_
#define _TASK_H_

typedef enum {
  taskReady = 0,
  taskRunning,
  taskDelayed,
  taskBlockedSemaphore,
  taskBlockedMutex
} taskState;

typedef struct {
  struct {
    unsigned long R4;
    unsigned long R5;
    unsigned long R6;
    unsigned long R7;
    unsigned long R8;
    unsigned long R9;
    unsigned long R10;
    unsigned long R11;
    unsigned long SP;
  } CPU;

  unsigned long stack[256];     // 1KB stack
  void (*taskFunction)(void);   // pointer to task function
  char taskName[16]; 
  unsigned char isUsed;         // flag: 1 if this task is used
  unsigned char priority;
  
  taskState state;
  unsigned long delayTicks;
  unsigned long wakeupTick;
} taskTypedef;

#endif
