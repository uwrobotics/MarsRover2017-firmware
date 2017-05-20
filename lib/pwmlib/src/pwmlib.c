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

// Each timer has its own handler. Therefore, every PWM channel on the same timer will have the same PWM period.
TIM_HandleTypeDef TIM1_Handler;
TIM_OC_InitTypeDef sConfig;

// Variable that stores the pulse width written to each channel
volatile uint16_t pulse_width = 0;


//////////////////////////////////////////
//      Internal Library Functions      //
//////////////////////////////////////////

// Initializes the PWM GPIO pins and clocks. Automatically called by HAL on initialization.
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    __HAL_RCC_TIM1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    // General GPIO settings (same for all pins)
    GPIO_InitStruct.Mode        = PWM_GPIO_MODE;
    GPIO_InitStruct.Pull        = PWM_GPIO_PULL;
    GPIO_InitStruct.Speed       = PWM_GPIO_SPEED;


    // PWM pins on GPIOA
    GPIO_InitStruct.Pin         = PWM1_GPIO_PIN | PWM2_GPIO_PIN | PWM3_GPIO_PIN;
    GPIO_InitStruct.Alternate   = PWM1_GPIO_ALTERNATE;

    HAL_GPIO_Init(PWM1_GPIO_PORT, &GPIO_InitStruct);

}

// Configures each individual PWM channel. Should not be called outside the library.
static int PWMLIB_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t pwm_id)
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
    sConfig->Pulse          = pulse_width & 0xFFFF;

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

// Initializes each timer. Should not be called outside of the library.
static int PWMLIB_TimerInit(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t pwm_id)
{
    switch (pwm_id)
    {
        case 1:
        case 2:
        case 3:
            htim->Instance       = TIM1;
            break;

        default:
            return -1;
    }

    __HAL_TIM_DISABLE(htim);

    htim->Init.Prescaler         = PWM_PRESCALER;
    htim->Init.Period            = PWM_PERIOD;
    htim->Init.ClockDivision     = PWM_CLOCK_DIVISION;
    htim->Init.CounterMode       = PWM_COUNTER_MODE;
    htim->Init.RepetitionCounter = 0;

    if (HAL_TIM_PWM_Init(htim) != HAL_OK)
    {
        return -2;
    }

    pulse_width = 0;

    if (PWMLIB_ConfigChannel(htim, sConfig, pwm_id) != 0)
    {
        return -3;
    }

    __HAL_TIM_ENABLE(htim);

    return 0;
}


////////////////////////////////////////
//      Public Library Functions      //
////////////////////////////////////////

/*******************************************************************************************************
    PWMLIB_Write
    ------------

    Description:
    ============
    Main function you care about. Writes a new duty cycle to the chosen channel. Duty cycle percentage
    should be between 0 and 1.

    Arguments:
    ==========
    uint32_t        pwm_id              ID of the PWM channel to change the period for.
    float           duty_cycle          New duty cycle for channel (between 0 and 1)

    Returns:
    ========
    0               On success
    -1              Invalid pwm_id
    -2              Configuring the PWM channel failed
*******************************************************************************************************/
int PWMLIB_Init(uint32_t pwm_id)
{
    int error = 0;
    TIM_HandleTypeDef *htim = NULL;

    switch (pwm_id)
    {
        // TIM1
        case 1:
        case 2:
        case 3:
            htim = &TIM1_Handler;
            break;

        // Invalid PWM number
        default:
            error = -1;
            break;
    }

    if (htim)
    {
        PWMLIB_TimerInit(htim, &sConfig, pwm_id);
    }

    return error;
}

/*******************************************************************************************************
    PWMLIB_Write
    ------------

    Description:
    ============
    Main function you care about. Writes a new duty cycle to the chosen channel. Duty cycle percentage
    should be between 0 and 1.

    Arguments:
    ==========
    uint32_t        pwm_id              ID of the PWM channel to change the period for.
    float           duty_cycle          New duty cycle for channel (between 0 and 1)

    Returns:
    ========
    0               On success
    -1              Invalid pwm_id
    -2              Configuring the PWM channel failed
*******************************************************************************************************/
int PWMLIB_Write(uint32_t pwm_id, float duty_cycle)
{
    TIM_HandleTypeDef *htim;
    uint8_t error = 0;

    switch (pwm_id)
    {
        case 1:
        case 2:
        case 3:
            htim = &TIM1_Handler;
            break;

        default:
            error = 1;
            break;
    }

    if (error)
    {
        return -1;
    }

    if (duty_cycle >= 1.0)
    {
        pulse_width = htim->Init.Period;
    }
    else if (duty_cycle <= 0.0)
    {
        pulse_width = 0;
    }
    else
    {
        pulse_width = (uint16_t) (duty_cycle * htim->Init.Period);
    }

    if (PWMLIB_ConfigChannel(htim, &sConfig, pwm_id) != 0)
    {
        return -2;
    }

    return 0;
}

/*******************************************************************************************************
    PWMLIB_ChangePeriod
    -------------------

    Description:
    ============
    Changes period for the PWM channel associated with the pwm_id. Usually will never need to be called.
    All channels using the same timer will have their periods changed. Don't try to set different
    periods if they use the same timer. Users should never need to call this function. Only use if you
    know what you're doing.

    Arguments:
    ==========
    uint32_t        pwm_id              ID of the PWM channel to change the period for.
    uint32_t        period              New period of the PWM channel (less than 65535)

    Returns:
    ========
    0               On success
    -1              Invalid period
    -2              Invalid pwm_id
    -3              HAL failed to initialize PWM
    -4              Configuring the PWM channel failed
*******************************************************************************************************/
int PWMLIB_ChangePeriod(uint32_t pwm_id, uint32_t period)
{
    TIM_HandleTypeDef *htim;
    uint8_t error = 0;

    if (period > 0xFFFF || period == 0)
    {
        return -1;
    }

    switch (pwm_id)
    {
        case 1:
        case 2:
        case 3:
            htim = &TIM1_Handler;
            break;

        default:
            error = 1;
            break;
    }

    if (error)
    {
        return -2;
    }

    __HAL_TIM_DISABLE(htim);

    htim->Init.Prescaler         = PWM_PRESCALER;
    htim->Init.Period            = period;
    htim->Init.ClockDivision     = PWM_CLOCK_DIVISION;
    htim->Init.CounterMode       = PWM_COUNTER_MODE;
    htim->Init.RepetitionCounter = 0;

    if (HAL_TIM_PWM_Init(htim) != HAL_OK)
    {
        return -3;
    }

    pulse_width = 0;

    if (PWMLIB_ConfigChannel(htim, &sConfig, pwm_id) != 0)
    {
        return -4;
    }

    __HAL_TIM_ENABLE(htim);

    return 0;
}
