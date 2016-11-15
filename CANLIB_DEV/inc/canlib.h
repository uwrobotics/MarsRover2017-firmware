/*
 * canlib.h
 *
 *  Created on: Jul 6, 2016
 *      Author: ryan
 */

#ifndef CANLIB_H_
#define CANLIB_H_

#include "stm32f0xx.h"
//#include <string.h>

//CONSTANTS FOR CAN BUS CONFIGURATION. NEED NOT MODIFY
#define CAN_GPIO_RX_PIN			GPIO_PIN_11
#define CAN_GPIO_TX_PIN			GPIO_PIN_12
#define CAN_GPIO_PORT			GPIOA
#define CAN_GPIO_MODE			GPIO_MODE_AF_PP
#define CAN_GPIO_PULL			GPIO_NOPULL
#define CAN_GPIO_SPEED			GPIO_SPEED_LOW
#define CAN_GPIO_ALTERNATE		GPIO_AF4_CAN
#define CAN_PORT				CAN
#define CAN_INIT_PRESCALER		6
#define CAN_INIT_MODE			CAN_MODE_LOOPBACK
#define CAN_INIT_SJW			CAN_SJW_1TQ
#define CAN_INIT_BS1			CAN_BS1_12TQ
#define CAN_INIT_BS2			CAN_BS2_3TQ
#define CAN_INIT_TTCM			DISABLE
#define CAN_INIT_ABOM			DISABLE
#define CAN_INIT_AWUM			DISABLE
#define CAN_INIT_NART			DISABLE
#define CAN_INIT_RFLM			DISABLE
#define CAN_INIT_TXFP			DISABLE
#define CAN_INIT_FIFO			CAN_FIFO0
#define CAN_LOCK				HAL_UNLOCKED
#define CAN_IDE_TYPE			CAN_ID_STD
#define CAN_RTR_TYPE			CAN_RTR_DATA

/* Union structures used for encoding and decoding various types and byte arrays
 * The code is compiled for little endian processors so this is fine
 */
typedef union{
	uint8_t byte_array[8];
	int64_t long_integer;
	uint64_t long_uinteger;
	double dub;
	int32_t integer;
	uint32_t uinteger;
	float floatingpt;
} encoding_union;

typedef struct{
	uint8_t DLC;
	uint32_t transmitter_ID;
	union{
		uint64_t long_uint;
		int64_t long_int;
		double dub;
		uint8_t whole_byte_array[8];
		struct{
			union{
				uint8_t byte_array_1[4];
				int32_t return_int_1;
				uint32_t return_uint_1;
				float return_float_1;
				char return_chars_1[4];
			};
			union{
				uint8_t byte_array_2[4];
				int32_t return_int_2;
				uint32_t return_uint_2;
				float return_float_2;
				char return_chars_2[4];
			};
		};
	};
} return_struct;

//Initialization/Handling function
int CANLIB_Init(uint32_t node_ID);
void CANLIB_ChangeID(uint32_t node_ID);
int CANLIB_AddFilter(uint16_t id);

//Tx Functions
void CANLIB_Tx_SendData(uint8_t dlc);

void CANLIB_Tx_SetDataWord(encoding_union* this_union, uint8_t offset);

void CANLIB_Tx_SetByte(uint8_t byte, uint8_t index);
void CANLIB_Tx_SetBytes(uint8_t* byte_array, uint8_t array_size);

void CANLIB_Tx_SetChar(char c, uint8_t index);
void CANLIB_Tx_SetChars(char *string, uint8_t char_count);

void CANLIB_Tx_SetUint(uint32_t message, uint8_t index);
void CANLIB_Tx_SetInt(int32_t message, uint8_t index);
void CANLIB_Tx_SetFloat(float message, uint8_t index);
void CANLIB_Tx_SetDouble(double message, uint8_t index);
void CANLIB_Tx_SetLongUint(uint64_t message, uint8_t index);
void CANLIB_Tx_SetLongInt(int64_t message, uint8_t index);

//Rx Functions

//OnMessageReceived() is called when message is received
//	Must be user implemented
__weak void CANLIB_Rx_OnMessageReceived();
void CANLIB_Rx_Decode();
uint8_t CANLIB_Rx_GetSenderID();
uint8_t CANLIB_Rx_GetDLC();
uint8_t CANLIB_Rx_GetSingleByte(uint8_t byte_index);
uint8_t CANLIB_Rx_GetSingleChar(uint8_t index);
void CANLIB_Rx_GetBytes(uint8_t* byte_array);
void CANLIB_Rx_GetChars(char* char_array);
uint32_t CANLIB_Rx_GetAsUint(uint8_t uint_num);
int32_t CANLIB_Rx_GetAsInt(uint8_t int_num);
float CANLIB_Rx_GetAsFloat(uint8_t float_num);
int64_t CANLIB_Rx_GetAsLongInt();
uint64_t CANLIB_Rx_GetAsLongUint();
double CANLIB_Rx_GetAsDouble();

//Convenience functions
uint8_t CANLIB_SendBytes(uint8_t* byte_array, uint8_t array_size, uint32_t id);

#endif
