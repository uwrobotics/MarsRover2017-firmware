/*********************************************************
     __  ___                   ____
    /  |/  /___ ___________   / __ \____ _   _____  _____
   / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
  / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
 /_/  /_/\__,_/_/  /____/  /_/ |_|\____/|___/\___/_/

 Copyright 2017, UW Robotics Team

 @file     encoderlib.c
 @author:  Jerry Li

**********************************************************/
#include "pins.h"
#include "encoderlib.h"

TIM_HandleTypeDef encoder1;
TIM_HandleTypeDef encoder2;
TIM_Encoder_InitTypeDef sConfig;
uint16_t lastCount1;
uint16_t lastCount2;

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim) {
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	//encoder 1a
	GPIO_InitStruct.Pin = ENCODER_1A_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(ENCODER_1A_PORT, &GPIO_InitStruct);

	//encoder 1b
	GPIO_InitStruct.Pin = ENCODER_1B_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(ENCODER_1B_PORT, &GPIO_InitStruct);

	//encoder 2a
	GPIO_InitStruct.Pin = ENCODER_2A_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF0_TIM3;
	HAL_GPIO_Init(ENCODER_2A_PORT, &GPIO_InitStruct);
	
	//encoder 2b
	GPIO_InitStruct.Pin = ENCODER_2B_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF0_TIM3;
	HAL_GPIO_Init(ENCODER_2B_PORT, &GPIO_InitStruct);
}


//return code of -1 means error;
int EncoderLib_Init(uint32_t encoder) {
    TIM_HandleTypeDef *htim;
    
    switch(encoder) {
        case 1:
            htim = &encoder1;
            htim->Instance = TIM2;
            break;
        case 2:
            htim = &encoder2;
            htim->Instance = TIM3;
            break;
        default:
            return -1;
    }

    htim->Init.Prescaler = 0;
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = 0xFFFF; //encoder will count up to this before overflowing
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;

    if( HAL_TIM_Encoder_Init(htim, &sConfig) != HAL_OK){
		//error
        return -1;
	}

	if (HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL) != HAL_OK){
		//error
        return -1;
	}
}

uint32_t EncoderLib_ReadCount(uint32_t encoder){
    TIM_HandleTypeDef *htim;
    uint16_t lastCount;
    switch(encoder)
    {
        case 1:
            htim = &encoder1;
            lastCount = lastCount1;
            break;
        case 2:
            htim = &encoder2;
            lastCount = lastCount2;
            break;
        default:
            return 0;
    }

    //overflow should never happen, motors don't move that much
    //underflow, the 0x8FFF is around halfway, should never be that much time between reads of the encoder
    if(lastCount >= 0 && htim->Instance->CNT > 0x8FFF && __HAL_TIM_IS_TIM_COUNTING_DOWN(htim)){
        htim->Instance->CNT = 0;
    }

    lastCount = htim->Instance->CNT;
    return htim->Instance->CNT;
    
}