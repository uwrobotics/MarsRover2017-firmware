/*
 * canlib.c
 *
 *  Created on: Jul 6, 2016
 *      Author: ryan
 */

#include "canlib.h"
#include <limits.h>
#include <string.h>

GPIO_InitTypeDef GPIO_InitStruct;

CAN_HandleTypeDef CAN_HandleStruct;
CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;

return_struct received_message;
uint8_t filterCount = 0;

//Function not part of API so declared in here for internal use
void        CANLIB_Rx_Decode();

void CEC_CAN_IRQHandler()
{
	HAL_CAN_IRQHandler(&CAN_HandleStruct);
}

uint32_t error = HAL_CAN_ERROR_NONE;
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	error = hcan->ErrorCode;

	__HAL_CAN_CANCEL_TRANSMIT(hcan, CAN_TXMAILBOX_0);
	__HAL_CAN_CANCEL_TRANSMIT(hcan, CAN_TXMAILBOX_1);
	__HAL_CAN_CANCEL_TRANSMIT(hcan, CAN_TXMAILBOX_2);
	hcan->Instance->MSR |= CAN_MCR_RESET;
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan){

	__HAL_RCC_CAN1_CLK_ENABLE();

	__HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = CAN_GPIO_RX_PIN | CAN_GPIO_TX_PIN;
    GPIO_InitStruct.Mode = CAN_GPIO_MODE;
    GPIO_InitStruct.Pull = CAN_GPIO_PULL;
    GPIO_InitStruct.Speed = CAN_GPIO_SPEED;
    GPIO_InitStruct.Alternate = CAN_GPIO_ALTERNATE;

    HAL_GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(CEC_CAN_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);

}

int CANLIB_NetworkInit(uint8_t isLoopbackOn){

	CAN_HandleStruct.Instance = CAN_PORT;

	CAN_HandleStruct.Init.Prescaler = CAN_INIT_PRESCALER;
	CAN_HandleStruct.Init.ABOM = CAN_INIT_ABOM;
	CAN_HandleStruct.Init.AWUM = CAN_INIT_AWUM;
	CAN_HandleStruct.Init.BS1 = CAN_INIT_BS1;
	CAN_HandleStruct.Init.BS2 = CAN_INIT_BS2;
	if (isLoopbackOn){
		CAN_HandleStruct.Init.Mode = CAN_MODE_LOOPBACK;
	} else {
		CAN_HandleStruct.Init.Mode = CAN_INIT_MODE;
	}
	CAN_HandleStruct.Init.NART = CAN_INIT_NART;
	CAN_HandleStruct.Init.RFLM = CAN_INIT_RFLM;
	CAN_HandleStruct.Init.SJW = CAN_INIT_SJW;
	CAN_HandleStruct.Init.TTCM = CAN_INIT_TTCM;
	CAN_HandleStruct.Init.TXFP = CAN_INIT_TXFP;

	CAN_HandleStruct.pRxMsg = &RxMessage;
	CAN_HandleStruct.pTxMsg = &TxMessage;


	if (HAL_CAN_Init(&CAN_HandleStruct) != HAL_OK){
		return -1;
	}

	//This following code will allow all CAN messages to pass through the filter
	/*CAN_FilterConfTypeDef sFilterConfig;
	sFilterConfig.FilterNumber = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = 0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 0x2d;

	if (HAL_CAN_ConfigFilter(&CAN_HandleStruct, &sFilterConfig) != HAL_OK){
		return -2;
	} */


	if(HAL_CAN_Receive_IT(&CAN_HandleStruct, CAN_INIT_FIFO) != HAL_OK){
		return -2;
	}

	return 0;
}

void CANLIB_NodeInit(uint32_t id){
	CAN_HandleStruct.pTxMsg->StdId = id;
	CAN_HandleStruct.pTxMsg->IDE = CAN_IDE_TYPE;
	CAN_HandleStruct.pTxMsg->RTR = CAN_RTR_TYPE;
}

