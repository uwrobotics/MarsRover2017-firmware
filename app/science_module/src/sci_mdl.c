/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     sci_mdl.c
@author:  Rahul Rawat

Description:
TODO

**********************************************************/

#include "stm32f0xx.h"
#include "canlib.h"
#include "pwmlib.h"
//#include "encoderlib.h"
#include "pins.h"
#include <math.h>
#include <string.h>

//Code assumes that one board is in charge of motors controlling inclination and azimuth of a joint
//Example of azimuth vs inclination: http://edndoc.esri.com/arcobjects/9.1/java/arcengine/com/esri/arcgis/geometry/bitmaps/GeomVector3D.gif

//PWM IDs
#define PWM_AZIMUTH_ID           1
#define PWM_INCLINATION_ID       2

//CAN IDs that this will receive messages from
#define CAN_RX_ID                5   //Arbitrary value

//CAN IDs that this will code will transmit on
#define CAN_TX_ID                15  //Arbitrary value
#define CAN_ENCODER_DATA_ID      16  //Arbitrary value
#define CAN_LIMIT_SW_READ_ID     17  //Arbitrary value


#define LIMIT_SWITCH_COUNT       2

//Number of PWM commands per relevant CAN frame received
#define NUM_CMDS                 2
//Timer interrupt interval
#define PERIOD                   500
//Number of timer intervals of no message received to enter watchdog state
#define MSG_WATCHDOG_INTERVAL    1

//Index in received CAN frame for float for each axis motors
//The first 4 bytes contain azimuth motor PWM command and the last 4 contain inclination motor PWM command
#define AZIMUTH_AXIS_ID          0
#define INCLINATION_AXIS_ID      1

// Limit Switches
#define LIMIT_SWITCH_1_PIN  GPIO_PIN_4
#define LIMIT_SWITCH_1_PORT GPIOC
#define LIMIT_SWITCH_2_PIN  GPIO_PIN_3
#define LIMIT_SWITCH_2_PORT GPIOC

// Limit Switches
#define LIMIT_SWITCH_1_PIN  GPIO_PIN_4
#define LIMIT_SWITCH_1_PORT GPIOC
#define LIMIT_SWITCH_2_PIN  GPIO_PIN_3
#define LIMIT_SWITCH_2_PORT GPIOC

const float epsilon = 0.0001;
float incoming_cmd[NUM_CMDS] = { 0 }; //Array to hold incoming CAN messages
float joy_cmd[NUM_CMDS] = { 0 }; //Can we just reuse incoming_cmd?

//Flags
volatile uint8_t data_ready = 0;
volatile uint8_t msg_received = 0;

//contains abs value of the PWM command for each motor
volatile float azimuth_motor_duty_cycle = 0;
volatile float inclination_motor_duty_cycle = 0;

//Bitfield for limit switch readings
uint8_t limit_switch_readings = 0;

//Direction of each motor. 0 is backwards and 1 is forwards. 
//Maybe this can be a bitfield instead
uint8_t azimuth_direction = 0;
uint8_t inclination_direction = 0;

static TIM_HandleTypeDef s_TimerInstance =
{
    .Instance = TIM14
};

static void Error_Handler(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

    while (1)
    {
        //can anything useful be done here?
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

    // Direction control pins
    GPIO_InitTypeDef DirCtrl_InitStruct = {
            .Pin        = GPIO_PIN_5 | GPIO_PIN_2, //Pin 5 for azimuth, pin 2 for inclination. Change as needed  
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &DirCtrl_InitStruct);

    //Limit switch init. Assumes 2 limit switches
    GPIO_InitTypeDef LimitSwitch_InitStruct = {
        .Pin            = LIMIT_SWITCH_1_PIN | LIMIT_SWITCH_2_PIN,
        .Mode           = GPIO_MODE_INPUT,
        .Pull           = GPIO_NOPULL,
        .Speed          = GPIO_SPEED_FREQ_HIGH
    };

    HAL_GPIO_Init(GPIOC, &LimitSwitch_InitStruct);

}

void HAL_MspInit(void)
{
    /* System interrupt init*/
    /* SysTick_IRQn interrupt configuration */
    (void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    //Must be set to priority of 1 or else will have higher priority than CAN IRQ
    HAL_NVIC_SetPriority(SysTick_IRQn, 2, 2);
}

int main(void)
{
    HAL_Init();
    CLK_Init();
    GPIO_Init();
    Timer_Init(PERIOD); // 500 ms timer

    return 0;
}
