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
#define LIMIT_SWITCH_ELEVATORTOP_PIN        GPIO_PIN_15
#define LIMIT_SWITCH_ELEVATORTOP_PORT       GPIOC
#define LIMIT_SWITCH_ELEVATORBOTTOM_PIN     GPIO_PIN_14
#define LIMIT_SWITCH_ELEVATORBOTTOM_PORT    GPIOC

// Limit Switches for Application TODO: CHECK PINS
#define LIMIT_SWITCH_FILTER_OUTER_PIN       GPIO_PIN_13
#define LIMIT_SWITCH_FILTER_OUTER_PORT      GPIOC
#define LIMIT_SWITCH_FILTER_INNER_PIN       GPIO_PIN_12
#define LIMIT_SWITCH_FILTER_INNER_PORT      GPIOC

#define GPIO_DRILL_DIR_PIN                  GPIO_PIN_8
#define GPIO_DRILL_DIR_PORT                 GPIOC
#define GPIO_ELEVATOR_DIR_PIN               GPIO_PIN_9
#define GPIO_ELEVATOR_DIR_PORT              GPIOC
#define GPIO_FILTER_DIR_PIN                 GPIO_PIN_10
#define GPIO_FILTER_DIR_PORT                GPIOC

// Timer interrupt interval
#define PERIOD                              000

// Can IDs
#define CAN_DRILL_ELEVATOR_ID               700
#define CAN_SAMPLE_ID                       701
#define CAN_LIMIT_SWITCH_ID                 702
#define CAN_UV_ID                           703
#define CAN_GAS_ID                          704
#define CAN_TMP_1_ID                        705
#define CAN_HUM_1_ID                        706
#define CAN_TMP_2_ID                        707
#define CAN_HUM_2_ID                        708
#define CAN_TMP_3_ID                        709
#define CAN_HUM_3_ID                        710
#define CAN_TRIGGER_ID                      711
#define CAN_STOP_ID                         712

#define CAN_FILTERID_ELEVATOR_DRILL         12
#define CAN_FILTERID_FILTER                 13


#define MOTOR_FWD_DIR                       1
#define MOTOR_RVR_DIR                       0

// PWM ID FOR MOTORS TODO: PLEASE SET
#define PWM_DRILL_ID                        1
#define PWM_ELEVATOR_ID                     2
#define PWM_FILTER_ID                       3

#define DEBUG_MODE                          1

/***************variable wasteland********************/
uint8_t bin_loc = 0;
float incoming_msg[2] = {0};
int access_id = -1;
float number1 = 0.25;
float number2 = 0.69;

uint8_t limit_switch_readings = 0xff;

/*  LIMIT SWITCH BIT VECTOR
    elevator retracted          = 0000 1000;
    elevator extended           = 0000 0100;
    sample selector inner limit = 0000 0010;
    sample selector outer limit = 0000 0001;

/*****************************************************/

//const float epsilon = 0.0001;
//float incoming_cmd[NUM_CMDS] = { 0 }; //Array to hold incoming CAN messages
//float joy_cmd[NUM_CMDS] = { 0 }; //Can we just reuse incoming_cmd?

//Flags
volatile uint8_t data_ready = 0; // Only place this is used is in the comments

//Counter for timer
volatile uint32_t millis = 0; // Don't think this is even used



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

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

    while (1)
    {
        //can anything useful be done here?
    }
}


