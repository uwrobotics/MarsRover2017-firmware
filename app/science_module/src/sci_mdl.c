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


//PWM IDs
#define PWM_DRILL_ID            1
#define PWM_ELEVATOR_ID         2
#define PWM_SAMPLE_ID           3

//CAN IDs that this will receive messages from
#define CAN_DRILL_ELEVATOR_ID   700
#define CAN_SAMPLE_ID           701
#define CAN_LIMIT_SWITCH_ID     702
#define CAN_UV_ID               703
#define CAN_GAS_ID              704
#define CAN_TMP_1_ID            705
#define CAN_HUM_1_ID            706
#define CAN_TMP_2_ID            707
#define CAN_HUM_2_ID            708
#define CAN_TMP_3_ID            709
#define CAN_HUM_3_ID            710
#define CAN_TRIGGER_ID          711
#define CAN_STOP_ID             712

//CAN IDs that this will code will transmit on
#define CAN_TX_ID                15  //Arbitrary value
#define CAN_ENCODER_DATA_ID      16  //Arbitrary value
#define CAN_LIMIT_SW_READ_ID     17  //Arbitrary value

#define LIMIT_SWITCH_COUNT       2

//Timer interrupt interval
#define PERIOD                   1000

// Limit Switches TODO: REMOVE
#define LIMIT_SWITCH_1_PIN  GPIO_PIN_4
#define LIMIT_SWITCH_1_PORT GPIOC
#define LIMIT_SWITCH_2_PIN  GPIO_PIN_3
#define LIMIT_SWITCH_2_PORT GPIOC

// Limit Switches for Application TODO: CHECK PINS
#define LIMIT_SWITCH_FILTER_OUTER_PIN GPIO_PIN_5
#define LIMIT_SWITCH_FILTER_OUTER_PORT GPIOC
#define LIMIT_SWITCH_FILTER_INNER_PIN  GPIO_PIN_6
#define LIMIT_SWITCH_FILTER_INNER_PORT GPIOC

#define GPIO_DRILL_PIN GPIO_PIN_7
#define GPIO_DRILL_PORT GPIOC
#define GPIO_ELEVATOR_PIN GPIO_PIN_8
#define GPIO_ELEVATOR_PORT GPIOC
#define GPIO_FILTER_PIN GPIO_PIN_9
#define GPIO_FITLER_PORT GPIOC

// Duty Cycle Values TODO: ASK FOR RIGHT VALUES
#define ELEVATOR_DUTY_CYCLE_UP 0.6
#define ELEVATOR_DUTY_CYCLE_DOWN 0.4
#define ELEVATION_RETRACT 555
#define ELEVATION_DRILL 0

/***************variable wasteland********************/
uint8_t elevator_upordown = 0;
int elevation = 0;
float filter_dutycycle = 0.6;
uint8_t bin_loc = 0;
uint8_t filter_dir = 0;



/*****************************************************/

float incoming_cmd[NUM_CMDS] = { 0 }; //Array to hold incoming CAN messages
float joy_cmd[NUM_CMDS] = { 0 }; //Can we just reuse incoming_cmd?

//Flags
volatile uint8_t data_ready = 0;

//Counter for timer
volatile uint32_t millis = 0;

//Bitfield for limit switch readings
uint8_t limit_switch_readings = 0;

//Duty cycle of each motor
float drill_direction = 0.5;
float elevator_direction = 0.5;
float sample_direction = 0.5;

//Direction of each motor. 0 is backwards and 1 is forwards.
//Maybe this can be a bitfield instead
uint8_t drill_direction = 0;
uint8_t elevator_direction = 0;
uint8_t sample_direction = 0;

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

