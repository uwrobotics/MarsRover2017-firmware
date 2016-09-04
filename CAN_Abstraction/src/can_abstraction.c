/*
 * can_abstraction.c
 *
 *  Created on: Jul 6, 2016
 *      Author: ryan
 */

#include "can_properties.h"
#include <limits.h>
#include <string.h>


GPIO_InitTypeDef GPIO_InitStruct;
CAN_HandleTypeDef CAN_HandleStruct;

void CANAbstract__Init(){
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    if(CANAbstract_GPIO_Init()){
    	//TODO: Error handling
    }
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

void CANAbstract_CAN_NetworkInit(){

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
}

void CANAbstract_CAN_NodeInit(uint32_t Id){
	CAN_HandleStruct.pTxMsg->StdId = Id;
	CAN_HandleStruct.pTxMsg->IDE = CAN_IDE_TYPE;
	CAN_HandleStruct.pTxMsg->RTR = CAN_RTR_TYPE;
}

void CANAbstract_ClearDataArray(){
	//Reinitializes an integer array of size 8 to all 0s
	for(int i = 0; i<8; i++){
		CAN_HandleStruct.pTxMsg->Data[i] = 0;
	}
}

void Internal_CANAbstract_Transmit_WholeNumber(uint64_t message, sign_enum sign){

	CANAbstract_ClearDataArray();

	CAN_HandleStruct.pTxMsg->DLC = 0;

	CAN_HandleStruct.pTxMsg->Data[1] = message & 225;
	CAN_HandleStruct.pTxMsg->Data[2] = (message >> 8) & 225;
	CAN_HandleStruct.pTxMsg->Data[3] = (message >> 16) & 225;
	CAN_HandleStruct.pTxMsg->Data[4] = (message >> 24) & 225;
	if (message <= ULONG_MAX){

		CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_UINT32 | sign;
		HAL_CAN_Transmit_IT(&CAN_HandleStruct);

	} else {

		CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_UINT64_LSB | sign;
		HAL_CAN_Transmit_IT(&CAN_HandleStruct);

		CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_UINT64_MSB | sign;
		CAN_HandleStruct.pTxMsg->Data[1] = message & 225;
		CAN_HandleStruct.pTxMsg->Data[2] = (message >> 32) & 225;
		CAN_HandleStruct.pTxMsg->Data[3] = (message >> 40) & 225;
		CAN_HandleStruct.pTxMsg->Data[4] = (message >> 48) & 225;

		HAL_CAN_Transmit_IT(&CAN_HandleStruct);

	}
}

void CANAbstract_Transmit_Uint(uint64_t message){
	Internal_CANAbstract_Transmit_WholeNumber(message, POSITIVE);
}

void CANAbstract_Transmit_Int(int64_t message){
	CANAbstract_ClearDataArray();

	if (message < 0){
		Internal_CANAbstract_Transmit_WholeNumber((uint64_t)(-message), NEGATIVE);
	} else {
		Internal_CANAbstract_Transmit_WholeNumber((uint64_t)(message), POSITIVE);
	}
}

void CANAbstract_Transmit_Float(float message){
	//Done according to floating point standards:
	//	Data[1] has the sign bit
	//  Data[2] has the exponent
	//  Data[3-5] has the fraction from LSBs to MSBs
	CANAbstract_ClearDataArray();

	float_union this_float_union;
	this_float_union.input = message;

	CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_FLOAT;
	CAN_HandleStruct.pTxMsg->Data[1] = (this_float_union.result >> 31) & 1;
	CAN_HandleStruct.pTxMsg->Data[2] = (this_float_union.result >> 23) & 255;
	CAN_HandleStruct.pTxMsg->Data[3] = this_float_union.result & 255;
	CAN_HandleStruct.pTxMsg->Data[4] = (this_float_union.result >> 8) & 255;
	CAN_HandleStruct.pTxMsg->Data[5] = (this_float_union.result >> 16) & 127;

	CAN_HandleStruct.pTxMsg->DLC = 0;

	HAL_CAN_Transmit_IT(&CAN_HandleStruct);
}


void CANAbstract_Transmit_Char(char message){
	CANAbstract_ClearDataArray();
	CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_CHAR;
	CAN_HandleStruct.pTxMsg->Data[1] = (uint8_t)message;
	CAN_HandleStruct.pTxMsg->DLC = 0;
	HAL_CAN_Transmit_IT(&CAN_HandleStruct);
}

void CANAbstract_Transmit_String(char message[]){
	CANAbstract_ClearDataArray();

	uint32_t message_length = strlen(message);
	if (message_length <= 7){
		CAN_HandleStruct.pTxMsg->Data[0] = CAN_ABSTRACTION_TYPE_SHORT_STRING;
		for(int index = 1; index <= message_length; index++){
			CAN_HandleStruct.pTxMsg->Data[index] = (uint8_t)message[index-1];
		}

		CAN_HandleStruct.pTxMsg->DLC = 0;
		HAL_CAN_Transmit_IT(&CAN_HandleStruct);

	}
}

return_struct CANAbstract_Receive_Decode(){
	return_struct decoded_message;

	decoded_message.return_type = CAN_HandleStruct.pRxMsg->Data[0];
	switch (decoded_message.return_type){
		case CAN_ABSTRACTION_TYPE_UINT32:
			break;
		case CAN_ABSTRACTION_TYPE_UINT64_LSB:
			break;
		case CAN_ABSTRACTION_TYPE_UINT64_MSB:
			break;
		case CAN_ABSTRACTION_TYPE_FLOAT:
			break;
		case CAN_ABSTRACTION_TYPE_DOUBLE:
			break;
		case CAN_ABSTRACTION_TYPE_CHAR:
			break;
		case CAN_ABSTRACTION_TYPE_SHORT_STRING:
			break;
		case CAN_ABSTRACTION_TYPE_LONG_STRING:
			break;
		default:
			break;
	}

	return decoded_message;
}



