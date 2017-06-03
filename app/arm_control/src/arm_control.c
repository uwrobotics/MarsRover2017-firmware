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
//#include "encoderlib.h"
#include "pins.h"
#include <math.h>
#include <string.h>

//Code assumes that one board is in charge of motors controlling inclination and azimuth of a joint
//Example of azimuth vs inclination: http://edndoc.esri.com/arcobjects/9.1/java/arcengine/com/esri/arcgis/geometry/bitmaps/GeomVector3D.gif

//1 for shoulder/turntable, 2 for elbow, 3 for wrist
#define BOARD_TYPE                  3

//PWM IDs
#define PWM_AZIMUTH_ID              1
#define PWM_INCLINATION_ID          2

//CAN IDs that this will receive messages from
#if BOARD_TYPE == 1
#define CAN_RX_ID                   500  // Shoulder
#elif BOARD_TYPE == 2
#define CAN_RX_ID                   501  // Elbow
#elif BOARD_TYPE == 3
#define CAN_RX_ID                   502  // Wrist
#endif

//CAN IDs that this will code will transmit on
#define CAN_TX_ID                   304  //Arbitrary value
// #define CAN_ENCODER_DATA_ID     16  //Arbitrary value
#define CAN_LIMIT_SW_TABLE_ID       300
#define CAN_LIMIT_SW_ELBOW_ID       301
#define CAN_LIMIT_SW_SHOULDER_ID    302

//Number of PWM commands per relevant CAN frame received
#define NUM_CMDS                    2
//Timer interrupt interval
#define PERIOD                      500
//Number of timer intervals of no message received to enter watchdog state
#define MSG_WATCHDOG_INTERVAL       1

//Index in received CAN frame for float for each axis motors
//The first 4 bytes contain azimuth motor PWM command and the last 4 contain inclination motor PWM command
#define AZIMUTH_AXIS_ID             0
#define INCLINATION_AXIS_ID         1


// Limit Switches
#define LIMIT_SWITCH_1_PIN      GPIO_PIN_12
#define LIMIT_SWITCH_1_PORT     GPIOC
#define LIMIT_SWITCH_2_PIN      GPIO_PIN_13
#define LIMIT_SWITCH_2_PORT     GPIOC
#define LIMIT_SWITCH_3_PIN      GPIO_PIN_14
#define LIMIT_SWITCH_3_PORT     GPIOC
#define LIMIT_SWITCH_4_PIN      GPIO_PIN_15
#define LIMIT_SWITCH_4_PORT     GPIOC

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

