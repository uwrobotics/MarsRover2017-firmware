/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     arm_control.c
@author:  Archie Lee

Description:
Basic program for open-loop control of the arm motors.
Commands are received over CAN.

**********************************************************/

#include "stm32f0xx.h"
#include "canlib.h"
#include "pwmlib.h"
#include "pins.h"
#include <math.h>
#include <string.h>

#define PWM_ID          1
#define CAN_RX_ID       5
#define CAN_TX_ID       15

#define NUM_CMDS        3
#define PERIOD          500

const float epsilon = 0.0001;
float incoming_cmd[NUM_CMDS] = { 0 };
float joy_cmd[NUM_CMDS] = { 0 };
volatile uint8_t axis_idx = 0;
volatile uint8_t data_ready = 0;
volatile uint8_t msg_received = 0;

static TIM_HandleTypeDef s_TimerInstance =
{
    .Instance = TIM14
};

static void Error_Handler(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

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

void TIM14_IRQHandler()
{
    HAL_TIM_IRQHandler(&s_TimerInstance);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (msg_received)
    {
        msg_received = 0;
    }
    else
    {
        PWMLIB_Write(PWM_ID, 0);
    }
}

void Timer_Init(uint32_t period)
{
    __HAL_RCC_TIM14_CLK_ENABLE();
    s_TimerInstance.Init.Prescaler = 47999; // 1 tick = 1 ms
    s_TimerInstance.Init.CounterMode = TIM_COUNTERMODE_UP;
    s_TimerInstance.Init.Period = period;
    s_TimerInstance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    s_TimerInstance.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&s_TimerInstance);
    HAL_TIM_Base_Start_IT(&s_TimerInstance);
}

void GPIO_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // LEDs
    GPIO_InitTypeDef LED_InitStruct = {
            .Pin        = GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &LED_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

    // Direction control pin
    GPIO_InitTypeDef DirCtrl_InitStruct = {
            .Pin        = GPIO_PIN_5,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &DirCtrl_InitStruct);
}

void HAL_MspInit(void)
{
    /* System interrupt init*/
    /* SysTick_IRQn interrupt configuration */
    (void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    //Must be set to priority of 1 or else will have higher priority than CAN IRQ
    HAL_NVIC_SetPriority(SysTick_IRQn, 2, 2);
//    HAL_NVIC_SetPriority(TIM14_IRQn, 1, 1);
//    HAL_NVIC_EnableIRQ(TIM14_IRQn);
}

void CAN_Init(uint32_t id)
{
    switch(CANLIB_Init(id, 0))
    {
        case 0:
            //Initialization of CAN handler successful
            break;
        case -1:
            //Initialization of CAN handler not successful
            break;
        default:
            break;
    }

    CANLIB_AddFilter(CAN_RX_ID);
}

int main(void)
{
    HAL_Init();
    CLK_Init();
    GPIO_Init();
    Timer_Init(PERIOD); // 500 ms timer

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

    if (CANLIB_Init(CAN_TX_ID, 0) != 0)
    {
        Error_Handler();
    }
    if (CANLIB_AddFilter(CAN_RX_ID) != 0)
    {
        Error_Handler();
    }
    if (PWMLIB_Init(PWM_ID) != 0)
    {
        Error_Handler();
    }

    while(1)
    {
        while (!data_ready);
        data_ready = 0;

        memcpy(joy_cmd, incoming_cmd, NUM_CMDS * sizeof(*incoming_cmd));

        if (joy_cmd[0] > (0.0 - epsilon) && joy_cmd[0] < (0.0 + epsilon))
        {
            PWMLIB_Write(PWM_ID, 0);
        }
        else if (fabs(joy_cmd[0]) <= 0.5) 
        {
            // Forward
            if (joy_cmd[0] > 0.0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
//                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
            }
            // Reverse
            else if (joy_cmd[0] < 0.0)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
//                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
            }
            PWMLIB_Write(PWM_ID, fabs(joy_cmd[0]));
        }
    }

    return 0;
}

void CANLIB_Rx_OnMessageReceived(void)
{
    static uint32_t prev_idx = NUM_CMDS - 1;

    switch(CANLIB_Rx_GetSenderID())
    {
        // Expect frame format to be:
        // Bytes 0-3: Command
        // Byte 4: Joystick axis index (i.e. axis 0, axis 1, axis 2)
        case 5:
            msg_received = 1;
            axis_idx = CANLIB_Rx_GetSingleByte(4);
            if ((axis_idx == (prev_idx + 1) % NUM_CMDS) && !data_ready)
            {
                incoming_cmd[axis_idx] = CANLIB_Rx_GetAsFloat(0);
                prev_idx = axis_idx;

                if (axis_idx == (NUM_CMDS -1))
                {
                    data_ready = 1;
                }
            }
            break;

        default:
            break;
    }
}
