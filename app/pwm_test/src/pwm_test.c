/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     pwm_test.c
@author:  Archie Lee

**********************************************************/
#include "stm32f0xx.h"
#include "pwmlib.h"
#include "pins.h"

volatile uint32_t pwm_value[MAX_PWM_CHANNELS] = {0};


// Initialize system clock --> must do for ALL programs!!!
void CLK_Init(void)
{
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
    __HAL_RCC_SYSCFG_CLK_ENABLE();
}

int PWM_Init(uint32_t pwm_id)
{
    // should also initialize LED on pin PC9 if pwm_id 4 is passed in
    if (PWMLIB_Init(pwm_id) != 0)
    {
        // error;
        return -1;
    }

    return 0;
}

// don't initialize PC9 because pwmlib will use that pin
void LED_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {
            .Pin        = GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int test1(uint32_t pwm_id)
{
    uint32_t i;
    float duty_cycle;

    if (pwm_id <= MAX_PWM_CHANNELS)
    {
        for (i = 100; i >= 0; i--)
        {
            duty_cycle = (float)i / 100.0;
            if (PWMLIB_Write(pwm_id, duty_cycle) != 0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
                HAL_Delay(2000);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            HAL_Delay(25);
        }
        HAL_Delay(500);
        PWMLIB_Write(pwm_id, 0);
        HAL_Delay(500);

        return 0;
    }
    return -1;
}

int main(void)
{
    uint32_t pwm_id = 1;

    //Always call. Enables prefetch and calls above function
    HAL_Init();

    CLK_Init();
    LED_Init();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_Delay(2000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

    if (PWM_Init(pwm_id) != 0)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_Delay(2000);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
    }

    while (1)
    {
        // If a test returns -1, the HAL has returned HAL_ERROR
        if (test1(pwm_id) == -1)
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
            HAL_Delay(2000);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        }
    }

    return 0;
}
