  RSEG  text : CODE
  
  EXTERN     currentTask   
  EXTERN     maxTask   
  EXTERN     totalTask   
  EXTERN     getNextTask
  EXTERN     task
  EXTERN     updateDelayedTasks
  EXTERN     systemTick
  EXTERN     taskDelayHandler
  
  
  PUBLIC     SysTick_Handler
  PUBLIC     SVC_Handler
  PUBLIC     PendSV_Handler
  
  SVC_Handler:
  // Get parameter from R0 (ticks value)
  PUSH  {LR}
  BL    taskDelayHandler    // Call C handler with ticks in R0
  POP   {LR}
  BX    LR
  
 //---------------------------------------------------------------------------- 
  PendSV_Handler:
  // Get current task
  LDR   R0, =currentTask
  LDR   R0, [R0]
  
  // Calculate current task address
  LDR   R1, =task
  MOV   R2, #1092
  MUL   R3, R0, R2
  ADD   R1, R1, R3
  
  // Save current task context
SAVE_CPU:
  STR   R4,  [R1, #4 * 0]
  STR   R5,  [R1, #4 * 1]
  STR   R6,  [R1, #4 * 2]
  STR   R7,  [R1, #4 * 3]
  STR   R8,  [R1, #4 * 4]
  STR   R9,  [R1, #4 * 5]
  STR   R10, [R1, #4 * 6]
  STR   R11, [R1, #4 * 7]
  STR   SP,  [R1, #4 * 8]
  
  // Get next task
  PUSH  {LR}
  BL    getNextTask
  POP   {LR}
  
  // R0 now contains next task index
  LDR   R1, =currentTask
  STR   R0, [R1] // Update currentTask
  
  //Load next task context
  LDR   R1, =task
  //Calculate task address: task{currentTask}
  //Size = 9*4(CPU) + 256*4 (stack) + 4 (taskFunction)
  // + 16 (taskName) + 4 (isUsed, priority) (2 + 2 padding) + 3*4(taskDelay) = 1096
  MOV   R2, #1092
  MUL   R3, R0, R2
  ADD   R1, R1, R3
  //LOAD NEXT CPU
LOAD_CPU:
  LDR   R4,  [R1, #4 * 0]
  LDR   R5,  [R1, #4 * 1]
  LDR   R6,  [R1, #4 * 2]
  LDR   R7,  [R1, #4 * 3]
  LDR   R8,  [R1, #4 * 4]
  LDR   R9,  [R1, #4 * 5]
  LDR   R10, [R1, #4 * 6]
  LDR   R11, [R1, #4 * 7]
  LDR   SP,  [R1, #4 * 8] 
  
  BX    LR
  //---------------------------------------------------------------------------
  
  SysTick_Handler:
  //R0 - R1 - R2 - R3 - R12 - LR - PC - EPSR
  
  //Increment system tick counter
  LDR   R0, =systemTick
  LDR   R1, [R0]
  ADD   R1, R1, #1
  STR   R1, [R0]
  
  // updateDelayedTasks
  PUSH  {LR}
  BL    updateDelayedTasks
  POP   {LR}
  
  // Trigger PendSV for context switch
  LDR   R0, =0xE000ED04  // SCB_ICSR
  LDR   R1, [R0]
  ORR   R1, R1, #0x10000000  // Set PendSV bit
  STR   R1, [R0]

SYSTICK_EXIT:
  BX    LR
  

  END