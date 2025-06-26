#include"hplRTOS.h"
#include"mutex.h"
#include"LHP_stm32f103.h"

volatile unsigned long resource = 0;
mutexTypedef mutex;

void HardFault_Handler(){
    return;
}

void task0(){
  while(1){

    mutexLock(&mutex);
    __asm("NOP");
    taskDelay(6);
    __asm("NOP");
    resource++;
    __asm("NOP");
    mutexUnlock(&mutex);
    __asm("NOP");
  }   
}

void task1(){
    while(1){
    __asm("NOP");
    mutexLock(&mutex);
    __asm("NOP");
    resource++;
    mutexUnlock(&mutex);
    __asm("NOP");
    taskDelay(13);
    __asm("NOP");
  } 
}



void main(){
    
    
    rtosInit();
    rtosInitSafe();
    
    mutexInit(&mutex);
 
    taskCreate(task0, "a");
    taskCreate(task1, "b");
    
//    __asm("NOP");
//    __asm("NOP");
//    SCB_ICSR |= PENDSV_SET;
//    __asm("NOP");
//    __asm("NOP");
    
    
    taskStartScheduler();

    while(1){}
}