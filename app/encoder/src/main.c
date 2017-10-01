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
#include "encoderlib.h"

// uint8_t encA = 0;
// uint8_t encB = 0;

//interrupt method for reading encoders, switched to using hardware timers
//https://www.rcgroups.com/forums/showthread.php?969957-Good-C-code-for-Quadrature-encoding
// update must be called at least once per state change of aNow or bNow
// void update( uint8_t aNow, uint8_t bNow){
// 	static uint8_t aPrev;
// 	if( !aPrev && aNow ) {
// 		if( bNow )
// 			encoderPos++;
// 		else
// 			encoderPos--;
// 	}
// 	aPrev = aNow;
// }

// void EXTI2_3_IRQHandler(void){
// 	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); //Toggle the state of pin PC9
// 	encB = !encB;
// 	update(encA,encB);
//   EXTI->PR |= EXTI_PR_PR3;
// }

// void EXTI4_15_IRQHandler(void){
// 	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); //Toggle the state of pin PC9
// 	encA = !encA;
// 	update(encA,encB);
// 	EXTI->PR |= EXTI_PR_PR15;

// }

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
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();

	//init LEDS
	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin = GPIO_PIN_9 | GPIO_PIN_8,// | GPIO_PIN_7 | GPIO_PIN_6,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH
	};
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	//init interrupt pins
	// GPIO_InitStruct.Pin = GPIO_PIN_3;
	// GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	// GPIO_InitStruct.Pull = GPIO_NOPULL;
	// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	// HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// GPIO_InitStruct.Pin = GPIO_PIN_15;
	// GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	// GPIO_InitStruct.Pull = GPIO_NOPULL;
	// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	// HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// NVIC_SetPriority(EXTI2_3_IRQn, 1); //4. Set Priority to 1
	// NVIC_EnableIRQ(EXTI2_3_IRQn);  // 5. Enable EXTI0_1 interrupt in NVIC (do 4 first)

	// NVIC_SetPriority(EXTI4_15_IRQn, 1); //4. Set Priority to 1
	// NVIC_EnableIRQ(EXTI4_15_IRQn);  // 5. Enable EXTI0_1 interrupt in NVIC (do 4 first)

	EncoderLib_Init(ENCODER1);
	EncoderLib_Init(ENCODER2);
	long encoderPos1 = 0;
	long encoderPos2 = 0;


//Pin C6 is red, C7 is blue
	while(1) {
		encoderPos1 = EncoderLib_ReadCount(ENCODER1);
		encoderPos2 = EncoderLib_ReadCount(ENCODER2);
		if(encoderPos2 > 150)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		}
		else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		}
	}
}
