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

long encoderPos = 0;
uint8_t encA = 0;
uint8_t encB = 0;

TIM_HandleTypeDef htim;


void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim) {
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}


//https://www.rcgroups.com/forums/showthread.php?969957-Good-C-code-for-Quadrature-encoding
// update must be called at least once per state change of aNow or bNow
void update( uint8_t aNow, uint8_t bNow){
	static uint8_t aPrev;
	if( !aPrev && aNow ) {
		if( bNow )
			encoderPos++;
		else
			encoderPos--;
	}
	aPrev = aNow;
}

void EXTI2_3_IRQHandler(void){
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); //Toggle the state of pin PC9
	encB = !encB;
	update(encA,encB);
  EXTI->PR |= EXTI_PR_PR3;
}

void EXTI4_15_IRQHandler(void){
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); //Toggle the state of pin PC9
	encA = !encA;
	update(encA,encB);
	EXTI->PR |= EXTI_PR_PR15;

}

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
			.Pin = GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH
	};
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


	//enable hardware encoding
	htim.Instance = TIM2;
	htim.Init.Prescaler = 0;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.Period = 0xFFFF;
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	TIM_Encoder_InitTypeDef sConfig;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;

	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if( HAL_TIM_Encoder_Init(&htim, &sConfig) != HAL_OK){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	}

	if (HAL_TIM_Encoder_Start(&htim, TIM_CHANNEL_ALL) != HAL_OK){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	}

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


//	SYSCFG->EXTICR1  &= (0x000F) ;
	//1. clear bits 3:0 in the SYSCFG_EXTICR1 reg to amp EXTI Line to NVIC

	//EXTI->RTSR = EXTI_RTSR_TR0;
	// 2.Set interrupt trigger to rising edge

	//EXTI->IMR = EXTI_IMR_MR0; // 3. unmask EXTI0 line
	// NVIC_SetPriority(EXTI2_3_IRQn, 1); //4. Set Priority to 1
	// NVIC_EnableIRQ(EXTI2_3_IRQn);  // 5. Enable EXTI0_1 interrupt in NVIC (do 4 first)

	// NVIC_SetPriority(EXTI4_15_IRQn, 1); //4. Set Priority to 1
	// NVIC_EnableIRQ(EXTI4_15_IRQn);  // 5. Enable EXTI0_1 interrupt in NVIC (do 4 first)

	encoderPos = 0;

//Pin C6 is red, C7 is blue
	while(1) {
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9); //Toggle the state of pin PC9
		//HAL_Delay(100); //delay 100ms

		encoderPos = TIM2->CNT;
		if(encoderPos > 50)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
		}
		else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
		}
		// if (encoderPos > 100){
		// 	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
		// 	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
		// }
		// else{
		// 	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
		// 	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);

		// }
	}
}
