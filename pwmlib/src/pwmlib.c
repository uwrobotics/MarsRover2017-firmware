/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     pwmlib.c
@author:  Archie Lee

**********************************************************/

#include "pwmlib.h"

TIM_HandleTypeDef PwmAStruct;
TIM_HandleTypeDef PwmCStruct;
TIM_OC_InitTypeDef sConfig;

void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        htim->Instance->CCR1 = pwm_value[0];
        htim->Instance->CCR2 = pwm_value[1];
        htim->Instance->CCR3 = pwm_value[2];
    }
    else if (htim->Instance == TIM3)
    {
        htim->Instance->CCR4 = pwm_value[3];
    }
}

void TIM1_CC_IRQn(void)
{
    HAL_TIM_IRQHandler(&PwmAStruct);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&PwmCStruct);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    // PWM pins on GPIOA
    GPIO_InitStruct.Pin         = PWM1_GPIO_PIN | PWM2_GPIO_PIN | PWM3_GPIO_PIN;
    GPIO_InitStruct.Mode        = PWM_GPIO_MODE;
    GPIO_InitStruct.Pull        = PWM_GPIO_PULL;
    GPIO_InitStruct.Speed       = PWM_GPIO_SPEED;
    GPIO_InitStruct.Alternate   = PWM1_GPIO_ALTERNATE;

    HAL_GPIO_Init(PWM1_GPIO_PORT, &GPIO_InitStruct);

    // PWM pins on GPIOC
    GPIO_InitStruct.Pin         = PWM4_GPIO_PIN;
    GPIO_InitStruct.Mode        = PWM_GPIO_MODE;
    GPIO_InitStruct.Pull        = PWM_GPIO_PULL;
    GPIO_InitStruct.Speed       = PWM_GPIO_SPEED;
    GPIO_InitStruct.Alternate   = PWM4_GPIO_ALTERNATE;

    HAL_GPIO_Init(PWM4_GPIO_PORT, &GPIO_InitStruct);

    // Make sure this is lower value (priority) than system timer and CAN
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);

    HAL_NVIC_SetPriority(TIM3_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

static int PWMLIB_ChannelInit(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t pwm_id)
{
    uint32_t channel;

    switch (pwm_id)
    {
        case 1:
            channel         = TIM_CHANNEL_1;
            break;

        case 2:
            channel         = TIM_CHANNEL_2;
            break;

        case 3:
            channel         = TIM_CHANNEL_3;
            break;

        case 4:
            channel         = TIM_CHANNEL_4;
            break;

        default:
            return -1;
    }

    sConfig->OCMode         = PWM_OC_MODE;
    sConfig->OCPolarity     = PWM_OC_POLARITY;
    sConfig->OCFastMode     = PWM_OC_FAST_MODE;
    sConfig->OCIdleState    = PWM_OC_IDLE_STATE;
    sConfig->Pulse          = PWM_OC_DUTY_CYCLE;

    if (HAL_TIM_PWM_ConfigChannel(htim, sConfig, channel) != HAL_OK)
    {
        return -2;
    }

    return 0;
}

static int PWMLIB_TimerInit(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t pwm_id)
{
    switch (pwm_id)
    {
        case 1:
        case 2:
        case 3:
            htim.Instance       = TIM1;
            break;

        case 4:
            htim.Instance       = TIM3;
            break;

        default:
            return -1;
    }

    htim->Init.Prescaler         = PWM_PRESCALER;
    htim->Init.Period            = PWM_PERIOD;
    htim->Init.ClockDivision     = PWM_CLOCK_DIVISION;
    htim->Init.CounterMode       = PWM_COUNTER_MODE;

    if (HAL_TIM_PWM_Init(&htim) != HAL_OK)
    {
        return -2;
    }

    if (PWMLIB_ChannelInit(htim, sConfig, pwm_id) != 0)
    {
        return -3;
    }

    if (HAL_TIM_PWM_Start_IT(htim, channel) != HAL_OK)
    {
        return -4;
    }

    return 0;
}

int PWMLIB_Init(uint32_t pwm_id)
{
    int error = 0;

    switch (pwm_id)
    {
        // TIM1
        case 1:
            if (PWMLIB_TimerInit(&PwmAStruct, &sConfig, 1) != 0)
            {
                error = -1;
            }
            break;

        case 2:
            if (PWMLIB_TimerInit(&PwmAStruct, &sConfig, 2) != 0)
            {
                error = -2;
            }
            break;

        case 3:
            if (PWMLIB_TimerInit(&PwmAStruct, &sConfig, 3) != 0)
            {
                error = -3;
            }
            break;

        // TIM3
        case 4:
            if (PWMLIB_TimerInit(&PwmCStruct, &sConfig, 4) != 0)
            {
                error = -4;
            }
            break;

        // Invalid PWM number
        default:
            error = -5;
            break;
    }

    return error;
}
