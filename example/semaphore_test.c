#include"LHP_stm32f103.h"
#include"hplRTOS.h"
#include"semaphore.h"

semaphoreTypedef val;

volatile unsigned long resource = 0;

void HardFault_Handler(){
    return;
}

void task0(){
  while(1){

    semaphoreTake(&val);
    __asm("NOP");
    taskDelay(6);
    __asm("NOP");
    resource++;
    __asm("NOP");
    semaphoreGive(&val);
    __asm("NOP");
  }   
}

void task1(){
    while(1){
    __asm("NOP");
    semaphoreTake(&val);
    __asm("NOP");
    resource++;
    semaphoreGive(&val);
    __asm("NOP");
    taskDelay(13);
    __asm("NOP");
  } 
}



void main(){
    
    
    rtosInit();
    rtosInitSafe();
    
    semaphoreInit(&val, 1, 1);
 
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