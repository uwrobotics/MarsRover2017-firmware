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
#include "pwmiclib.h"
#include "pins.h"


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
    if (PWMICLIB_Init(pwm_id) != 0)
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
            .Pin        = LED1_GPIO_PIN | LED2_GPIO_PIN | LED3_GPIO_PIN | LED4_GPIO_PIN,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}


int main(void)
{
    //Always call. Enables prefetch and calls above function
    HAL_Init();
    CLK_Init();

    if (PWMICLIB_Init(ENCODER1) != 0)
    {
        Error_Handler();
    }

    HAL_Delay(1000);

    while (1)
    {
        uint32_t count = PWMICLIB_Read(ENCODER1);

        if (count > 4096)
        {
            count = 0;
        }

        if (count < 1024)
        {
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_RESET);
        }
        else if (count < 2048)
        {
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_RESET);
        }
        else if (count < 3072)
        {
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_RESET);
        }
        else if (count < 4096)
        {
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_GPIO_PIN, GPIO_PIN_SET);
        }
    }

    return 0;
}
