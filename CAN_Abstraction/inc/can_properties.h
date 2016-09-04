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

#define CAN_ABSTRACTION_TYPE_INT8			(uint8_t)0
#define CAN_ABSTRACTION_TYPE_UINT32 		(uint8_t)1
#define CAN_ABSTRACTION_TYPE_UINT64_LSB		(uint8_t)2
#define CAN_ABSTRACTION_TYPE_UINT64_MSB		(uint8_t)3
#define CAN_ABSTRACTION_TYPE_FLOAT			(uint8_t)4
#define CAN_ABSTRACTION_TYPE_DOUBLE			(uint8_t)5
#define CAN_ABSTRACTION_TYPE_CHAR  			(uint8_t)10
#define CAN_ABSTRACTION_TYPE_SHORT_STRING	(uint8_t)11
#define CAN_ABSTRACTION_TYPE_LONG_STRING	(uint8_t)12

#define CAN_ABSTRACTION_NEGATIVE				(uint8_t)16

typedef enum{
	POSITIVE=0,
	NEGATIVE = 16
} sign_enum;

typedef union{
	uint32_t result;
	float input;
} float_union;

typedef struct{
	uint8_t return_type;
	union{
		int64_t return_int;
		uint64_t return_uint;
		char return_char;
		float return_float;
		char** return_string_ptr;
	};
} return_struct;

void CANAbstract_Transmit_Uint(uint64_t message);

#endif
