/*
 * Testing humidity and temperature (HT) sensor code
*/

#include "stm32f0xx.h"
// #include "i2clib.h"
// #include "humidity_temperature.h"

static const uint16_t TIMEOUT = 5000;

static void Error_Handler(void);
void CLK_Init(void);
void TX_Init(void);
void TX_data(float data);

int main(void)
{
    HAL_Init();
    CLK_Init();
	// I2C_init(I2C1);
    TX_Init();

	// HT_Device_t ht_sensor;
	// init_ht(&ht_sensor, TIMEOUT);

    float humidity = 0;
    float temperature = 0;

	while(1) {
		// humidity = read_hum(&ht_sensor);
        // temperature = read_temp(&ht_sensor);
		// display data somehow
        humidity += 1;
        temperature += 2;

		HAL_Delay(500);
	}

    return 0;
}

static void Error_Handler(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    while (1) {}
}

void CLK_Init(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Select HSI48 Oscillator as PLL source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
    {
        Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
    {
        Error_Handler();
    }
}

void TX_Init(void)
{   /*
     * See below link for code comments and explanations
     * stm32f4-discovery.net/2014/04/library-04-connect-stm32f429-discovery-to-computer-with-usart/
    */

    // Initialize PA9

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize USART

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStruct);
    USART_Cmd(USART1, ENABLE);
}

void TX_data(float data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
}