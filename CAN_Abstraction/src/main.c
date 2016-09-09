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
#include "can_properties.h"
#include "string.h"

void setup_test();
void lazy_mans_wait(int wait_time);
void test_toggle_pins();

int main(void)
{
		HAL_Init();

		setup_test();

		test_toggle_pins();

		lazy_mans_wait(10000);

		test_toggle_pins();

		switch(CANAbstract_Init(1)){
			case 0:
				break;
			case -1:
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
				return -1;
			case -2:
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
				return -2;
			case -3:
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
				return -3;
			default:
				return -10;
		}

}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//HELPER FUNCTIONS FOR TESTING

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


void setup_test(){
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
	__GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
			.Mode = GPIO_MODE_OUTPUT_OD,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH
	};

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void lazy_mans_wait(int wait_time){
	for (int i = 0; i > wait_time; i++){};
	return;
}

void test_toggle_pins(){
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
}
