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
#include "i2clib.h"
#include "humidity_temperature.h"
#include "uv.h"
#include "uart_lib.h"
//#include "encoderlib.h"
#include "pins.h"
#include <math.h>
#include <string.h>

//
/*
    LIMIT 1= PC12
    LIMIT 2 =PC13
    LIMIT 3 =PC14
    LIMIT 4 =PC15

    MOTORPWM1 = A8
    MOTORPWM2 = A9
    MOTORPWM3 = A10

    MOTORDIR1 = C8
    MOTORDIR2 = C9
    MOTORDIR3 = C10

    FAMPWM = C11

    LEDS1 = C0
    LEDS2 = C1
    LEDS_STAY = PC4
    LEDS_CAN = PC5
*/
// Limit Switches TODO: REMOVE
#define LIMIT_SWITCH_ELEVATORTOP_PIN        GPIO_PIN_12
#define LIMIT_SWITCH_ELEVATORTOP_PORT       GPIOC
#define LIMIT_SWITCH_ELEVATORBOTTOM_PIN     GPIO_PIN_13
#define LIMIT_SWITCH_ELEVATORBOTTOM_PORT    GPIOC

// Limit Switches for Application TODO: CHECK PINS
// #define LIMIT_SWITCH_FILTER_OUTER_PIN       GPIO_PIN_13
// #define LIMIT_SWITCH_FILTER_OUTER_PORT      GPIOC
// #define LIMIT_SWITCH_FILTER_INNER_PIN       GPIO_PIN_12
// #define LIMIT_SWITCH_FILTER_INNER_PORT      GPIOC

#define GPIO_DRILL_DIR_PIN                  GPIO_PIN_8
#define GPIO_DRILL_DIR_PORT                 GPIOC
#define GPIO_ELEVATOR_DIR_PIN               GPIO_PIN_9
#define GPIO_ELEVATOR_DIR_PORT              GPIOC
// #define GPIO_FILTER_DIR_PIN                 GPIO_PIN_10
// #define GPIO_FILTER_DIR_PORT                GPIOC

// Timer interrupt period
#define PERIOD                              1000

// CAN IDs
#define CAN_DRILL_ELEVATOR_ID               700
// #define CAN_SAMPLE_ID                       701
#define CAN_LIMIT_SWITCH_ID                 702
#define CAN_UV_ID                           703
#define CAN_GAS_ID                          704
#define CAN_TMP_1_ID                        705
#define CAN_HUM_1_ID                        706
#define CAN_TMP_2_ID                        707
#define CAN_HUM_2_ID                        708
#define CAN_TMP_3_ID                        709
#define CAN_HUM_3_ID                        710
#define CAN_START_STOP_ID                   711
#define CAN_ID_STEP                         CAN_TMP_2_ID - CAN_TMP_1_ID

// Directions
#define MOTOR_FWD_DIR                       1
#define MOTOR_RVR_DIR                       0

// Motor indexes
#define NUM_MOTORS                          1
#define DRILL_IDX                           0
// #define ELEVATOR_IDX                        1
// #define SAMPLE_IDX                          2

// PWM IDs
#define PWM_DRILL_ID                        1
#define PWM_ELEVATOR_ID                     2
// #define PWM_FILTER_ID                       3

// Maximums
#define MAX_DRILL_DUTY_CYCLE                0.5
#define MAX_ELEVATOR_DUTY_CYCLE             0.1

#define DEBUG_MODE                          0

/***************variable wasteland********************/
uint8_t bin_loc = 0;
int access_id = -1;
float number1 = 0.25;
float number2 = 0.69;

uint8_t limit_switch_readings = 0xff;

/*  LIMIT SWITCH BIT VECTOR
    elevator top        = 0000 0001;
    elevator bottom     = 0000 0010; */

/*****************************************************/

//const float epsilon = 0.0001;
//float incoming_cmd[NUM_CMDS] = { 0 }; //Array to hold incoming CAN messages
//float joy_cmd[NUM_CMDS] = { 0 }; //Can we just reuse incoming_cmd?

//Flags
volatile uint8_t data_ready = 0; // Only place this is used is in the comments
volatile uint8_t start = 0; // flag for starting and stopping the process

