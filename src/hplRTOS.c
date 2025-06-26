#include"hplRTOS.h"


static unsigned char lastRunTask = IDLE_TASK_INDEX;
taskTypedef task[maxTask];
unsigned char totalTask;
unsigned char currentTask;
volatile unsigned long systemTick = 0;
  //R0 - R1 - R2 - R3 - R12 - LR - PC - EPSR

void idleTask(void) {
    while(1) {
    }
}

//initialization idle Task
void rtosInitSafe(){
    task[IDLE_TASK_INDEX].taskFunction = idleTask;
    task[IDLE_TASK_INDEX].isUsed = 1;
    task[IDLE_TASK_INDEX].state = taskReady;  // ALWAYS READY
    task[IDLE_TASK_INDEX].priority = 255;     // Lowest priority
    
    const char* name = "IDLE";
    int j = 0;
    while (name[j] != '\0' && j < 15) {
        task[IDLE_TASK_INDEX].taskName[j] = name[j];
        j++;
    }
    task[IDLE_TASK_INDEX].taskName[j] = '\0';

    initTaskStack(IDLE_TASK_INDEX);
}

void rtosInit(){
  for(int i = 0; i < maxTask; i++){
    task[i].isUsed = 0;
    task[i].taskFunction = 0;
    task[i].priority = PRIORITY_NORMAL;
  }
  currentTask = 0;
  totalTask = 0;
  
   // Set PendSV to lowest priority
    *((volatile unsigned long*) 0xE000ED20) |= 0x00FF0000;
    
    // Set SVC to high priority 
    *((volatile unsigned long*) 0xE000ED1C) |= 0x00000000; // SVC Priority = 0
}


void taskCreateWithPriority(void (*taskFunction)(void), const char* taskName, unsigned char priority){
  for(int i = 0; i < maxTask; i++){
    if(task[i].isUsed == 0){
      task[i].taskFunction = taskFunction;
      task[i].isUsed = 1;
      task[i].state = taskReady;  
      task[i].priority = priority;
      task[i].wakeupTick = 0; 
      task[i].delayTicks = 0;         
      totalTask++;
      
      // Copy task name
      int j = 0;
      while(taskName[j] != '\0' && j < 15) {
        task[i].taskName[j] = taskName[j];
        j++;
      }
      task[i].taskName[j] = '\0';
      
      initTaskStack(i);
      return;
    }
  }
}

void taskCreate(void (*taskFunction)(void), const char* taskName){
  taskCreateWithPriority(taskFunction, taskName, PRIORITY_NORMAL);
}

void initTaskStack(unsigned char indexTask){
  task[indexTask].stack[247] = 0; // R0
  task[indexTask].stack[248] = 0; // R1
  task[indexTask].stack[249] = 0; // R2
  task[indexTask].stack[250] = 0; // R3
  task[indexTask].stack[251] = 0; // R12
  task[indexTask].stack[252] = 0; // LR
  task[indexTask].stack[253] = (unsigned long)task[indexTask].taskFunction; // ptrFunction -> PC
  task[indexTask].stack[254] = 0x01000000; // EPSR
  task[indexTask].stack[255] = 0; //buffer for fist task's sp
  task[indexTask].CPU.SP = (unsigned long)&task[indexTask].stack[247];
}

void taskStartScheduler(void){
  if(totalTask > 0){
    unsigned char highestPriority = 255;
    for(int i = 0; i < maxTask; i++){
      if(task[i].isUsed == 1 && task[i].state == taskReady && task[i].priority < highestPriority){
        highestPriority = task[i].priority;
        currentTask = i;
      }
    }
  }                                    
  SysTick_Init(1);          
  // first task's SP -> CPU's SP
  unsigned long *stack_addr = &task[currentTask].stack[255];
  __asm("MOV SP, %0" : : "r"(stack_addr));
  task[currentTask].taskFunction();                                      
}

//select task based on priority, and use round-robin scheduling among tasks whith the same priority
unsigned char getNextTask(void){
  //find highest priority
  unsigned char highestPriority = 255;
  for(int i = 0; i < maxTask; i++){
    if(task[i].isUsed == 1 && task[i].state == taskReady && task[i].priority < highestPriority){
      highestPriority = task[i].priority; 
    }
  }
  
  // if not found
  if(highestPriority == 255){
    return IDLE_TASK_INDEX;
  }
  
  //round robin
  for(int i=(lastRunTask+1)%maxTask; i!=lastRunTask; i=(i+1)%maxTask){
    if(task[i].isUsed == 1 && task[i].state == taskReady && task[i].priority == highestPriority){
      lastRunTask = i;
      return i;
    }
  }
  return lastRunTask;
}



void taskDelayHandler(unsigned long ticks) {
    if(ticks == 0) return;
    
    // Set current task to blocked state
    task[currentTask].state = taskDelayed;
    task[currentTask].delayTicks = ticks;
    task[currentTask].wakeupTick = systemTick + ticks;
    
    // Trigger PendSV for context switch
    SCB_ICSR |= PENDSV_SET;
}

void updateDelayedTasks(void){
  for(int i = 0; i < maxTask; i++){
    if(task[i].isUsed == 1 && task[i].state == taskDelayed){
      if(systemTick > task[i].wakeupTick){
        task[i].state = taskReady;
        task[i].delayTicks = 0;
        task[i].wakeupTick = 0; 
      }
    }
  }
}