void TIM14_IRQHandler()
{
    HAL_TIM_IRQHandler(&s_TimerInstance);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // do stuff in here

    // add logic to only increment when start signal occurs
    millis++;
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

int CAN_Init(uint32_t can_id)
{
    if (CANLIB_Init(can_id, 0) != 0)
    {
        return -1;
    }

    CANLIB_AddFilter(CAN_DRILL_ELEVATOR_ID);
    CANLIB_AddFilter(CAN_SAMPLE_ID);
    CANLIB_AddFilter(CAN_TRIGGER_ID);
    CANLIB_AddFilter(CAN_STOP_ID);

    return 0;
}

void sensorInit(void)
{
    //TODO;
}

void i2cInit(void)
{

}

void runElevator(int elevator_upordown)
{
    // TODO: read CAN message to run elevator
    // elevator_upordown : 1 = up, 0 = down
    // TODO: elevation = read_elevator_enc
    if(elevator_upordown)
    {
        while(elevation < ELEVATION_RETRACT)
        {
            PWMLIB_Write(PWM_ELEVATOR_ID,ELEVATOR_DUTY_CYCLE_UP);
            HAL_Delay(200);
        }
    }
    else
    {
        while(elevation > ELEVATION_DRILL)
        {
            PWMLIB_Write(PWM_ELEVATOR_ID,ELEVATOR_DUTY_CYCLE_DOWN);
            HAL_Delay(200);
        }
    }
}

void runDrillDistance(float drill_dutycycle, float elevator_duty_cycle, float distance)
{
    int drill_dir = 0;
    if(elevation > distance)
    {
        drill_dir = 1;
        int elevator_dir = 1;
        while(elevation > distance)
        {
            HAL_GPIO_WritePin(GPIO_DRILL_PORT,GPIO_DRILL_PIN,drill_dir);
            PWMLIB_Write(PWM_DRILL_ID,drill_dutycycle);
            HAL_GPIO_WritePin(GPIO_ELEVATOR_PORT,GPIO_ELEVATOR_PIN,elevator_dir);
            PWMLIB_Write(PWM_DRILL_ID,elevator_duty_cycle);
            HAL_Delay(200);
        }
    }
    else
    {
        drill_dir = 0;
        int elevator_dir = 0;
        while(elevation < distance)
        {
            HAL_GPIO_WritePin(GPIO_DRILL_PORT,GPIO_DRILL_PIN,drill_dir);
            PWMLIB_Write(PWM_DRILL_ID,drill_dutycycle);
            HAL_GPIO_WritePin(GPIO_ELEVATOR_PORT,GPIO_ELEVATOR_PIN,elevator_dir);
            PWMLIB_Write(PWM_DRILL_ID,elevator_duty_cycle);
            HAL_Delay(200);
        }
    }
}

void runFilterToBin(int bin_no)
{
    //determine logic to go to certain filter

    uint8_t filter_limits_reading = 1;
    while(bin_loc != bin_no)
    {
        HAL_GPIO_WritePin(GPIO_FILTER_PORT,GPIO_FILTER_PIN,filter_dir);
        PWMLIB_Write(PWM_FILTER_ID,filter_dutycycle);
        HAL_Delay(100);
        filter_limits_reading = HAL_GPIO_ReadPin(LIMIT_SWITCH_FILTER_INNER_PORT, LIMIT_SWITCH_FILTER_INNER_PIN);
        if(!filter_limits_reading)
        {
            bin_loc++;
            bin_loc = bin_loc%4;
        }
    }
}

void runFiltertoPassThrough(void)
{
    //determine limit switch logic to run filter to pass through
    runFilterToBin(4);
}

void resetFilter(void)
{
    // drive motor
    uint8_t filter_zeroed = 0;
    uint8_t filter_limits_reading[2] = {1};
    while(!filter_zeroed)
    {
        filter_limits_reading[0] = HAL_GPIO_ReadPin(LIMIT_SWITCH_FILTER_OUTER_PORT, LIMIT_SWITCH_FILTER_OUTER_PIN); //pull-up switch 0 when pressed
        HAL_GPIO_WritePin(GPIO_FILTER_PORT,GPIO_FILTER_PIN,filter_dir);
        PWMLIB_Write(PWM_FILTER_ID,filter_dutycycle);
        HAL_Delay(200);
        if (!filter_limits_reading[0])
            do{
                PWMLIB_Write(PWM_FILTER_ID,filter_dutycycle/2);
                filter_limits_reading[1] = HAL_GPIO_ReadPin(LIMIT_SWITCH_FILTER_INNER_PORT, LIMIT_SWITCH_FILTER_INNER_PIN);
                HAL_Delay(100);
            } while(filter_limits_reading[1]);
            filter_zeroed = 1;
    }
    bin_loc = 0;
}

float getHumidity(int sensor_no)
{
    read_hum(device ptr)
}

float getTemp(int sensor_no)
{

}

float getGas(int sensor_no)
{

}

float getUV(int sensor_no)
{

}

int main(void)
{
    HAL_Init();
    CLK_Init();
    GPIO_Init();
    Timer_Init(PERIOD); // 1000 ms timer

    if (PWMLIB_Init(PWM_DRILL_ID) != 0)
    {
        Error_Handler();
    }
    if (PWMLIB_Init(PWM_ELEVATOR_ID) != 0)
    {
        Error_Handler();
    }
    if (PWMLIB_Init(PWM_DRILL_ID) != 0)
    {
        Error_Handler();
    }

    if (CAN_Init(CAN_LIMIT_SWITCH_ID) != 0)
    {
        Error_Handler();
    }

    return 0;
}

void CANLIB_Rx_OnMessageReceived(void)
{
    switch(CANLIB_Rx_GetSenderID())
    {
        // Frame format: 1st float is drill duty cycle, 2nd float is elevator
        // duty cycle
        case CAN_DRILL_ELEVATOR_ID:
            break;

        // Frame format: 1st float is sample selector duty cycle
        case CAN_SAMPLE_ID:
            break;

        // Frame format: uint that is either 1 (trigger) or 0 (no trigger)
        // Should act more like a flag, where action only taken on trigger
        case CAN_TRIGGER_ID:
            break;

        // Frame format: similar to trigger, but for stopping
        case CAN_STOP_ID:
            break;

        default:
            break;
    }
}
