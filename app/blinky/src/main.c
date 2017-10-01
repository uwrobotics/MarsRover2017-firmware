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
//#include "stm32f072b_discovery.h"
#include "uart_lib.h"

int main(void)
{
	HAL_Init();

	RCC_ClkInitTypeDef RCC_ClkStruct;
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

	/* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
	RCC_ClkStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	/* TODO: handle clock config failure */
	(void)HAL_RCC_ClockConfig(&RCC_ClkStruct, FLASH_LATENCY_1);


	/* Enable appropriate peripheral clocks */
	__SYSCFG_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin = GPIO_PIN_10,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH
	};

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitTypeDef LimitSwitch_InitStruct = {
        .Pin            = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
        .Mode           = GPIO_MODE_INPUT,
        .Pull           = GPIO_NOPULL,
        .Speed          = GPIO_SPEED_FREQ_HIGH
    };

    HAL_GPIO_Init(GPIOC, &LimitSwitch_InitStruct);

	// UART_LIB_INIT();
	// uint8_t a[] = "Hello";
	// UART_LIB_PRINT_CHAR_ARRAY(a, sizeof(a));

	// int b = 1234;
	// UART_LIB_PRINT_INT(b);

	// double c = 555.123;
	// UART_LIB_PRINT_DOUBLE(c);
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	while(1) {
		 //Toggle the state of pin PC9
		 if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == 1)
		 {
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
		 }
		 else
		 {
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
		 }
		//HAL_Delay(500);
	}
}
