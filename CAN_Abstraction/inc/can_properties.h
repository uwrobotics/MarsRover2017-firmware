/*
 * can_properties.h
 *
 *  Created on: Jul 6, 2016
 *      Author: ryan
 */

#ifndef CAN_PROPERTIES_H_
#define CAN_PROPERTIES_H_

#include "stm32f0xx.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_can.h"


//CONSTANTS FOR CAN BUS CONFIGURATION
#define CAN_GPIO_RX_PIN			GPIO_PIN_11
#define CAN_GPIO_TX_PIN			GPIO_PIN_12
#define CAN_GPIO_PORT			GPIOA
#define CAN_GPIO_MODE			GPIO_MODE_AF_PP
#define CAN_GPIO_PULL			GPIO_NOPULL
#define CAN_GPIO_SPEED			GPIO_SPEED_HIGH
#define CAN_GPIO_ALTERNATE		GPIO_AF4_CAN
#define CAN_PORT				CAN
#define CAN_INIT_PRESCALER		1024
#define CAN_INIT_MODE			CAN_MODE_NORMAL
#define CAN_INIT_SJW			CAN_SJW_1TQ
#define CAN_INIT_BS1			CAN_BS1_1TQ
#define CAN_INIT_BS2			CAN_BS2_1TQ
#define CAN_INIT_TTCM			DISABLE
#define CAN_INIT_ABOM			DISABLE
#define CAN_INIT_AWUM			ENABLE
#define CAN_INIT_NART			DISABLE
#define CAN_INIT_RFLM			DISABLE
#define CAN_INIT_TXFP			DISABLE
#define CAN_LOCK				HAL_UNLOCKED
#define CAN_IDE_TYPE			CAN_ID_STD
#define CAN_RTR_TYPE			CAN_RTR_DATA

#define CAN_ABSTRACTION_PRIMITIVE_INT8			(uint8_t)0
#define CAN_ABSTRACTION_PRIMITIVE_INT32 		(uint8_t)1
#define CAN_ABSTRACTION_PRIMITIVE_INT64_LSB		(uint8_t)2
#define CAN_ABSTRACTION_PRIMITIVE_INT64_MSB		(uint8_t)3
#define CAN_ABSTRACTION_PRIMITIVE_CHAR  		(uint8_t)5

#define CAN_ABSTRACTION_UNSIGNED	(uint8_t)0
#define CAN_ABSTRACTION_SIGNED		(uint8_t)1

void CANAbstract_Transmit_Uint(uint32_t message);

#endif