static void Error_Handler_Motors(void)
{

#if DEBUG_MODE
    uint8_t debug_msg[] = "motors not connected";
    UART_LIB_PRINT_CHAR_ARRAY(debug_msg,sizeof(debug_msg));
#endif

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

    uint32_t timeStamp; // TODO time stamp


    for (int id=CAN_TMP_1_ID; id<CAN_TMP_3_ID; id+=2)
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

    // LEDs and M
    GPIO_InitTypeDef MotorDir_InitStruct = {
            .Pin        = GPIO_DRILL_DIR_PIN | GPIO_ELEVATOR_DIR_PIN | GPIO_FILTER_DIR_PIN,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &MotorDir_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_DRILL_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_ELEVATOR_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_FILTER_DIR_PIN, GPIO_PIN_RESET);

    // Direction control pins
    /*
    GPIO_InitTypeDef DirCtrl_InitStruct = {
            .Pin        = GPIO_PIN_5 | GPIO_PIN_2, //Pin 5 for azimuth, pin 2 for inclination. Change as needed
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &DirCtrl_InitStruct);
    */

    //Limit switch init. Assumes 2 limit switches
    GPIO_InitTypeDef LimitSwitch_InitStruct = {
        .Pin            = LIMIT_SWITCH_FILTER_INNER_PIN | LIMIT_SWITCH_FILTER_OUTER_PIN | LIMIT_SWITCH_ELEVATORBOTTOM_PIN | LIMIT_SWITCH_ELEVATORTOP_PIN,
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
        /*     Initialization of the uv and ht sensors    */
    init_ht(&ht_sensor, 5000); // Timeout of 5000 ms
    init_uv(&uv_sensor, &INT_TIME, 5000);
}

void motor_pwm_init(void)
{


    if (PWMLIB_Init(PWM_ELEVATOR_ID) != 0)
    {
        //Error_Handler_Motors();
    }
    if (PWMLIB_Init(PWM_DRILL_ID) != 0)
    {
        Error_Handler_Motors();
    }


}


/*  LIMIT SWITCH BIT VECTOR
    elevator retracted          = 0000 1000;
    elevator extended           = 0000 0100;
    sample selector outer limit = 0000 0010;
    sample selector inner limit = 0000 0001;
*/

void checkLimits()
{
    limit_switch_readings =     HAL_GPIO_ReadPin(LIMIT_SWITCH_FILTER_INNER_PORT,LIMIT_SWITCH_FILTER_INNER_PIN) |
                                HAL_GPIO_ReadPin(LIMIT_SWITCH_FILTER_OUTER_PORT,LIMIT_SWITCH_FILTER_OUTER_PIN) << 1 |
                                HAL_GPIO_ReadPin(LIMIT_SWITCH_ELEVATORTOP_PORT,LIMIT_SWITCH_ELEVATORTOP_PIN) << 2 |
                                HAL_GPIO_ReadPin(LIMIT_SWITCH_ELEVATORBOTTOM_PORT,LIMIT_SWITCH_ELEVATORBOTTOM_PIN) << 3;

    UART_LIB_PRINT_INT(limit_switch_readings);
}

void runDrillDistance(float drill_duty_cycle, float elevator_duty_cycle)
{
    int drill_dir = MOTOR_FWD_DIR;
    int elevator_dir = MOTOR_FWD_DIR;

    // Assuming 0 is down
    if ((drill_duty_cycle<0))
        drill_dir = MOTOR_RVR_DIR;
    if (elevator_duty_cycle<0)
        elevator_dir = MOTOR_RVR_DIR;

    checkLimits();
    if((elevator_duty_cycle<0) && (limit_switch_readings & 0x04))
    {
        PWMLIB_Write(PWM_ELEVATOR_ID,0.0);
        HAL_Delay(100);
        return;
    }

    if((elevator_duty_cycle>0) && (limit_switch_readings & 0x08))
    {
        PWMLIB_Write(PWM_ELEVATOR_ID,0.0);
        HAL_Delay(100);
        return;
    }

    HAL_GPIO_WritePin(GPIO_DRILL_DIR_PORT,GPIO_DRILL_DIR_PIN,drill_dir);
    HAL_GPIO_WritePin(GPIO_ELEVATOR_DIR_PORT,GPIO_ELEVATOR_DIR_PIN,elevator_dir);
    PWMLIB_Write(PWM_DRILL_ID,fabs(drill_duty_cycle));
    PWMLIB_Write(PWM_ELEVATOR_ID,fabs(elevator_duty_cycle));
    HAL_Delay(100);
}

void resetFilter(uint8_t filter_dir, float filter_duty_cycle)
{
    // drive motor
    uint8_t filter_zeroed = 0;
    //uint8_t filter_limits_reading[2] = {1};
    while(!filter_zeroed)
    {
        checkLimits();
        //limit_switch_readings
//        uint8_t result, lim1, lim2;
//        result =
//        result = lim1 << 1 | ;
//        1010
        HAL_GPIO_WritePin(GPIO_FILTER_DIR_PORT,GPIO_FILTER_DIR_PIN,filter_dir);
        PWMLIB_Write(PWM_FILTER_ID,filter_duty_cycle);
        HAL_Delay(100);
        //if (!filter_limits_reading[0])
        if(!(limit_switch_readings & 0x02))
        {
            do{
                checkLimits();
                PWMLIB_Write(PWM_FILTER_ID,filter_duty_cycle/2);
                HAL_Delay(50);
            } while((limit_switch_readings & 0x01));
            filter_zeroed = 1;
        }
    }
    bin_loc = 0;
}

void runFilterToBin(int bin_no, float filter_duty_cycle)
{
    //determine logic to go to certain filter
    uint8_t filter_dir = 1;
    if (filter_duty_cycle>0)
    {
        filter_dir = 0;
    }

    if(bin_no == -1)
    {
        resetFilter(bin_no, filter_duty_cycle);
    }

    //uint8_t filter_limits_reading = 1;
    while(bin_loc != bin_no)
    {
        HAL_GPIO_WritePin(GPIO_FILTER_DIR_PORT,GPIO_FILTER_DIR_PIN,filter_dir);
        PWMLIB_Write(PWM_FILTER_ID,fabs(filter_duty_cycle));
        HAL_Delay(100);
        //filter_limits_reading = HAL_GPIO_ReadPin(LIMIT_SWITCH_FILTER_INNER_PORT, LIMIT_SWITCH_FILTER_INNER_PIN);
        checkLimits();
        if(!(limit_switch_readings & 0x01))
        {
            bin_loc++;
            bin_loc = bin_loc%4;
        }
    }
}

void testMotor(float drill_duty_cycle)
{
    uint8_t drill_dir;
    if (drill_duty_cycle<0)
        drill_dir = 0;

    //uint8_t  = "Hello";
    //UART_LIB_PRINT_CHAR_ARRAY(a, sizeof(a));


    HAL_GPIO_WritePin(GPIO_DRILL_DIR_PORT,GPIO_DRILL_DIR_PIN,drill_dir);
    if (!(limit_switch_readings & 0x01))
    {
            PWMLIB_Write(PWM_DRILL_ID,0.0);
    }
    else
    {
        PWMLIB_Write(PWM_DRILL_ID,fabs(drill_duty_cycle));
    }
    HAL_Delay(100);
}

void sendCAN(int send_to_id)
{
    UART_LIB_INIT();
    //Here, we change the node ID to 10, put a 32 bit uint in the 4 LSBs of the
    // CAN data array (indicated by CANLIB_INDEX_0), and send the first 4 bytes.
    CANLIB_ChangeID(send_to_id);
    CANLIB_Tx_SetFloat(number1, CANLIB_INDEX_0);
    CANLIB_Tx_SetFloat(number2, CANLIB_INDEX_1);
    CANLIB_Tx_SendData(CANLIB_DLC_ALL_BYTES);
}

float getHumidity(int sensor_no)
{

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
    sensorInit();
    Timer_Init(PERIOD); // 500 ms timer


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

    UART_LIB_INIT();

    HAL_NVIC_SetPriority(TIM14_IRQn, 2, 2);

    /*
    while (1)
    {
        checkLimits();
        sendCAN(12);
        testMotor(incoming_msg[1]);
    }
    */

    /* EXAMPLE OF UART USE FOR TESTING
    UART_LIB_INIT();
    uint8_t a[] = "Hello";
    UART_LIB_PRINT_CHAR_ARRAY(a, sizeof(a));

    int b = 1234;
    UART_LIB_PRINT_INT(b);

    double c = 555.123;
    UART_LIB_PRINT_DOUBLE(c);
    */


    while(1)
    {
        checkLimits();
        switch(access_id)
        {
            case CAN_FILTERID_ELEVATOR_DRILL: ;//empty statement - C quirk >_>
                //getFiltertoBin
                float drill_duty_cycle = incoming_msg[0];
                float elevator_duty_cycle = incoming_msg[1];
                runDrillDistance(drill_duty_cycle,elevator_duty_cycle);
                break;

            case CAN_FILTERID_FILTER: ;
                //determine filter_no;
                int bin_no = (int)incoming_msg[0];
                float filter_duty_cycle = incoming_msg[1];
                runFilterToBin(bin_no,filter_duty_cycle);


            /* add cases for sensors */

                // determine drill_duty_cycle, elevator_duty_cycle_distance
            default:
                break;
        }
    }


    return 0;
}

void CANLIB_Rx_OnMessageReceived(void)
{
/*
    switch(CANLIB_Rx_GetSenderID())
    {
        // Expect frame format to be:
        // Bytes 0-3: Azimuth axis PWM input
        // Byte 4-7: Inclination axis PWM input

        case CAN_RX_ID:
            incoming_cmd[AZIMUTH_AXIS_ID] = CANLIB_Rx_GetAsFloat(AZIMUTH_AXIS_ID);
            incoming_cmd[INCLINATION_AXIS_ID] = CANLIB_Rx_GetAsFloat(INCLINATION_AXIS_ID);
            data_ready = 1;
            incoming_cmd[0] = CANLIB_Rx_GetAsFloat
            break;

        default:
            break;
    }
*/
    access_id = CANLIB_Rx_GetSenderID();
    incoming_msg[0] = CANLIB_Rx_GetAsFloat(0);
    incoming_msg[1] = CANLIB_Rx_GetAsFloat(1);



#if DEBUG_MODE

    UART_LIB_INIT();
    UART_LIB_PRINT_INT(access_id);
    UART_LIB_PRINT_DOUBLE(incoming_msg[0]);
    UART_LIB_PRINT_DOUBLE(incoming_msg[1]);

#endif

}
