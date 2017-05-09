#include "stm32f0xx.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
#endif

#define LOWER_BYTE 255
#define UPPER_BYTE (LOWER_BYTE << 8)

void LED_Init(){
    __GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
 
void Oscillator_Init(){
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
}

void Clock_Init(){
	/* Enable appropriate peripheral clocks */
	__SYSCFG_CLK_ENABLE();
	
	RCC_ClkInitTypeDef RCC_ClkStruct;
	/* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
	RCC_ClkStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	
	/* TODO: handle clock config failure */
	(void)HAL_RCC_ClockConfig(&RCC_ClkStruct, FLASH_LATENCY_1);
}

uint16_t pec15table[256];
uint16_t CRC15_POLY = 0x4599;
void init_PEC15_Table(){
	uint16_t rem;
	for(int i=0; i < 256; i++){
		rem = i << 7;
		for(int bit=8; bit>0; --bit){
			if(rem & 0x4000){
				rem = (rem << 1);
				rem = (rem ^ CRC15_POLY);
			} else {
				rem = (rem << 1);
			}
		}
		pec15table[i] = rem & 0xFFFF;
	}
}

uint16_t calc_pec15(uint8_t *data, uint32_t len){
	uint16_t rem, address;
	rem = 16; //PEC seed
	for(int i=0; i<len; i++){
		address = ((rem >> 7) ^ data[i]) & 0xff; //Calculated PEC table address
		rem = (rem << 8) ^ pec15table[address];
	}
	return (rem << 1); //CRC15 has a 0 in the LSB
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi){
	GPIO_InitTypeDef SPI_GPIO_InitStruct;
	
	__GPIOB_CLK_ENABLE();
	SPI_GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    SPI_GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    SPI_GPIO_InitStruct.Pull = GPIO_NOPULL;
    SPI_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    SPI_GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
    HAL_GPIO_Init(GPIOB, &SPI_GPIO_InitStruct);

	__HAL_RCC_SPI2_CLK_ENABLE();
	hspi->Init.Mode = SPI_MODE_MASTER;
	hspi->Init.Direction = SPI_DIRECTION_2LINES;
	hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi->Init.NSS = SPI_NSS_SOFT;
	hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi->Init.CRCPolynomial = 7;
	hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
}

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

void print_byte(UART_HandleTypeDef *huart, uint8_t num){
	uint8_t print_arr[4];
	print_arr[0] = (num / 100) + 48;
	print_arr[1] = ((num % 100) / 10) + 48;
	print_arr[2] = (num % 10) + 48;
	print_arr[3] = '\n';
	HAL_UART_Transmit(huart, print_arr, 4, 0xFF);
}

int main(void){
    uint32_t i, j;
    HAL_StatusTypeDef status;
    UART_HandleTypeDef huart1 = {
    	.Instance = USART1
    };
    SPI_HandleTypeDef hspi2 = {
    	.Instance = SPI2
    };
    uint16_t ADDRESS_COMMAND_MASK;
    uint16_t READ_CONFIGURATION_GROUP;
    uint16_t command_pec;

    uint8_t command_bytes[2];
    //uint8_t rxData[8] = "UNINIT!\n";
    uint8_t printData[8] = "\n\nSTART\n";
    uint8_t loopPrint[6] = "\nLOOP\n";
    uint8_t startData[4];
    uint8_t readTxData[4];
    uint8_t writeTxData[14];
    uint8_t spiTest[8];
    uint8_t rxData[12];
    uint8_t wakeUp = 0xFF;

	HAL_Init();
    Oscillator_Init();
    Clock_Init();

    LED_Init();
    
    HAL_UART_Init(&huart1);

	HAL_SPI_Init(&hspi2);
	init_PEC15_Table();

    HAL_Delay(1000);
    status = HAL_UART_Transmit(&huart1, printData, 8, 0xFF);
    
    ADDRESS_COMMAND_MASK = 1 << 7;
	READ_CONFIGURATION_GROUP = 1 << 1;
	
	//command_bytes[1] = ADDRESS_COMMAND_MASK;
	//command_bytes[0] = READ_CONFIGURATION_GROUP;
	command_bytes[0] = 0x07;
	command_bytes[1] = 0x21;
	//command_pec = calc_pec15(command_bytes, 2);
	
	startData[0] = 0x87;
	startData[1] = 0x23;
	startData[2] = 0xC9;
	startData[3] = 0xF0;

	readTxData[0] = 0x80;
	readTxData[1] = 0x02;
	readTxData[2] = 0x5B;
	readTxData[3] = 0x1E;

	writeTxData[0] = 0x80;
	writeTxData[1] = 0x01;
	writeTxData[2] = 0x4D;
	writeTxData[3] = 0x7A;
	//writeTxData[4] = 0x00;
	writeTxData[4] = 0x04;
	writeTxData[5] = 0x09;
	writeTxData[6] = 0x08;
	writeTxData[7] = 0x07;
	writeTxData[8] = 0x06;
	writeTxData[9] = 0x05;
	//writeTxData[11] = 0xFF;
	writeTxData[10] = 0xEA;
	writeTxData[11] = 0xB6;

	spiTest[0] = 'H';
	spiTest[1] = 'e';
	spiTest[2] = 'l';
	spiTest[3] = 'l';
	spiTest[4] = 'o';
	spiTest[5] = '!';
	spiTest[6] = '!';
	spiTest[7] = '\n';

	rxData[0] = 0x00;
	rxData[1] = 0x00;
	rxData[2] = 0x00;
	rxData[3] = 0x00;
	rxData[4] = 0x00;
	rxData[5] = 0x00;
	rxData[6] = 0x00;
	rxData[7] = 0x00;
	rxData[8] = 0x00;
	rxData[9] = 0x00;
	rxData[10] = 0x00;
	rxData[11] = 0x00;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	for(i=0; i<4; i++){
		print_byte(&huart1, readTxData[i]);
	}
	/*
	while(1){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
		status = HAL_SPI_TransmitReceive(&hspi2, spiTest, rxData, 8, 0xFF);
		while(hspi2.State == HAL_SPI_STATE_BUSY);
		HAL_UART_Transmit(&huart1, rxData, 8, 0xFF);
		HAL_Delay(5000);
	}
	*/
	/*
	while(1){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
		status = HAL_SPI_TransmitReceive(&hspi2, spiTest, rxData, 8, 0xFF);
		while(hspi2.State == HAL_SPI_STATE_BUSY);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(2000);
	}
	*/
	/*
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	status = HAL_SPI_Transmit(&hspi2, &wakeUp, 1, 0xFF);
	while(hspi2.State == HAL_SPI_STATE_BUSY);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	*/
	/*
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	status = HAL_SPI_Transmit(&hspi2, startData, 4, 0xFF);
	while(hspi2.State == HAL_SPI_STATE_BUSY);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	*/
    j = 0;
    while(1){
    	if(j%2){
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    		//status = HAL_SPI_Transmit(&hspi2, &wakeUp, 1, 0xFF);
    		status = HAL_SPI_Transmit(&hspi2, writeTxData, 12, 0xFF);
    		while(hspi2.State == HAL_SPI_STATE_BUSY);
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	    	HAL_UART_Transmit(&huart1, loopPrint, 6, 0xFF);
    	} else{
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    		//status = HAL_SPI_Transmit(&hspi2, &wakeUp, 1, 0xFF);
    		status = HAL_SPI_Transmit(&hspi2, readTxData, 4, 0xFF);
    		status = HAL_SPI_Receive(&hspi2, rxData, 12, 0xFF);
    		while(hspi2.State == HAL_SPI_STATE_BUSY);
    		//status = HAL_SPI_Receive(&hspi2, rxData, 10, 0xFF);
    		//while(hspi2.State == HAL_SPI_STATE_BUSY);
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	    	HAL_UART_Transmit(&huart1, loopPrint, 6, 0xFF);
	    	for(i=0; i<12; i++){
	    		print_byte(&huart1, rxData[i]);
	    	}
    	}
    	if(status == HAL_OK){
    		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    		HAL_Delay(1000);
    	} else{
    		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    		HAL_Delay(1000);
    	}
    	j++;
    }
}
