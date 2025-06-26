#include"LHP_stm32f103.h"
#include"hplRTOS.h"



void HardFault_Handler(){
    return;
}

void task0(){
    while(1){
        GPIOC.ODR.BIT.B_13 = !GPIOC.ODR.BIT.B_13;
        taskDelay(300);
//        for(int i = 0; i < 50000; i++){};
    }
}

void task1(){
    while(1){
        GPIOC.ODR.BIT.B_14 = !GPIOC.ODR.BIT.B_14;
        taskDelay(300);
//        for(int i = 0; i < 50000; i++){};
    }
}

void task2(){
    while(1){
       GPIOC.ODR.BIT.B_15 = !GPIOC.ODR.BIT.B_15;
       taskDelay(300);
//        for(int i = 0; i < 50000; i++){};
    }
}

void main(){
    rtosInit();
    rtosInitSafe();
 
    taskCreate(task0, "LED13");
    taskCreate(task1, "LED14");
    taskCreate(task2, "LED15");
    
    RCC.APB2_ENR.BIT.IOPCEN = 1;
    GPIO_Mode(&GPIOC, BIT15| BIT13 | BIT14, output_push_pull_10Mhz);
    

    taskStartScheduler();

    while(1){}
}