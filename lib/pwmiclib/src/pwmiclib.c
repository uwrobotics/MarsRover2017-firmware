/*********************************************************
     __  ___                   ____
    /  |/  /___ ___________   / __ \____ _   _____  _____
   / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
  / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
 /_/  /_/\__,_/_/  /____/  /_/ |_|\____/|___/\___/_/

 Copyright 2017, UW Robotics Team

 @file     pwmiclib.c
 @author:  Archie Lee

**********************************************************/
#include "pins.h"
#include "pwmiclib.h"


TIM_HandleTypeDef encoder1;
TIM_HandleTypeDef encoder2;
TIM_HandleTypeDef encoder3;
TIM_HandleTypeDef encoder4;
TIM_IC_InitTypeDef sConfig;


void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    //encoder 1a
    GPIO_InitStruct.Pin = ENCODER_PWM1_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init(ENCODER_PWM1_PORT, &GPIO_InitStruct);

    //encoder 1b
    GPIO_InitStruct.Pin = ENCODER_PWM2_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init(ENCODER_PWM2_PORT, &GPIO_InitStruct);

    //encoder 2a
    GPIO_InitStruct.Pin = ENCODER_PWM3_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF0_TIM3;
    HAL_GPIO_Init(ENCODER_PWM3_PORT, &GPIO_InitStruct);

    //encoder 2b
    GPIO_InitStruct.Pin = ENCODER_PWM4_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF0_TIM3;
    HAL_GPIO_Init(ENCODER_PWM4_PORT, &GPIO_InitStruct);
}

static int PWMICLIB_ConfigChannel(TIM_HandleTypeDef *htim, TIM_IC_InitTypeDef* sConfig, uint32_t encoder, uint32_t channel)
{
    sConfig->ICPolarity     = TIM_INPUTCHANNELPOLARITY_RISING;  // capture rising edge
    sConfig->ICSelection    = TIM_ICSELECTION_DIRECTTI;         // direct connection
    sConfig->ICPrescaler    = TIM_ICPSC_DIV1;                   // capture all events
    sConfig->ICFilter       = 3;                                // filter of N events (debouncing)

    if (HAL_TIM_IC_ConfigChannel(htim, sConfig, channel) != HAL_OK)
    {
        return -2;
    }

    return 0;
}

//return code of -1 means error;
int PWMICLIB_Init(uint32_t encoder)
{
    TIM_HandleTypeDef *htim;
    uint32_t channel;

    switch (encoder)
    {
        case 1:
            htim = &encoder1;
            htim->Instance = TIM2;
            channel = TIM_CHANNEL_1;
            break;

        case 2:
            htim = &encoder2;
            htim->Instance = TIM2;
            channel = TIM_CHANNEL_2;
            break;

        case 3:
            htim = &encoder3;
            htim->Instance = TIM3;
            channel = TIM_CHANNEL_1;
            break;

        case 4:
            htim = &encoder4;
            htim->Instance = TIM3;
            channel = TIM_CHANNEL_2;
            break;

        default:
            return -1;
    }

    htim->Init.Prescaler        = ENCODER_PRESCALER;        // for 48 MHz clock, 1 tick = 1 us
    htim->Init.CounterMode      = TIM_COUNTERMODE_UP;       // count up
    htim->Init.Period           = ENCODER_PERIOD;           // from datasheet
    htim->Init.ClockDivision    = TIM_CLOCKDIVISION_DIV1;   // default clock division

    if (HAL_TIM_IC_Init(htim) != HAL_OK)
    {
        //error
        return -2;
    }

    if (PWMICLIB_ConfigChannel(htim, &sConfig, encoder, channel) != 0)
    {
        //error
        return -3;
    }

    if (HAL_TIM_IC_Start(htim, channel) != HAL_OK)
    {
        return -4;
    }

    return 0;
}

uint32_t PWMICLIB_Read(uint32_t encoder)
{
    TIM_HandleTypeDef *htim;
    // uint16_t lastCount;

    switch (encoder)
    {
        case 1:
            htim = &encoder1;
            break;

        case 2:
            htim = &encoder2;
            break;

        case 3:
            htim = &encoder3;
            break;

        case 4:
            htim = &encoder4;
            break;

        default:
            return 0;
    }

    // //overflow should never happen, motors don't move that much
    // //underflow, the 0x8FFF is around halfway, should never be that much time between reads of the encoder
    // if(lastCount >= 0 && htim->Instance->CNT > 0x8FFF && __HAL_TIM_IS_TIM_COUNTING_DOWN(htim)){
    //     htim->Instance->CNT = 0;
    // }

    return htim->Instance->CCR1;

}
