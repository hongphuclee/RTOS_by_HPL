#ifndef QUEUE_H_
#define QUEUE_H_

#include"rtos_config.h"
#include"hplRTOS.h"


#define QUEUE_MAX_SIZE  16      //maximum number of items in queue
#define QUEUE_ITEM_SIZE 32      //size of each item in bytes

typedef enum{
  QUEUE_OK = 0,
  QUEUE_FULL,
  QUEUE_EMPTY,
  QUEUE_ERROR
} queueResult;

typedef struct{
  unsigned char buffer[QUEUE_MAX_SIZE][QUEUE_ITEM_SIZE]; // circular buffer
  unsigned char head;   //index of first item
  unsigned char tail;   //index of next free slot
  unsigned char count;  //current number of item, prevent tail > head in circular buffer
  unsigned char maxSize;
  
  unsigned char sendWaitingTasks[maxTask];
  unsigned char sendWaitingCount;
  unsigned char receiveWaitingTasks[maxTask];
  unsigned char receiveWaitingCount;
} queueTypedef;

//Management functions
void queueInit(queueTypedef* queue, unsigned char maxSize);
queueResult queueSend(queueTypedef* queue, void* item, unsigned long timeout);
queueResult queueReceive(queueTypedef* queue, void* item, unsigned long timeout);
queueResult queueSendFromISR(queueTypedef* queue, void* item);
queueResult queueReceiveFromISR(queueTypedef* queue, void* item);

//status functions
unsigned char queueGetCount(queueTypedef* queue);
unsigned char queueGetFreeSpace(queueTypedef* queue);
unsigned char queueIsFull(queueTypedef* queue);
unsigned char queueIsEmpty(queueTypedef* queue);

// Helper functions
void queueWakeWaitingTasks(unsigned char* waitingTasks, unsigned char* waitingCount);
void queueAddWaitingTask(unsigned char* waitingTasks, unsigned char* waitingCount, unsigned char taskId);
void queueRemoveWaitingTask(unsigned char* waitingTasks, unsigned char* waitingCount, unsigned char taskId);


#endif