//volatile uint64_t ms_elapsed = 0;

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
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //blue led (LD6)

    //If msg_received is greater than the set limit, then reset (or do something else)
    if (msg_received > MSG_WATCHDOG_INTERVAL)
    {
        //Watchdog expiration
        //NVIC_SystemReset();
        //Do something else?
    }
    //motor_duty_cycle != 0 implies that the motor is moving
    else if (msg_received != 0 && (azimuth_motor_duty_cycle || inclination_motor_duty_cycle))
    {
        //Increment message received if no message received but motor is moving
        msg_received++;
    }
    else
    {
        //This sets msg_received to 1 if either the azimuth or inclination motors are moving
        msg_received = 1 && (azimuth_motor_duty_cycle || inclination_motor_duty_cycle);
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
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // LEDs
    GPIO_InitTypeDef LED_InitStruct = {
            .Pin        = GPIO_PIN_10 | GPIO_PIN_11,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &LED_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);

    // Direction control pins
    GPIO_InitTypeDef DirCtrl_InitStruct = {
            .Pin        = GPIO_PIN_8 | GPIO_PIN_9, //Pin 8 for azimuth, pin 9 for inclination. Change as needed
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &DirCtrl_InitStruct);

#if BOARD_TYPE == 1
    //Limit switch init. Assumes 4 limit switches
    GPIO_InitTypeDef LimitSwitch_InitStruct = {
        .Pin            = LIMIT_SWITCH_1_PIN | LIMIT_SWITCH_2_PIN | LIMIT_SWITCH_3_PIN | LIMIT_SWITCH_4_PIN,
        .Mode           = GPIO_MODE_INPUT,
        .Pull           = GPIO_NOPULL,
        .Speed          = GPIO_SPEED_FREQ_HIGH
    };

    HAL_GPIO_Init(GPIOC, &LimitSwitch_InitStruct);
#elif BOARD_TYPE == 2
    //Limit switch init. Assumes 2 limit switches
    GPIO_InitTypeDef LimitSwitch_InitStruct = {
        .Pin            = LIMIT_SWITCH_1_PIN | LIMIT_SWITCH_2_PIN,
        .Mode           = GPIO_MODE_INPUT,
        .Pull           = GPIO_NOPULL,
        .Speed          = GPIO_SPEED_FREQ_HIGH
    };

    HAL_GPIO_Init(GPIOC, &LimitSwitch_InitStruct);
#endif

    // Alternate direction pin
    GPIO_InitTypeDef ALT_PIN = {
        .Pin            = GPIO_PIN_11,
        .Mode           = GPIO_MODE_OUTPUT_PP,
        .Pull           = GPIO_NOPULL,
        .Speed          = GPIO_SPEED_FREQ_HIGH
    };

    HAL_GPIO_Init(GPIOB, &ALT_PIN);
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


    if (CANLIB_Init(CAN_TX_ID, 0) != 0)
    {
        Error_Handler();
    }
    if (CANLIB_AddFilter(CAN_RX_ID) != 0)
    {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(TIM14_IRQn, 1, 2);
    HAL_NVIC_EnableIRQ(TIM14_IRQn);

    if (PWMLIB_Init(PWM_AZIMUTH_ID) != 0)
    {
        Error_Handler();
    }
    if (PWMLIB_Init(PWM_INCLINATION_ID) != 0)
    {
        Error_Handler();
    }

    //Encoder stuff doesnt work
    //uint32_t encoder_1_reading = 0;
    //uint32_t encoder_2_reading = 0;

    //This encoder stuff doesnt work
    /*if (EncoderLib_Init(ENCODER1))
    {
        Error_Handler();
    }
    else if (EncoderLib_Init(ENCODER2))
    {
        Error_Handler();
    }*/

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);

    while(1)
    {
        /*
        The code here would get and send encoder readings over CAN, given a working library

        encoder_1_reading = EncoderLib_ReadCount(ENCODER1);
        encoder_2_reading = EncoderLib_ReadCount(ENCODER2);
        CANLIB_ChangeID(CAN_ENCODER_READ_ID);
        CANLIB_Tx_SetInt(encoder_1_reading, CANLIB_INDEX_0);
        CANLIB_Tx_SetInt(encoder_2_reading, CANLIB_INDEX_1);
        CANLIB_Tx_SendData(CANLIB_DLC_ALL_BYTES);
        */

        //Read limit switches into bitfield and shut off appropriate motor as req'd and send reading over CAN
        //Assumes limit switches will output high when hit
#if BOARD_TYPE == 1
        limit_switch_readings =
            HAL_GPIO_ReadPin(LIMIT_SWITCH_1_PORT, LIMIT_SWITCH_1_PIN) |
            HAL_GPIO_ReadPin(LIMIT_SWITCH_2_PORT, LIMIT_SWITCH_2_PIN) << 1 |
            HAL_GPIO_ReadPin(LIMIT_SWITCH_3_PORT, LIMIT_SWITCH_3_PIN) << 2 |
            HAL_GPIO_ReadPin(LIMIT_SWITCH_4_PORT, LIMIT_SWITCH_4_PIN) << 3;
#elif BOARD_TYPE == 2
        limit_switch_readings =
            HAL_GPIO_ReadPin(LIMIT_SWITCH_1_PORT, LIMIT_SWITCH_1_PIN) |
            HAL_GPIO_ReadPin(LIMIT_SWITCH_2_PORT, LIMIT_SWITCH_2_PIN) << 1;
#endif

// Check extra switches for turntable
#if BOARD_TYPE == 1
        //only stop if the motion is in the direction that triggers limit
        if (limit_switch_readings & 0b0100 && azimuth_direction == 0)
        {
            PWMLIB_Write(PWM_AZIMUTH_ID, inclination_motor_duty_cycle = 0);
        }
        //only stop if the motion is in the direction that triggers limit
        if (limit_switch_readings & 0b1000 && azimuth_direction == 1)
        {
            PWMLIB_Write(PWM_AZIMUTH_ID, inclination_motor_duty_cycle = 0);
        }
        CANLIB_Tx_SetByte((limit_switch_readings >> 2), 0);
        CANLIB_Tx_SetByte(0, 1);
        CANLIB_ChangeID(CAN_LIMIT_SW_SHOULDER_ID);
        CANLIB_Tx_SendData(CANLIB_DLC_ALL_BYTES);
#endif
// Don't check limit switches if wrist board
#if BOARD_TYPE != 3
        if (limit_switch_readings & 0b0001 && inclination_direction == 0)
        {
            PWMLIB_Write(PWM_INCLINATION_ID, azimuth_motor_duty_cycle = 0);
        }
        if (limit_switch_readings & 0b0010 && inclination_direction == 1)
        {
            PWMLIB_Write(PWM_INCLINATION_ID, azimuth_motor_duty_cycle = 0);
        }
        CANLIB_Tx_SetByte(limit_switch_readings, 0);
        CANLIB_Tx_SetByte(0, 1);
    #if BOARD_TYPE == 1
        CANLIB_ChangeID(CAN_LIMIT_SW_TABLE_ID);
    #elif BOARD_TYPE == 2
        CANLIB_ChangeID(CAN_LIMIT_SW_ELBOW_ID);
    #endif
        CANLIB_Tx_SendData(CANLIB_DLC_ALL_BYTES);
#endif

        //If there's no incoming data, restart the loop
        //Otherwise a message was received to change duty cycle
        if (!(data_ready))
        {
            continue;
        }

        data_ready = 0;
        //Reset the variable that counts timer intervals before a software watchdog
        msg_received = 0;
        memcpy(joy_cmd, incoming_cmd, NUM_CMDS * sizeof(*incoming_cmd));

        //For azimuth motors:
        //   If joystick command is 0 or close enough, set to 0
        //   Else if azimuth limit switch not hit, or direction of command is opposite of direction that triggered limit switch:
        //       Set duty cycle and direction switch accordingly as well as direction and duty cycle variables
        if (joy_cmd[AZIMUTH_AXIS_ID] > (0.0 - epsilon) && joy_cmd[AZIMUTH_AXIS_ID] < (0.0 + epsilon))
        {
            PWMLIB_Write(PWM_AZIMUTH_ID, azimuth_motor_duty_cycle = 0);
        }
        else if (!(limit_switch_readings & 0b0001) || (azimuth_direction != (joy_cmd[AZIMUTH_AXIS_ID] > 0)))
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, azimuth_direction = joy_cmd[AZIMUTH_AXIS_ID] > 0);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, azimuth_direction = joy_cmd[AZIMUTH_AXIS_ID] > 0);
            PWMLIB_Write(PWM_AZIMUTH_ID, azimuth_motor_duty_cycle = fmin(fabs(joy_cmd[AZIMUTH_AXIS_ID]), 0.5f));
        }

        //Same as above but for azimuth motors
        if (joy_cmd[INCLINATION_AXIS_ID] > (0.0 - epsilon) && joy_cmd[INCLINATION_AXIS_ID] < (0.0 + epsilon))
        {
            PWMLIB_Write(PWM_INCLINATION_ID, inclination_motor_duty_cycle = 0);
        }
        else if (!(limit_switch_readings & 0b0010) || (inclination_direction != (joy_cmd[INCLINATION_AXIS_ID] > 0)))
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, inclination_direction = joy_cmd[INCLINATION_AXIS_ID] > 0);
            PWMLIB_Write(PWM_INCLINATION_ID, inclination_motor_duty_cycle = fmin(fabs(joy_cmd[INCLINATION_AXIS_ID]), 0.5f));
        }

    }
    return 0;
}

void CANLIB_Rx_OnMessageReceived(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);
    switch(CANLIB_Rx_GetSenderID())
    {
        // Expect frame format to be:
        // Bytes 0-3: Azimuth axis PWM input
        // Byte 4-7: Inclination axis PWM input
        case CAN_RX_ID:
            incoming_cmd[AZIMUTH_AXIS_ID] = CANLIB_Rx_GetAsFloat(AZIMUTH_AXIS_ID);
            incoming_cmd[INCLINATION_AXIS_ID] = CANLIB_Rx_GetAsFloat(INCLINATION_AXIS_ID);
            data_ready = 1;
            break;

        default:
            break;
    }
}
