#include"LHP_stm32f103.h"
#include"hplRTOS.h"
#include"semaphore.h"

semaphoreTypedef val;

volatile unsigned long resource = 0;

void HardFault_Handler(){
    return;
}

void task0(){
  int x = 6;
  int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  while(1){
   x++;
   x++;
   x++;
   for(int i = 0; i < 10; i++){
     a[i]++;
   }
   taskDelay(10);
  }
}

void task1(){
    while(1){
      
  } 
}

void main(){   
    rtosInit();
    rtosInitSafe();
    
    semaphoreInit(&val, 1, 1);
 
    taskCreate(task0, "a");
    taskCreate(task1, "b");
    
    
    taskStartScheduler();

    while(1){}
}