void CANLIB_ChangeID(uint32_t node_ID){
	CAN_HandleStruct.pTxMsg->StdId = node_ID;
}

int CANLIB_AddFilter(uint16_t id){

	if(filterCount == 28){
		return 1;
	}

	static CAN_FilterConfTypeDef sFilterConfig;
	sFilterConfig.FilterNumber = filterCount;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = id << 5;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = ~(id << 5);
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = 0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.BankNumber = 0x2d;

	if (HAL_CAN_ConfigFilter(&CAN_HandleStruct, &sFilterConfig) != HAL_OK){
		return 2;
	}

	filterCount ++;
	return 0;
}

int CANLIB_SetupFilters(uint32_t size, uint32_t* node_ids){
    int ret = 0;
    //Declare an array of multi_frame structs of length size
    multi_frame* dev_msgs = (mutli_frame*)malloc( sizeof(multi_frame) * size );
    
    if(size > 28){
        size = 28;
    }

    for(int i = 0; i < size, i++){
        //Initialize the multi_frame structs


        if( (ret = CANLIB_AddFilter( node_ids[i] )) == 2){
            return ret;
        }
    }

    return ret;
}

int CANLIB_Init(uint32_t node_id, uint8_t isLoopbackOn){

    if(CANLIB_NetworkInit(isLoopbackOn) != 0){
        return -1;
    }

    CANLIB_NodeInit(node_id);

    return 0;
}

void CANLIB_ClearDataArray(){
	//Reinitializes an integer array of size 8 to all 0s
	for(int i = 0; i<8; i++){
		CAN_HandleStruct.pTxMsg->Data[i] = 0;
	}
}

//--Message Encoding Functions


void CANLIB_Tx_SendData(uint8_t dlc){
	CAN_HandleStruct.pTxMsg->DLC = (uint32_t) dlc;

	HAL_CAN_Transmit_IT(&CAN_HandleStruct);
}

void CANLIB_Tx_SetDataWord(encoding_union* this_union, uint8_t offset){
	CAN_HandleStruct.pTxMsg->Data[0 + offset] = this_union->byte_array[0];
	CAN_HandleStruct.pTxMsg->Data[1 + offset] = this_union->byte_array[1];
	CAN_HandleStruct.pTxMsg->Data[2 + offset] = this_union->byte_array[2];
	CAN_HandleStruct.pTxMsg->Data[3 + offset] = this_union->byte_array[3];
}

void CANLIB_Tx_SetByte(uint8_t byte, uint8_t index){
	CAN_HandleStruct.pTxMsg->Data[index] = byte;
}

void CANLIB_Tx_SetBytes(uint8_t* byte_array, uint8_t array_size){
	for (uint8_t byte_index = 0; byte_index < array_size; byte_index ++){
		CAN_HandleStruct.pTxMsg->Data[byte_index] = byte_array[byte_index];
	}
}

void CANLIB_Tx_SetChars(char *string, uint8_t char_count){
	CANLIB_Tx_SetBytes((uint8_t*) string, char_count);
}

void CANLIB_Tx_SetUint(uint32_t message, uint8_t index){
	encoding_union uint_union;
	uint_union.uinteger = message;

	CANLIB_Tx_SetDataWord(&uint_union, index * 4);

}

void CANLIB_Tx_SetInt(int32_t message, uint8_t index){
	encoding_union int_union;
	int_union.uinteger = message;

	CANLIB_Tx_SetDataWord(&int_union, index * 4);
}

void CANLIB_Tx_SetFloat(float message, uint8_t index){
	encoding_union float_union;
	float_union.floatingpt = message;

	CANLIB_Tx_SetDataWord(&float_union, index * 4);
}

void CANLIB_Tx_SetChar(char c, uint8_t index){
	CAN_HandleStruct.pTxMsg->Data[index] = (uint8_t)c;
}

