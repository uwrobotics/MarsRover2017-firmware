/*
 * can_abstraction.c
 *
 *  Created on: Jul 6, 2016
 *      Author: ryan
 */

#include "can_properties.h"
#include <limits.h>
#include <string.h>


static GPIO_InitTypeDef GPIO_InitStruct;

static CAN_HandleTypeDef CAN_HandleStruct;

static return_struct received_message;

void CANAbstract__Init(uint32_t node_id){
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    if(!CANAbstract_GPIO_Init()){
    	//TODO: Error handling
    }

    if(!CANAbstract_CAN_NetworkInit()){
    	//TODO: Error handling
    }

    CANAbstract_CAN_NodeInit(node_id);

}

int CANAbstract_GPIO_Init(){

    GPIO_InitStruct.Pin = CAN_GPIO_RX_PIN | CAN_GPIO_TX_PIN;
    GPIO_InitStruct.Mode = CAN_GPIO_MODE;
    GPIO_InitStruct.Pull = CAN_GPIO_PULL;
    GPIO_InitStruct.Speed = CAN_GPIO_SPEED;
    GPIO_InitStruct.Alternate = CAN_GPIO_ALTERNATE;

    HAL_GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStruct);

    return 0;
}

int CANAbstract_CAN_NetworkInit(){

	CAN_HandleStruct.Instance = CAN_PORT;

	CAN_HandleStruct.Init.Prescaler = CAN_INIT_PRESCALER;
	CAN_HandleStruct.Init.ABOM = CAN_INIT_ABOM;
	CAN_HandleStruct.Init.AWUM = CAN_INIT_AWUM;
	CAN_HandleStruct.Init.BS1 = CAN_INIT_BS1;
	CAN_HandleStruct.Init.BS2 = CAN_INIT_BS2;
	CAN_HandleStruct.Init.Mode = CAN_INIT_MODE;
	CAN_HandleStruct.Init.NART = CAN_INIT_NART;
	CAN_HandleStruct.Init.RFLM = CAN_INIT_RFLM;
	CAN_HandleStruct.Init.SJW = CAN_INIT_SJW;
	CAN_HandleStruct.Init.TTCM = CAN_INIT_TTCM;
	CAN_HandleStruct.Init.TXFP = CAN_INIT_TXFP;

	CAN_HandleStruct.Lock = CAN_LOCK;

	HAL_CAN_Init(&CAN_HandleStruct);

	HAL_CAN_IRQHandler(&CAN_HandleStruct);

	HAL_NVIC_SetPriority(CEC_CAN_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);

	return 0;
}

void CANAbstract_CAN_NodeInit(uint32_t id){
	CAN_HandleStruct.pTxMsg->StdId = id;
	CAN_HandleStruct.pTxMsg->IDE = CAN_IDE_TYPE;
	CAN_HandleStruct.pTxMsg->RTR = CAN_RTR_TYPE;
}

void CEC_CAN_IRQHandler()
{
	HAL_CAN_IRQHandler(&CAN_HandleStruct);
	if(HAL_CAN_Receive_IT(&CAN_HandleStruct, CAN_FIFO0) == HAL_OK){
		CANAbstract_Rx_Decode();
	}

}

void CANAbstract_ClearDataArray(){
	//Reinitializes an integer array of size 8 to all 0s
	for(int i = 0; i<8; i++){
		CAN_HandleStruct.pTxMsg->Data[i] = 0;
	}
}

//--ENCODING FUNCTIONS

void CANAbstract_Transmit_String(char *string, uint8_t id){
	CANAbstract_ClearDataArray();

	CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_STRING;
	CAN_HandleStruct.pTxMsg->Data[1] = id;

	int length = strlen(string);

	if(length > 6){
		return;
	}

	for(int i = 0; i < length; i++){
		CAN_HandleStruct.pTxMsg->Data[i+2] = (uint8_t)string[i];
	}
}


