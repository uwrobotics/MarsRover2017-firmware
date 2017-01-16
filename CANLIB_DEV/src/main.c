/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  *
  *
  * This file exists to demonstrate basic use of the CAN library for Waterloo
  * Mars Rover. It's a very contrived and simple 'blink' example, basically.
  * This is meant to run in loopback mode (on one dev board).
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include "canlib.h"
#include "stm32f072b_discovery.h"

//Globals that we send
//  Any variables or arrays used for transmission can
//  be local as well
uint8_t array[8] = {1,2,3,4,5,6,7,8};
uint32_t number1 = 32423432;


//Globals used for receiving values within the CAN callback function
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
	//Always call. Enables prefetch and calls above function
	HAL_Init();

	setup_test();

	/* First relevant thing: Initialize CAN communication handler
	 * The input parameter to CANLIB_Init() is the node ID you want this node to have.
	 * In this case, we start with a node ID of 1
	 * This demo was developed with 1 board, so loopback mode is ON
	 */
	switch(CANLIB_Init(1, CANLIB_LOOPBACK_ON)){
		case 0:
			//Initialization of CAN handler successful
			break;
		case -1:
			//Initialization of CAN handler not successful
			break;
		default:
			break;
	}

	//Using CANLIB_AddFilter() will allow you to specify who this node can receive messages from
	// according to the sender node ID. This ensures less CAN interrupts
	//Without calling this function, no messages will be received
	CANLIB_AddFilter(10);
	CANLIB_AddFilter(3);
	CANLIB_AddFilter(4);

	HAL_Delay(1000);

	while(1){

		//Send the byte array from above, return if unsuccessful
		//Notice that all 8 bytes of the byte array are being sent, and
		//the node ID of this node is changed to 3 before this message is sent
		if(CANLIB_SendBytes(array, CANLIB_DLC_ALL_BYTES, 3)){
			return -1;
		}

		HAL_Delay(100);
		//If the bits received are what we sent, light the green LED on PC9
		// else, light the red LED on PC8
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

		//Here, we change the node ID to 10, put a 32 bit uint in the 4 LSBs of the
		// CAN data array (indicated by CANLIB_INDEX_0), and send the first 4 bytes.
		CANLIB_ChangeID(10);
		CANLIB_Tx_SetUint(number1, CANLIB_INDEX_0);
		CANLIB_Tx_SendData(CANLIB_DLC_FOUR_BYTES);
		HAL_Delay(100);

		//If the number received from the callback is the same as the one sent,
		// turn on the green LED.
		// Else, turn on the red LED.
		if (received_number == number1){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
		}

		HAL_Delay(5000);

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);

		HAL_Delay(1000);

		//Change the ID to 5 and send the SAME 4 BYTES
		//This is because the CAN data array is PERSISTENT, so number1 is sent again,
		// just with a different node ID
		CANLIB_ChangeID(5);
		CANLIB_Tx_SendData(4);

		//The green LED will turn on if the number received in the default case is equal to
		// the number sent. This WOULD run if the message above triggered an Rx interrupt, as
		// there is no case to receive messages from a node with ID of 5. However, the
		// interrupt never fires. This is the expected behaviour, because we never set up a filter
		// to receive messages from nodes with ID of 5!
		// Therefore, the red LED will turn on
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

	return 0;
}

//This has NOTHING to do with the CAN library, but something similar is done in all
// All STM32Cube code has similar initialization: First the clocks, peripheral clocks
// then GPIOs and other things are initialized
// Here, we set up clocks and PC8 and PC9 as outputs for they are on LEDs that we want
// to flicker.
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
	__GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin = GPIO_PIN_9 | GPIO_PIN_8,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH
	};

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

//This is the CAN Rx function to implement
//Do anything you want in here, but it makes sense to do something similar to below:
// Switch on the node ID of who sends the message, and interpret
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

