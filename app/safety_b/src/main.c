/**
  ******************************************************************************
  * @file    main.c
  * @author  Jake Fisher
  * @version V1.0
  * @date    12-May-2017
  * @brief   UWRT safety board main function (Microcontroller B).
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include "safety_master_lib.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
#endif

/* GPIO */
#define GPIO_A_OUTPUTS                /* None */
#define GPIO_A_INPUTS                 /* None */
#define GPIO_B_OUTPUTS                (GPIO_PIN_0 | GPIO_PIN_4 | GPIO_PIN_5)
#define GPIO_B_INPUTS                 /* None */
#define GPIO_C_OUTPUTS                (GPIO_PIN_0 | GPIO_PIN_4 | GPIO_PIN_8 | GPIO_PIN_13)
#define GPIO_C_INPUTS                 /* None */
#define GPIO_D_OUTPUTS                (GPIO_PIN_2)
#define GPIO_D_INPUTS                 /* None */

void GPIO_Init(){
    /* Ensure peripheral-specific (SPI, I2C, UART) GPIOs get set elsewhere */

    //__GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_B_OUT_InitStruct, GPIO_C_OUT_InitStruct, GPIO_D_OUT_InitStruct;
    
    GPIO_B_OUT_InitStruct.Pin = GPIO_B_OUTPUTS;
    GPIO_B_OUT_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_B_OUT_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_B_OUT_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_B_OUT_InitStruct);
    GPIO_C_OUT_InitStruct.Pin = GPIO_C_OUTPUTS;
    GPIO_C_OUT_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_C_OUT_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_C_OUT_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_C_OUT_InitStruct);
    GPIO_D_OUT_InitStruct.Pin = GPIO_D_OUTPUTS;
    GPIO_D_OUT_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_D_OUT_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_D_OUT_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_D_OUT_InitStruct);

    GPIO_Set(&B0);
    GPIO_Set(&B4);
    GPIO_Set(&B5);
    GPIO_Set(&C0);
    GPIO_Set(&C4);
    GPIO_Set(&C8);
    GPIO_Set(&C13);
    GPIO_Set(&D2);
}

 
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


void Initialization(void){
	HAL_Init();
	Oscillator_Init();
	Clock_Init();
	GPIO_Init();
}

int main(void){
    Initialization();
    while(1);
}
