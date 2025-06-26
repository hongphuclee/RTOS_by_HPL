#include"LHP_stm32f103.h"
#include"hplRTOS.h"

void HardFault_Handler(){
    return;
}

void task0(){
  while(1){
    __asm("MOV R0, #7");
    __asm("MOV R1, #8");
    __asm("MOV R2, #9");
    __asm("MOV R3, #10");
    __asm("MOV R4, #11");
    __asm("MOV R5, #12");
    __asm("MOV R6, #13");
    __asm("MOV R7, #14");
    __asm("MOV R8, #15");
    __asm("MOV R9, #16");
    __asm("MOV R10, #17");
    __asm("MOV R11, #18");
    __asm("MOV R12, #19");
    __asm("NOP");
    taskDelay(2);
    __asm("NOP");
  }   
}

void task1(){
    while(1){
    __asm("NOP");
    taskDelay(2);
    __asm("NOP");
  } 
}

void task2(){
    while(1){
    __asm("NOP");
    taskDelay(2);
    __asm("NOP");
  } 
}

void task3(){
    while(1){
    __asm("NOP");
    taskDelay(2);
    __asm("NOP");
  } 
}

void main(){
    rtosInit();
    rtosInitSafe();
 
    taskCreateWithPriority(task0, "a", PRIORITY_NORMAL);
    taskCreateWithPriority(task1, "b", PRIORITY_LOW);
    taskCreateWithPriority(task2, "c", PRIORITY_HIGH);
    taskCreateWithPriority(task3, "d", PRIORITY_LOW);
    
    taskStartScheduler();

    while(1){}
}