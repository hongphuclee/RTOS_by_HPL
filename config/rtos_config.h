#ifndef _RTOS_CONFIG_H_
#define _RTOS_CONFIG_H_

// SCB_ICSR |= PENDSV_SET => system call interrupt
#define SCB_ICSR        *((volatile unsigned long*) 0xE000ED04) 
#define PENDSV_SET      (1 << 28)


#define maxTask                         8

#define IDLE_TASK_INDEX                 (maxTask - 1)

#define invalidTask                     0xFF

#define PRIORITY_HIGH                   0
#define PRIORITY_ABOVE_NORMAL           2
#define PRIORITY_NORMAL                 5
#define PRIORITY_BELOW_NORMAL           7
#define PRIORITY_LOW                    10

#endif

