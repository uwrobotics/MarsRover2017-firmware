/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  *
  *
  * This file exists to demonstrate basic use of the I2C library for Waterloo
  * Mars Rover. It tests basic I2C functinality by writing to/reading from some registers.
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include "i2clib.h"
#include "pins.h"

#define TRANSMITTER         1

#define LOOPBACK            0

#define TEST_PASSED 1
#define TEST_FAILED 2
#define LED_RED     GPIO_PIN_6
#define LED_BLUE    GPIO_PIN_7
#define LED_ORANGE  GPIO_PIN_8
#define LED_YELLOW  GPIO_PIN_9

I2C_Device_t MCP_temp_sensor;
I2C_Device_t arduino;

//Globals that we send
//  Any variables or arrays used for transmission can
//  be local as well
uint32_t node_ids[3] = {3, 4, 10};
uint8_t array[8] = {1,2,3,4,5,6,7,8};
uint32_t number1 = 32423432;

//Globals used for receiving values within the CAN callback function
uint8_t received_bytes[8];
uint32_t received_number;
uint32_t wrongly_received_number;

void setup_test();

void HAL_MspInit(void)
{
    /* System interrupt init*/
    /* SysTick_IRQn interrupt configuration */
    (void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    //Must be set to priority of 1 or else will have higher priority than CAN IRQ
    HAL_NVIC_SetPriority(SysTick_IRQn, 1, 1);
}



/* Test 1: test the I2C_send_data and I2C_receive_data calls (blocking)
 *
 *  Set MCP9808 lower temp threshold at address 0x03:
 *  Write 0x03-0x80-0x20 to MCP. It will store -2 degC as the thresh low temp
 *
 *  Read from low temp threshold in MCP:
 *  Write 0x03 to MCP. It will prepare to send the register back
 *  Immediately read 2 bytes from MCP: check that they match 0x80-0x20
 */
int test1(void)
{   //Write to the lower limit register using blocking call:
    // 
    uint8_t temp_thresh_set[3] = {0x03,0x01,0x20};
    uint8_t temp_thresh_read[2] = {0};
    uint8_t register_address = 0x03;

    //Write to the low temp threshold register
    I2C_send_data(&MCP_temp_sensor, temp_thresh_set, 3);

    //Write register address to sensor to start read request
    I2C_send_data(&MCP_temp_sensor, &register_address, 1);

    //Read the low temperature threshold, compare its values with those set
    I2C_receive_data(&MCP_temp_sensor, temp_thresh_read, 2);

    if (temp_thresh_read[0] == 0x01 && temp_thresh_read[1] == 0x20) {
        return TEST_PASSED;
    } else {
        return TEST_FAILED;
    }
}

int test2(void)
{
    uint8_t temp_thresh_set[2] = {0x05,0x90};//89 degC
    uint8_t temp_thresh_read[2] = {0};
    uint16_t temp_upper_thresh_reg_address = 0x02;

    I2C_mem_write(&MCP_temp_sensor, temp_upper_thresh_reg_address, temp_thresh_set, 2);

    I2C_mem_read(&MCP_temp_sensor, temp_upper_thresh_reg_address, temp_thresh_read, 2);

    if (temp_thresh_read[0] == 0x05 && temp_thresh_read[1] == 0x90) {
        return TEST_PASSED;
    } else {
        return TEST_FAILED;
    }
}

volatile int tx_done = 0;
volatile int rx_done = 0;

void I2C_Tx_Complete(I2C_Device_t *device) {
    if (device == &MCP_temp_sensor) {
        tx_done = 1;
    }
}

void I2C_Rx_Complete(I2C_Device_t *device) {
    if (device == &MCP_temp_sensor) {
        rx_done = 1;
    }
}

int test3(void)
{
    uint8_t temp_thresh_set[3] = {0x03,0x00,0x30}; //3 degC
    uint8_t temp_thresh_read[2] = {0};
    uint8_t register_address = 0x03;
    tx_done = 0;
    rx_done = 0;
    //Write to the low temp threshold register
    I2C_send_data_IT(&MCP_temp_sensor, temp_thresh_set, 3);

    while (!tx_done) {
        //Do nothing, make sure we get the interupt generated
        //In normal use we probably wouldn't care about waiting for this interupt unless it was
        //required to be complete for a subsequent step (in which case the blocking call may be easier)
    }

    //Write register address to sensor to start read request
    I2C_send_data(&MCP_temp_sensor, &register_address, 1); //This should be blocking sice its needed for the next step

    //Read the low temperature threshold, compare its values with those set
    I2C_receive_data_IT(&MCP_temp_sensor, temp_thresh_read, 2);

    while (!rx_done) {
        //Do nothing and wait for the received interupt. Code that can be executed before getting the sensor readings
        //could go here
    }

    tx_done = 0;
    rx_done = 0;
    if (temp_thresh_read[0] == 0x00 && temp_thresh_read[1] == 0x30) {
        return TEST_PASSED;
    } else {
        return TEST_FAILED;
    }
}

int test4(void)
{
    uint8_t temp_thresh_set[2] = {0x02,0xE0};//46 degC
    uint8_t temp_thresh_read[2] = {0};
    uint16_t temp_upper_thresh_reg_address = 0x02;
    tx_done = 0;
    rx_done = 0;
    //Write to the low temp threshold register
    I2C_mem_write_IT(&MCP_temp_sensor, temp_upper_thresh_reg_address, temp_thresh_set, 3);

    while (!tx_done) {
        //Do nothing, make sure we get the interupt generated
        //In normal use we probably wouldn't care about waiting for this interupt unless it was
        //required to be complete for a subsequent step (in which case the blocking call may be easier)
    }

    //Write register address to sensor to start read request
    //I2C_mem_write_IT(&MCP_temp_sensor, &register_address, 1); //This should be blocking sice its needed for the next step

    //Read the low temperature threshold, compare its values with those set
    I2C_mem_read_IT(&MCP_temp_sensor, temp_upper_thresh_reg_address, temp_thresh_read, 2);

    while (!rx_done) {
        //Do nothing and wait for the received interupt. Code that can be executed before getting the sensor readings
        //could go here
    }

    tx_done = 0;
    rx_done = 0;
    if (temp_thresh_read[0] == 0x02 && temp_thresh_read[1] == 0xE0) {
        return TEST_PASSED;
    } else {
        return TEST_FAILED;
    }
}

int main(void)
{
    //Always call. Enables prefetch and calls above function
    HAL_Init();

    setup_test();

    //LED sequence to make sure we are running
    HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOC, LED_ORANGE, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOC, LED_YELLOW, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, LED_ORANGE, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, LED_YELLOW, GPIO_PIN_RESET);
    HAL_Delay(1000);


    if (I2C_init(I2C1) == 0) {
        //If red light comes on, then init succeeded
        HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_SET);
        HAL_Delay(1000);
    } else {
        goto FAILED;
    }

    //Initialize slave devices
    if (I2C_slave_init(&MCP_temp_sensor, I2C1, 0x30,1000) != 0)
        goto FAILED;
    if (I2C_slave_init(&arduino, I2C1, 0x08,1000) != 0)
        goto FAILED;
    if (I2C_slave_mem_init(&MCP_temp_sensor, 8) != 0)
        goto FAILED;
    //If orange light comes on, then slave_init's succeeded
    HAL_GPIO_WritePin(GPIOC, LED_ORANGE, GPIO_PIN_SET);
    HAL_Delay(1000);


    if (test1() == TEST_PASSED) {
        //If blue light comes on, then test1 passed
        HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_SET);
        HAL_Delay(1000);
    } else {
        goto FAILED;
    }

    if (test2() == TEST_PASSED) {
        //If yellow light comes on, then test2 passed
        HAL_GPIO_WritePin(GPIOC, LED_YELLOW, GPIO_PIN_SET);
        HAL_Delay(1000);
    } else {
        goto FAILED;
    }


    if (test3() == TEST_PASSED) {
        //If red light turns off, then test1 passed
        HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_RESET);
        HAL_Delay(1000);
    } else {
        goto FAILED;
    }


    if (test3() == TEST_PASSED) {
        //If orange light turns off, then test1 passed
        HAL_GPIO_WritePin(GPIOC, LED_ORANGE, GPIO_PIN_RESET);
        HAL_Delay(1000);
    } else {
        goto FAILED;
    }

    HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, LED_ORANGE, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, LED_YELLOW, GPIO_PIN_RESET);


    char *test_str = "Hello\n";
    uint8_t mpc_temp_register = 0x05;
    uint8_t temperature[2] = {0,0};
    I2C_send_data(&arduino, (uint8_t *)test_str, 6);


    //Tests are completed. Let's read temperature readings from the MCP9808
    while(1)
    {
        HAL_StatusTypeDef ret = 6;

        //Write the desired register address (register 0x05 --> temperature register)
        ret = I2C_send_data(&MCP_temp_sensor,&mpc_temp_register,1);

        if (ret == HAL_OK){
            //read the temperature
            if (I2C_receive_data(&MCP_temp_sensor, temperature,2)== HAL_OK){
            HAL_Delay(100);

            //Send the received temperature to the arduino for serial printing, signal success with blue LED
            I2C_send_data(&arduino, temperature,2);
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_SET);
            HAL_Delay(2000);
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_RESET);
            HAL_Delay(2000);
           }
        }
        else {
            //Flashing lights means an error
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_SET);
            HAL_Delay(200);
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_RESET);
            HAL_Delay(200);
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_SET);
            HAL_Delay(200);
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_RESET);
            HAL_Delay(200);
        }

        //Lets to the same thing with I2C_mem_read

        ret = I2C_mem_read(&MCP_temp_sensor, mpc_temp_register, temperature, 2);
        if (ret == HAL_OK) {
            //Send the received temperature to the arduino for serial printing, signal success with yellow LED
            I2C_send_data(&arduino, temperature,2);
            HAL_GPIO_WritePin(GPIOC, LED_YELLOW, GPIO_PIN_SET);
            HAL_Delay(2000);
            HAL_GPIO_WritePin(GPIOC, LED_YELLOW, GPIO_PIN_RESET);
            HAL_Delay(2000);
        } else {
            //Flashing lights means an error
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_SET);
            HAL_Delay(200);
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_RESET);
            HAL_Delay(200);
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_SET);
            HAL_Delay(200);
            HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_RESET);
            HAL_Delay(200);
        }

    }

    return 0;