//Counter for timer
volatile uint32_t millis = 0; // Don't think this is even used

volatile float drill_cmd = 0;
volatile int elevator_cmd = 0;

/*   Initialization of the two sensors  */
HT_Device_t ht_sensor;
I2C_Device_t uv_sensor;
const uv_it_t INT_TIME = 250;


static TIM_HandleTypeDef s_TimerInstance =
{
    .Instance = TIM14
};

static void Error_Handler(void)
{

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

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
    uint32_t timeStamp; // TODO time stamp

    // add logic to only increment when start signal occurs
    if (start)
    {
        for (int id = CAN_TMP_1_ID; id < CAN_TMP_3_ID; id += CAN_ID_STEP)
        {
            // Humidity
            float humidity = read_hum(&ht_sensor);
            CANLIB_ChangeID(id+1);
            CANLIB_Tx_SetFloat(humidity, CANLIB_INDEX_0); // socketcan to topic assumes MSB bytes are float
            CANLIB_Tx_SetUint(timeStamp, CANLIB_INDEX_1);
            CANLIB_Tx_SendData(CANLIB_DLC_ALL_BYTES);

            // Temperature
            float temperature = read_temp(&ht_sensor, 1); // uses temperature measured when reading humidity
            CANLIB_ChangeID(id);
            CANLIB_Tx_SetFloat(temperature, CANLIB_INDEX_0); // socketcan to topic assumes MSB bytes are float
            CANLIB_Tx_SetUint(timeStamp, CANLIB_INDEX_1);
            CANLIB_Tx_SendData(CANLIB_DLC_ALL_BYTES);
        }

        uint32_t uv_data = read_uv(&uv_sensor, &INT_TIME);
        uv_class_t uv_class = get_uv_class(&uv_sensor, &uv_data, &INT_TIME); // Where would this be used?
        CANLIB_ChangeID(CAN_UV_ID);
        CANLIB_Tx_SetUint(uv_data, CANLIB_INDEX_0);
        CANLIB_Tx_SetUint(timeStamp, CANLIB_INDEX_1);
        CANLIB_Tx_SendData(CANLIB_DLC_ALL_BYTES);
        CANLIB_Tx_SetUint(uv_class, CANLIB_INDEX_0);
        CANLIB_Tx_SetUint(0, CANLIB_INDEX_1);
        CANLIB_Tx_SendData(CANLIB_DLC_ALL_BYTES);

        // Increment timer
        millis++;
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

    /*
    LIMIT 1 = PC12
    LIMIT 2 = PC13
    LIMIT 3 = PC14
    LIMIT 4 = PC15

    MOTORPWM1 = A8
    MOTORPWM2 = A9
    MOTORPWM3 = A10

    MOTORDIR1 = C8
    MOTORDIR2 = C9
    MOTORDIR3 = C10

    FAMPWM = C11

    LEDS1 = C0
    LEDS2 = C1
    LEDS_STAY = PC4
    LEDS_CAN = PC5
    */

    // LED pins
    GPIO_InitTypeDef LED_InitStruct = {
            .Pin        = GPIO_PIN_0 | GPIO_PIN_1,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &LED_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

    // Direction pins
    GPIO_InitTypeDef MotorDir_InitStruct = {
            .Pin        = GPIO_DRILL_DIR_PIN | GPIO_ELEVATOR_DIR_PIN, // | GPIO_FILTER_DIR_PIN,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &MotorDir_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_DRILL_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_ELEVATOR_DIR_PIN, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOC, GPIO_FILTER_DIR_PIN, GPIO_PIN_RESET);

    // Limit switch pins
    GPIO_InitTypeDef LimitSwitch_InitStruct = {
        .Pin            = LIMIT_SWITCH_ELEVATORBOTTOM_PIN | LIMIT_SWITCH_ELEVATORTOP_PIN, // | LIMIT_SWITCH_FILTER_INNER_PIN | LIMIT_SWITCH_FILTER_OUTER_PIN;
        .Mode           = GPIO_MODE_INPUT,
        .Pull           = GPIO_PULLUP,
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

int PWM_Init(void)
{
    if (PWMLIB_Init(PWM_DRILL_ID) != 0)
    {
        return -1;
    }
    if (PWMLIB_Init(PWM_ELEVATOR_ID) != 0)
    {
        return -1;
    }
    // if (PWMLIB_Init(PWM_FILTER_ID) != 0)
    // {
    //     return -1;
    // }

    return 0;
}

int CAN_Init(uint32_t can_id)
{
    if (CANLIB_Init(can_id, 0) != 0)
    {
        return -1;
    }

    CANLIB_AddFilter(CAN_DRILL_ELEVATOR_ID);
    //CANLIB_AddFilter(CAN_SAMPLE_ID);
    CANLIB_AddFilter(CAN_START_STOP_ID);

    return 0;
}

void Sensor_Init(void)
{
    //TODO;
        /*     Initialization of the uv and ht sensors    */
    init_ht(&ht_sensor, 5000); // Timeout of 5000 ms
    init_uv(&uv_sensor, &INT_TIME, 5000);
}

/*  LIMIT SWITCH BIT VECTOR
    elevator bottom         = 0000 0010;
    elevator top            = 0000 0001;
*/
void checkLimits(void)
{
    limit_switch_readings = HAL_GPIO_ReadPin(LIMIT_SWITCH_ELEVATORTOP_PORT, LIMIT_SWITCH_ELEVATORTOP_PIN) |
                            HAL_GPIO_ReadPin(LIMIT_SWITCH_ELEVATORBOTTOM_PORT, LIMIT_SWITCH_ELEVATORBOTTOM_PIN) << 1;
#if DEBUG_MODE
    UART_LIB_PRINT_INT(limit_switch_readings);
#endif
}

void runDrillDistance(float drill_duty_cycle, float elevator_duty_cycle)
{
    int drill_dir = MOTOR_FWD_DIR;
    int elevator_dir = MOTOR_FWD_DIR;
    float drill_speed, elevator_speed;

    // Assuming 0 is down
    if (drill_duty_cycle < 0)
    {
        drill_dir = MOTOR_RVR_DIR;
    }
    if (elevator_duty_cycle < 0)
    {
        elevator_dir = MOTOR_RVR_DIR;
    }

    // Check and cap inputs
    if (fabs(drill_duty_cycle) > MAX_DRILL_DUTY_CYCLE)
    {
        drill_speed = MAX_DRILL_DUTY_CYCLE;
    }
    else
    {
        drill_speed = drill_duty_cycle;
    }

    if (fabs(elevator_duty_cycle) > MAX_ELEVATOR_DUTY_CYCLE)
    {
        elevator_speed = MAX_ELEVATOR_DUTY_CYCLE;
    }
    else
    {
        elevator_speed = elevator_duty_cycle;
    }

    checkLimits();
    // hit top (limit switches are active low)
    if ((elevator_duty_cycle > 0) && (limit_switch_readings & 0x01))
    {
        PWMLIB_Write(PWM_ELEVATOR_ID, 0.0);
        return;
    }
    // hit bottom (limit switches are active low)
    if ((elevator_duty_cycle < 0) && (limit_switch_readings & 0x02) >> 1)
    {
        PWMLIB_Write(PWM_ELEVATOR_ID, 0.0);
        return;
    }

    HAL_GPIO_WritePin(GPIO_DRILL_DIR_PORT, GPIO_DRILL_DIR_PIN, drill_dir);
    HAL_GPIO_WritePin(GPIO_ELEVATOR_DIR_PORT, GPIO_ELEVATOR_DIR_PIN, elevator_dir);
    PWMLIB_Write(PWM_DRILL_ID, drill_speed);
    PWMLIB_Write(PWM_ELEVATOR_ID, elevator_speed);
}

// Blocking function that resets motors to original position
void resetElevator(void)
{
    // Move elevator up
    HAL_GPIO_WritePin(GPIO_ELEVATOR_DIR_PORT, GPIO_ELEVATOR_DIR_PIN, MOTOR_FWD_DIR);
    do
    {
        checkLimits();
        if (limit_switch_readings & 0b01)
        {
            PWMLIB_Write(PWM_ELEVATOR_ID, MAX_ELEVATOR_DUTY_CYCLE);
        }
        else
        {
            PWMLIB_Write(PWM_ELEVATOR_ID, 0.0);
        }
    } while (!limit_switch_readings & 0x01);
    PWMLIB_Write(PWM_ELEVATOR_ID, 0.0);
}

// void resetFilter(uint8_t filter_dir, float filter_duty_cycle)
// {
//     // drive motor
//     uint8_t filter_zeroed = 0;
//     //uint8_t filter_limits_reading[2] = {1};
//     while (!filter_zeroed)
//     {
//         checkLimits();
//         //limit_switch_readings
// //        uint8_t result, lim1, lim2;
// //        result =
// //        result = lim1 << 1 | ;
// //        1010
//         HAL_GPIO_WritePin(GPIO_FILTER_DIR_PORT, GPIO_FILTER_DIR_PIN, filter_dir);
//         PWMLIB_Write(PWM_FILTER_ID, filter_duty_cycle);
//         HAL_Delay(100);
//         //if (!filter_limits_reading[0])
//         if (!(limit_switch_readings & 0x02))
//         {
//             do
//             {
//                 checkLimits();
//                 PWMLIB_Write(PWM_FILTER_ID, filter_duty_cycle/2);
//                 HAL_Delay(50);
//             } while (limit_switch_readings & 0x01);
//             filter_zeroed = 1;
//         }
//     }
//     bin_loc = 0;
// }

// void runFilterToBin(int bin_no, float filter_duty_cycle)
// {
//     //determine logic to go to certain filter
//     uint8_t filter_dir = 1;
//     if (filter_duty_cycle > 0)
//     {
//         filter_dir = 0;
//     }

//     if (bin_no == -1)
//     {
//         resetFilter(bin_no, filter_duty_cycle);
//     }

//     //uint8_t filter_limits_reading = 1;
//     while(bin_loc != bin_no)
//     {
//         HAL_GPIO_WritePin(GPIO_FILTER_DIR_PORT, GPIO_FILTER_DIR_PIN, filter_dir);
//         PWMLIB_Write(PWM_FILTER_ID, fabs(filter_duty_cycle));
//         HAL_Delay(100);
//         //filter_limits_reading = HAL_GPIO_ReadPin(LIMIT_SWITCH_FILTER_INNER_PORT, LIMIT_SWITCH_FILTER_INNER_PIN);
//         checkLimits();
//         if(!(limit_switch_readings & 0x01))
//         {
//             bin_loc++;
//             bin_loc = bin_loc%4;
//         }
//     }
// }

int main(void)
{
    HAL_Init();
    CLK_Init();
    GPIO_Init();
    Sensor_Init();
    Timer_Init(PERIOD); // 500 ms timer

    if (PWM_Init() != 0)
    {
        Error_Handler();
    }
    if (CAN_Init(CAN_LIMIT_SWITCH_ID) != 0)
    {
        Error_Handler();
    }

#if DEBUG_MODE
    UART_LIB_INIT();
#endif

    HAL_NVIC_SetPriority(TIM14_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(TIM14_IRQn);

    while (1)
    {
        if (start)
        {
            runDrillDistance(drill_cmd, elevator_cmd * MAX_ELEVATOR_DUTY_CYCLE);
        }
    }

    return 0;
}

// CAN Rx Callback
void CANLIB_Rx_OnMessageReceived(void)
{
    switch(CANLIB_Rx_GetSenderID())
    {
        // Message Organization:
        // drill_cmd -- 1st float in CAN_DRILL_ELEVATOR_ID
        // elevator_cmd -- 2nd int in CAN_DRILL_ELEVATOR_ID (used as direction for now: 1 for up, -1 for down, 0 for nothing)
        case CAN_DRILL_ELEVATOR_ID:
            drill_cmd = CANLIB_Rx_GetAsFloat(0);
            elevator_cmd = CANLIB_Rx_GetAsInt(1);
            break;

        // case CAN_SAMPLE_ID:
        //     incoming_msg[SAMPLE_IDX] = CANLIB_Rx_GetAsFloat(0);
        //     break;

        case CAN_START_STOP_ID:
            start = !start;
            if (!start)
            {
                millis = 0;
            }
            break;

        default:
            break;
    }
}