void CANLIB_Tx_SetDouble(double message){
	encoding_union double_union;
	double_union.dub = message;

	CANLIB_Tx_SetBytes(double_union.byte_array, 8);
}

void CANLIB_Tx_SetLongUint(uint64_t message){
	encoding_union uint_union;
	uint_union.long_uinteger = message;

	CANLIB_Tx_SetBytes(uint_union.byte_array, 8);
}

void CANLIB_Tx_SetLongInt(int64_t message){
	encoding_union int_union;
	int_union.long_integer = message;

	CANLIB_Tx_SetBytes(int_union.byte_array, 8);
}

//Receiving and retreival functions

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan){
	if(HAL_CAN_Receive_IT(&CAN_HandleStruct, CAN_FIFO0) == HAL_OK){
		CANLIB_Rx_Decode();
		CANLIB_Rx_OnMessageReceived();
	}
}

//Purpose of this is to take the received data array and put it into our own data structure.
// Can be later expanded such that we can maintain a queue of these messages in the event they come too quickly for the user code
void CANLIB_Rx_Decode(){
	received_message.DLC = (uint8_t) CAN_HandleStruct.pRxMsg->DLC;
	received_message.transmitter_ID = CAN_HandleStruct.pRxMsg->StdId;

	for(uint8_t rxMsgIndex = 0; rxMsgIndex < received_message.DLC; rxMsgIndex ++){
		received_message.whole_byte_array[rxMsgIndex] = CAN_HandleStruct.pRxMsg->Data[rxMsgIndex];
	}

}

uint8_t CANLIB_Rx_GetSenderID(){
	return received_message.transmitter_ID;
}

uint8_t CANLIB_Rx_GetDLC(){
	return received_message.DLC;
}

uint8_t CANLIB_Rx_GetSingleByte(uint8_t byte_index){
	return received_message.whole_byte_array[byte_index];
}

uint8_t CANLIB_Rx_GetSingleChar(uint8_t index){
	return (char)received_message.whole_byte_array[index];
}

void CANLIB_Rx_GetBytes(uint8_t* byte_array){

	for(uint8_t rxMsgIndex = 0; rxMsgIndex < received_message.DLC; rxMsgIndex ++){
		byte_array[rxMsgIndex] = received_message.whole_byte_array[rxMsgIndex];
	}

}

void CANLIB_Rx_GetChars(char* char_array){
	for(uint8_t rxMsgIndex = 0; rxMsgIndex < received_message.DLC; rxMsgIndex ++){
		char_array[rxMsgIndex] = (char)received_message.whole_byte_array[rxMsgIndex];
	}
}

uint32_t CANLIB_Rx_GetAsUint(uint8_t uint_num){
	return (uint_num == 0) ? received_message.return_uint_1 : received_message.return_uint_2;
}

int32_t CANLIB_Rx_GetAsInt(uint8_t int_num){
	return (int_num == 0) ? received_message.return_int_1 : received_message.return_int_2;
}

float CANLIB_Rx_GetAsFloat(uint8_t float_num){
	return (float_num == 0) ? received_message.return_float_1 : received_message.return_float_2;
}

int64_t CANLIB_Rx_GetAsLongInt(){
	return received_message.long_int;
}

uint64_t CANLIB_Rx_GetAsLongUint(){
	return received_message.long_uint;
}

double CANLIB_Rx_GetAsDouble(){
	return received_message.dub;
}

//Convenience functions
int8_t CANLIB_SendBytes(uint8_t* byte_array, uint8_t array_size, uint32_t id){

	CANLIB_ClearDataArray();
	CAN_HandleStruct.pTxMsg->DLC = array_size;
	CANLIB_Tx_SetBytes(byte_array, array_size);
	CAN_HandleStruct.pTxMsg->StdId = id;
	if(HAL_CAN_Transmit_IT(&CAN_HandleStruct) != HAL_OK){
		return -1;
	}
	return 0;
}