FAILED:
    while (1) {
        //Flashing lights means an error
        HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, LED_ORANGE, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, LED_YELLOW, GPIO_PIN_SET);
        HAL_Delay(200);
        HAL_GPIO_WritePin(GPIOC, LED_RED, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, LED_BLUE, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, LED_ORANGE, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, LED_YELLOW, GPIO_PIN_RESET);
        HAL_Delay(200);
    }
    return -1;
}

//This has NOTHING to do with the CAN library, but something similar is done in all
// All STM32Cube code has similar initialization: First the clocks, peripheral clocks
// then GPIOs and other things are initialized
// Here, we set up clocks and PC8 and PC9 as outputs for they are on LEDs that we want
// to flicker.
void setup_test(void)
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
    __SYSCFG_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {
            .Pin = LED_YELLOW | LED_ORANGE | LED_BLUE | LED_RED,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/*
//This is the CAN Rx function to implement
//Do anything you want in here, but it makes sense to do something similar to below:
// Switch on the node ID of who sends the message, and interpret
void CANLIB_Rx_OnMessageReceived(void)
{
    switch(CANLIB_Rx_GetSenderID())
    {
        case 3:
            CANLIB_Rx_GetBytes(received_bytes);
            break;
        case 4:
            break;
        case 10:
            received_number = CANLIB_Rx_GetAsUint(0);
            break;
        default:
            wrongly_received_number = CANLIB_Rx_GetAsUint(0);
            break;
    }

}*/
