/*********************************************************
     __  ___                   ____
    /  |/  /___ ___________   / __ \____ _   _____  _____
   / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
  / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
 /_/  /_/\__,_/_/  /____/  /_/ |_|\____/|___/\___/_/ 

 Copyright 2017, UW Robotics Team

 @file     uart_lib.c
 @author:  Jerry Li

**********************************************************/
#include "stm32f0xx.h"
#include "uart_lib.h"
#include <stdlib.h>

UART_HandleTypeDef huart1;

void HAL_UART_MspInit(UART_HandleTypeDef *huart){
	GPIO_InitTypeDef UART_GPIO_InitStruct;

	__GPIOA_CLK_ENABLE();
    UART_GPIO_InitStruct.Pin = GPIO_PIN_9;
    UART_GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    UART_GPIO_InitStruct.Pull = GPIO_PULLUP;
    UART_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    UART_GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &UART_GPIO_InitStruct);

    __HAL_RCC_USART1_CLK_ENABLE();
    huart->Init.BaudRate = 9600;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	huart->Init.StopBits = UART_STOPBITS_1;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.Mode = UART_MODE_TX;
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.OverSampling = UART_OVERSAMPLING_8;
	huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
}

// return code -1: error
//              0: no error
int UART_LIB_INIT(void)
{
    huart1.Instance = USART1;
    if (HAL_UART_Init(&huart1) == HAL_OK) {
        return 0; 
    }
    return -1;
} 


int UART_LIB_PRINT_CHAR_ARRAY(uint8_t* data, uint16_t len)
{
    if (HAL_UART_Transmit(&huart1, data, len, 0xFF) == HAL_OK) {
        return 0;
    }
    return -1;
}

int UART_LIB_PRINT_INT(int data)
{
    char temp[1];
    uint16_t len = snprintf(temp, 1, "%d", data) + 1;
    uint8_t* buf = malloc(len);
    if (buf) {
        sprintf((char*)buf, "%d", data);
        if (HAL_UART_Transmit(&huart1, buf, len, 0xFF) == HAL_OK) {
            free(buf);
            return 0;
        }
    }
    free(buf);
    return -1;
}

int UART_LIB_PRINT_DOUBLE(double data)
{
    char temp[1];
    uint16_t len = snprintf(temp, 1, "%f", data) + 1;
    uint8_t* buf = malloc(len);
    if (buf) {
        sprintf((char*)buf, "%f", data);
        if (HAL_UART_Transmit(&huart1, buf, len, 0xFF) == HAL_OK){
            free(buf);
            return 0;
        }
    }
    free(buf);
    return -1;
}

