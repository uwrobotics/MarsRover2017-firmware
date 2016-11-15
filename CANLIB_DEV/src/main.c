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
#include "canlib.h"
#include "stm32f072b_discovery.h"

//Tx testing stuff
uint8_t array[8] = {1,2,3,4,5,6,7,8};
uint32_t number1 = 32423432;


//Rx Testing stuff
uint8_t received_bytes[8];
uint32_t received_number;
uint32_t wrongly_received_number;

void setup_test();

void HAL_MspInit(void) {
    /* System interrupt init*/
    /* SysTick_IRQn interrupt configuration */
    (void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    //Must be set to priority of 1 or else will have higher priority than CAN IRQ
    HAL_NVIC_SetPriority(SysTick_IRQn, 1, 1);
}

int main(void)
{
	HAL_Init();

	setup_test();

	(void)CANLIB_Init(1);
	CANLIB_AddFilter(10);
	CANLIB_AddFilter(3);
	CANLIB_AddFilter(4);

	HAL_Delay(1000);

	while(1){

		if(CANLIB_SendBytes(array, 8, 3)){
			return -1;
		}
		HAL_Delay(100);
		for(int i = 0; i < 8; i++){
			if (received_bytes[i] == i+1){
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);
			} else {
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
			}
		}

		HAL_Delay(5000);

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);

		HAL_Delay(1000);

		CANLIB_ChangeID(10);
		CANLIB_Tx_SetUint(number1, 0);
		CANLIB_Tx_SendData(4);
		HAL_Delay(100);

		if (received_number == number1){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
		}

		HAL_Delay(5000);

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);

		HAL_Delay(1000);

		CANLIB_ChangeID(5);
		CANLIB_Tx_SendData(4);

		if (wrongly_received_number == number1){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
		}

		HAL_Delay(5000);

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);

		HAL_Delay(1000);

	}

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);

	return 0;
}

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
	__GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_9 | GPIO_PIN_8,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH
	};

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void CANLIB_Rx_OnMessageReceived(){
	switch(CANLIB_Rx_GetSenderID()){
		case 3:
			CANLIB_Rx_GetBytes(received_bytes);
			break;
		case 4:
			break;
		case 10:
			received_number = CANLIB_Rx_GetAsUint(0);
			break;
		default:
			wrongly_received_number = CANLIB_Rx_GetAsUint(0);
			break;
	}

}

