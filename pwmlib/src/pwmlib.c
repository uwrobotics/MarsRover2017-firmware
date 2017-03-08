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
}

static int PWMLIB_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t pwm_id, uint32_t pulse_width)
{
    uint32_t channel;
    uint32_t error = 0;

    switch (pwm_id)
    {
        case 1:
            channel = TIM_CHANNEL_1;
            break;

        case 2:
            channel = TIM_CHANNEL_2;
            break;

        case 3:
            channel = TIM_CHANNEL_3;
            break;

        case 4:
            channel = TIM_CHANNEL_4;
            break;

        default:
            error = 1;
            break;
    }

    if (error)
    {
        return -1;
    }

    sConfig->OCMode         = PWM_OC_MODE;
    sConfig->OCPolarity     = PWM_OC_POLARITY;
    sConfig->OCNPolarity    = PWM_OC_NPOLARITY;
    sConfig->OCFastMode     = PWM_OC_FAST_MODE;
    sConfig->OCIdleState    = PWM_OC_IDLE_STATE;
    sConfig->OCNIdleState   = PWM_OC_NIDLE_STATE;
    sConfig->Pulse          = pulse_width;

    if (HAL_TIM_PWM_ConfigChannel(htim, sConfig, channel) != HAL_OK)
    {
        return -2;
    }

    if (HAL_TIM_PWM_Start(htim, channel) != HAL_OK)
    {
        return -3;
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
            htim->Instance       = TIM1;
            break;

        case 4:
            htim->Instance       = TIM3;
            break;

        default:
            return -1;
    }

    __HAL_TIM_DISABLE(htim);

    SystemCoreClockUpdate();

    htim->Init.Prescaler         = (uint16_t)(SystemCoreClock / 1000000) - 1;
    htim->Init.Period            = PWM_PERIOD;
    htim->Init.ClockDivision     = PWM_CLOCK_DIVISION;
    htim->Init.CounterMode       = PWM_COUNTER_MODE;

    if (HAL_TIM_PWM_Init(htim) != HAL_OK)
    {
        return -2;
    }

    if (PWMLIB_ConfigChannel(htim, sConfig, pwm_id, 0) != 0)
    {
        return -3;
    }

    __HAL_TIM_ENABLE(htim);

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

int PWMLIB_Write(uint32_t pwm_id, float duty_cycle)
{
    TIM_HandleTypeDef *htim;
    uint32_t pulse_width;

    switch (pwm_id)
    {
        case 1:
        case 2:
        case 3:
            htim = &PwmAStruct;

        case 4:
            htim = &PwmCStruct;
    }

    if (duty_cycle >= 1.0)
    {
        pulse_width = PWM_PERIOD;
    }
    else if (duty_cycle <= 0.0)
    {
        pulse_width = 0;
    }
    else
    {
        pulse_width = duty_cycle * PWM_PERIOD;
    }

    if (PWMLIB_ConfigChannel(htim, &sConfig, pwm_id, pulse_width) != 0)
    {
        return -1;
    }

    return 0;
}
