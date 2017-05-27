/**
  ******************************************************************************
  * @file    main.c
  * @author  Jake Fisher
  * @version V1.0
  * @date    13-May-2017
  * @brief   Testing program.
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include "safety_slave_lib.h"
#include <stdlib.h>

#ifdef __cplusplus
 extern "C" {
#endif

void Oscillator_Init(){
	RCC_OscInitTypeDef RCC_OscStruct;
	/* Select HSI48 Oscillator as PLL source */
	RCC_OscStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
	RCC_OscStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
	RCC_OscStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
	RCC_OscStruct.PLL.PLLMUL = RCC_PLL_MUL2;
	
	/* TODO: handle oscillator config failure */
	(void)HAL_RCC_OscConfig(&RCC_OscStruct);
}

void Clock_Init(){
	/* Enable appropriate peripheral clocks */
	__SYSCFG_CLK_ENABLE();
	
	RCC_ClkInitTypeDef RCC_ClkStruct;
	/* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
	RCC_ClkStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	
	/* TODO: handle clock config failure */
	(void)HAL_RCC_ClockConfig(&RCC_ClkStruct, FLASH_LATENCY_1);
}

int main(void){
    HAL_Init();
	Oscillator_Init();
	Clock_Init();
    Heartbeat_Init(VISION_HEARTBEAT_PRIORITY);
    while(1);
}

#ifdef __cplusplus
}
#endif