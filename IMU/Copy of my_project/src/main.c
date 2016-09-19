/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include "stm32f072b_discovery.h"
			

int main(void)
{
	SystemInit();
	STM_EVAL_LEDInit(LED3);

	STM_EVAL_LEDInit(LED4);

	STM_EVAL_LEDInit(LED5);

	STM_EVAL_LEDInit(LED6);
	int i;
	while(1){
		SystemCoreClockUpdate();
		STM_EVAL_LEDOn(LED3);
		while(i<100000){
			i++;
		}
		i=0;
		STM_EVAL_LEDOff(LED3);
		STM_EVAL_LEDOn(LED4);
		while(i<100000){
			i++;
		}
		i=0;
		STM_EVAL_LEDOff(LED4);
		STM_EVAL_LEDOn(LED5);
		while(i<100000){
			i++;
		}
		i=0;
		STM_EVAL_LEDOff(LED5);
		STM_EVAL_LEDOn(LED6);
		while(i<100000){
			i++;
		}
		i=0;
		STM_EVAL_LEDOff(LED6);
	}
}
