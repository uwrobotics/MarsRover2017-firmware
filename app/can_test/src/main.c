/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  *
  *
  * This file exists to demonstrate basic use of the CAN library for Waterloo
  * Mars Rover. It's a very contrived and simple 'blink' example, basically.
  * This is meant to run in loopback mode (on one dev board).
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include "canlib.h"
#include "pins.h"

#define TRANSMITTER         1

#define LOOPBACK            0

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

void CAN_Init(uint32_t id)
{
    // uint32_t i;

    /* First relevant thing: Initialize CAN communication handler
     * The input parameter to CANLIB_Init() is the node ID you want this node to have.
     * In this case, we start with a node ID of 1
     * Second parameter means no loopback mode
     * This demo was developed with 1 board, so loopback mode is ON
     */
    switch(CANLIB_Init(id, LOOPBACK))
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

#if (!TRANSMITTER && !LOOPBACK) || LOOPBACK
    //Using CANLIB_AddFilter() will allow you to specify who this node can receive messages from
    // according to the sender node ID. This ensures less CAN interrupts
    //Without calling this function, no messages will be received
    // for (i = 0; i < (sizeof(node_ids)/sizeof(uint32_t)); i++)
    // {
    //     CANLIB_AddFilter(i);
    // }
    CANLIB_AddFilter(3);
    CANLIB_AddFilter(4);
    CANLIB_AddFilter(10);
#endif
}

int test1(void)
{
#if TRANSMITTER
    //Send the byte array from above, return if unsuccessful
    //Notice that all 8 bytes of the byte array are being sent, and
    //the node ID of this node is changed to 3 before this message is sent
    if(CANLIB_SendBytes(array, CANLIB_DLC_ALL_BYTES, 3))
    {
        return -1;
    }

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_Delay(1000);
#endif
#if (!LOOPBACK && !TRANSMITTER) || LOOPBACK
    //If the bits received are what we sent, light the green LED on PC9
    // else, light the red LED on PC8
    for(int i = 0; i < 8; i++)
    {
        if (received_bytes[i] == i+1)
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
            received_bytes[i] = 0;
        }
        else
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			break;
        }
    }

    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_Delay(1000);
#endif
    return 0;
}

void test2(void)
{
#if TRANSMITTER
    //Here, we change the node ID to 10, put a 32 bit uint in the 4 LSBs of the
    // CAN data array (indicated by CANLIB_INDEX_0), and send the first 4 bytes.
    CANLIB_ChangeID(10);
    CANLIB_Tx_SetUint(number1, CANLIB_INDEX_0);
    CANLIB_Tx_SendData(CANLIB_DLC_FOUR_BYTES);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);

    HAL_Delay(5000);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
#endif
#if (!LOOPBACK && !TRANSMITTER) || LOOPBACK
    //If the number received from the callback is the same as the one sent,
    // turn on the green LED.
    // Else, turn on the red LED.
    if (received_number == number1)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    }

    HAL_Delay(5000);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_Delay(2000);
#endif
}

void test3(void)
{
#if TRANSMITTER
    //Change the ID to 5 and send the SAME 4 BYTES
    //This is because the CAN data array is PERSISTENT, so number1 is sent again,
    // just with a different node ID
    CANLIB_ChangeID(5);
    CANLIB_Tx_SendData(4);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);

    HAL_Delay(5000);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
#endif
#if (!LOOPBACK && !TRANSMITTER) || LOOPBACK
    //The green LED will turn on if the number received in the default case is equal to
    // the number sent. This WOULD run if the message above triggered an Rx interrupt, as
    // there is no case to receive messages from a node with ID of 5. However, the
    // interrupt never fires. This is the expected behaviour, because we never set up a filter
    // to receive messages from nodes with ID of 5!
    // Therefore, the red LED will turn on
    if (wrongly_received_number == number1)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    }

    HAL_Delay(5000);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_Delay(2000);
#endif
}

int main(void)
{
    //Always call. Enables prefetch and calls above function
    HAL_Init();

    setup_test();

#if TRANSMITTER
    CAN_Init(1);
#else
    CAN_Init(2);
#endif

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

    HAL_Delay(1000);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

    while(1)
    {
        // If a test returns -1, the HAL has returned HAL_ERROR
        if (test1() == -1)
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
            HAL_Delay(2000);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        }
    }

    return 0;
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
            .Pin = GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

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
}