void CANAbstract_Transmit_Uint(uint32_t message, uint8_t id){
	CANAbstract_ClearDataArray();

	encoding_union uint_union;
	uint_union.uinteger = message;

	CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_UINT32;
	CAN_HandleStruct.pTxMsg->Data[1] = id;
	CANAbstract_Tx_SetData(&uint_union);

	CANAbstract_Tx_SendShortData();

}

void CANAbstract_Transmit_Int(int32_t message, uint8_t id){
	CANAbstract_ClearDataArray();

	encoding_union int_union;
	int_union.uinteger = message;

	CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_INT32;
	CAN_HandleStruct.pTxMsg->Data[1] = id;
	CANAbstract_Tx_SetData(&int_union);

	CANAbstract_Tx_SendShortData();
}

void CANAbstract_Transmit_Float(float message, uint8_t id){
	CANAbstract_ClearDataArray();

	encoding_union float_union;
	float_union.floatingpt = message;

	CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_FLOAT;
	CAN_HandleStruct.pTxMsg->Data[1] = id;
	CANAbstract_Tx_SetData(&float_union);

	CANAbstract_Tx_SendShortData();
}


void CANAbstract_Transmit_Char(char message, uint8_t id){
	CANAbstract_ClearDataArray();
	CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_CHAR;
	CAN_HandleStruct.pTxMsg->Data[1] = id;
	CAN_HandleStruct.pTxMsg->Data[2] = (uint8_t)message;
	CAN_HandleStruct.pTxMsg->DLC = 0;
	HAL_CAN_Transmit_IT(&CAN_HandleStruct);
}


void CANAbstract_Rx_Decode(){
	received_message.return_type = CAN_HandleStruct.pRxMsg->Data[0];
	received_message.return_ID = CAN_HandleStruct.pRxMsg->Data[1];
	received_message.byte_array[0] = CAN_HandleStruct.pRxMsg->Data[2];
	received_message.byte_array[1] = CAN_HandleStruct.pRxMsg->Data[3];
	received_message.byte_array[2] = CAN_HandleStruct.pRxMsg->Data[4];
	received_message.byte_array[3] = CAN_HandleStruct.pRxMsg->Data[5];

	if (received_message.return_type == CAN_ABSTRACTION_TYPE_STRING){
		received_message.byte_array[4] = CAN_HandleStruct.pRxMsg->Data[6];
		received_message.byte_array[5] = CAN_HandleStruct.pRxMsg->Data[7];
	}
}

uint8_t CANAbstract_Rx_GetType(){
	return received_message.return_type;
}

char* CANAbstract_Rx_GetAsString(){
	static char string[7];

	for (int i = 0; i < 6; i++){
		string[i] = CAN_HandleStruct.pRxMsg->Data[i + 2];
		if (string[i] == 0){
			return string;
		}
	}

	string[6] = 0;
	return string;

}

uint32_t CANAbstract_Rx_GetAsUint(){
	return received_message.return_uint;
}

int32_t CANAbstract_Rx_GetAsInt(){
	return received_message.return_int;
}

char CANAbstract_Rx_GetAsChar(){
	return (char) received_message.byte_array[0];
}

float CANAbstract_Rx_GetAsFloat(){
	return received_message.return_float;
}
uint8_t CANAbstract_Rx_GetID(){
	return received_message.return_ID;
}
void CANAbstract_Tx_SetData(encoding_union* this_union){
	CAN_HandleStruct.pTxMsg->Data[2] = this_union->byte_array[0];
	CAN_HandleStruct.pTxMsg->Data[3] = this_union->byte_array[1];
	CAN_HandleStruct.pTxMsg->Data[4] = this_union->byte_array[2];
	CAN_HandleStruct.pTxMsg->Data[5] = this_union->byte_array[3];
}

void CANAbstract_Tx_SendShortData(){
	CAN_HandleStruct.pTxMsg->DLC = 6;

	HAL_CAN_Transmit_IT(&CAN_HandleStruct);
}


