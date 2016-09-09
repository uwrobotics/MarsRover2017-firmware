/*
 * can_properties.h
 *
 *  Created on: Jul 6, 2016
 *      Author: ryan
 */

#ifndef CAN_PROPERTIES_H_
#define CAN_PROPERTIES_H_

#include "stm32f0xx.h"
#include <string.h>

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
#define CAN_INIT_FIFO			CAN_FIFO0
#define CAN_LOCK				HAL_UNLOCKED
#define CAN_IDE_TYPE			CAN_ID_STD
#define CAN_RTR_TYPE			CAN_RTR_DATA

#define CAN_ABSTRACTION_TYPE_INT8			(uint8_t)0
#define CAN_ABSTRACTION_TYPE_UINT32 		(uint8_t)1
#define CAN_ABSTRACTION_TYPE_INT32 			(uint8_t)2
#define CAN_ABSTRACTION_TYPE_FLOAT			(uint8_t)3
#define CAN_ABSTRACTION_TYPE_CHAR  			(uint8_t)10
#define CAN_ABSTRACTION_TYPE_STRING			(uint8_t)11

typedef union{
	uint8_t byte_array[4];
	int32_t integer;
	uint32_t uinteger;
	float floatingpt;
} encoding_union;

typedef struct{
	uint8_t return_type;
	uint8_t return_ID;
	union{
		uint8_t byte_array[6];
		int32_t return_int;
		uint32_t return_uint;
		float return_float;
		char return_string[6];
	};
} return_struct;

int CANAbstract_Init(uint32_t node_ID);

void CANAbstract_Rx_Decode(void);
int CANAbstract_CAN_NetworkInit(void);
int CANAbstract_CAN_NodeInit(uint32_t node_ID);
void CANAbstract_Transmit_Uint(uint32_t message, uint8_t id);
int CANAbstract_GPIO_Init(void);
void CANAbstract_Tx_SetData(encoding_union* this_union);

void CANAbstract_Tx_SendShortData(void);

#endif
