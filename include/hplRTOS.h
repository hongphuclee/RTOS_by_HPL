
#ifndef _HPLRTOS_H_
#define _HPLRTOS_H_

#include"rtos_config.h"
#include"task.h"

/* taskDelay needs to be defined as a macro because if it were a normal function,
R0 would be overwritten by ticks when you call it*/

#define taskDelay(ticks) do {                                                           \
    __asm volatile(                                                                     \
        "PUSH {R0-R11, LR}      \n\t"     /* Save CURRENT R0 (before function call) */  \
        "MOV R0, %0             \n\t"     /* Load ticks into R0 */                      \
        "SVC #0                 \n\t"     /* Make SVC call */                           \
        "POP {R0-R11, LR}       \n\t"     /* Restore original R0 */                     \
        :                                                                               \
        : "i"(ticks)                                                                    \
        : "memory"                                                                      \
    ); \
} while(0)


void taskCreate(void (*taskFunction)(void), const char* taskName);
void taskCreateWithPriority(void (*taskFunction)(void), const char* taskName, unsigned char priority);
void taskStartScheduler(void);
void rtosInit();

void initTaskStack(unsigned char taskIndex);
unsigned char getNextTask(void);

void idleTask(void);
void rtosInitSafe();

extern taskTypedef task[maxTask];
extern unsigned char totalTask;
extern unsigned char currentTask;
extern volatile unsigned long systemTick;
extern unsigned char needContextSwitch;

#endif