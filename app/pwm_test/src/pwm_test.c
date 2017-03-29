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

#define PWM_ID      3

static void Error_Handler(void)
{
  while (1)
  {
  }
}

void CLK_Init(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Select HSI48 Oscillator as PLL source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
    {
        Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
    {
        Error_Handler();
    }
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
    int i;
    float duty_cycle;

    if (pwm_id <= MAX_PWM_CHANNELS)
    {
        for (i = 0; i <= 100; i++)
        {
            duty_cycle = (float)i / 100.0;
            if (PWMLIB_Write(pwm_id, duty_cycle) != 0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
                HAL_Delay(1000);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            HAL_Delay(10);
        }
        for (i = 100; i >= 0; i--)
        {
            duty_cycle = (float)i / 100.0;
            if (PWMLIB_Write(pwm_id, duty_cycle) != 0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
                HAL_Delay(1000);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            HAL_Delay(10);
        }

        HAL_Delay(500);

        return 0;
    }

    return -1;
}

int test2(uint32_t pwm_id)
{
    int i;
    float duty_cycle;

    if (pwm_id <= MAX_PWM_CHANNELS)
    {
        if (PWMLIB_ChangePeriod(pwm_id, 500) != 0)
        {
            return -1;
        }

        for (i = 0; i <= 100; i++)
        {
            duty_cycle = (float)i / 100.0;
            if (PWMLIB_Write(pwm_id, duty_cycle) != 0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
                HAL_Delay(1000);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            HAL_Delay(10);
        }
        for (i = 100; i >= 0; i--)
        {
            duty_cycle = (float)i / 100.0;
            if (PWMLIB_Write(pwm_id, duty_cycle) != 0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
                HAL_Delay(1000);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            HAL_Delay(10);
        }

        HAL_Delay(500);

        return 0;
    }
    return -1;
}

int MotorTest(uint32_t pwm_id)
{
    int i;
    float duty_cycle;

    if (pwm_id <= MAX_PWM_CHANNELS)
    {
        for (i = 0; i <= 5; i++)
        {
            // only set to maximum 50% power because motor is 12V, supply is 24V
            duty_cycle = (float)i * 0.1;
            if (PWMLIB_Write(pwm_id, duty_cycle) != 0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
                HAL_Delay(1000);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            HAL_Delay(1000);
        }
        for (i = 5; i >= 0; i--)
        {
            // only set to maximum 50% power because motor is 12V, supply is 24V
            duty_cycle = (float)i * 0.1;
            if (PWMLIB_Write(pwm_id, duty_cycle) != 0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
                HAL_Delay(1000);
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            HAL_Delay(1000);
        }
    }
    return -1;
}

int main(void)
{
    //Always call. Enables prefetch and calls above function
    HAL_Init();

    CLK_Init();

    HAL_Delay(2000);

    LED_Init();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_Delay(1000);

    if (PWM_Init(PWM_ID) != 0)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_Delay(1000);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_Delay(1000);
    }

    HAL_Delay(1000);

    while (1)
    {
        // If a test returns -1, the HAL has returned HAL_ERROR
        if (MotorTest(PWM_ID) == -1)
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
            HAL_Delay(1000);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
            HAL_Delay(1000);
        }
        HAL_Delay(10000);
    }

    return 0;
}
