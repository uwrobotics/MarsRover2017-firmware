#include "stm32f0xx.h"
#include "pwmlib.h"
#include "pins.h"
#include <math.h>
#include <string.h>

#define PWM_ID 	3
#define PERIOD  19999 // 1 tick = 1 us

static void Error_Handler(void)
{
	while (1){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	}
    
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


void GPIO_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // LEDs
    GPIO_InitTypeDef LED_InitStruct = {
            .Pin        = GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &LED_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

}




void HAL_MspInit(void)
{
    /* System interrupt init*/
    /* SysTick_IRQn interrupt configuration */
    (void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    //Must be set to priority of 1 or else will have higher priority than CAN IRQ
    HAL_NVIC_SetPriority(SysTick_IRQn, 2, 2);
}



int main(void)
{
//  Pulse length (degrees) = (MAX – MIN) * degrees / 180 + MIN
//  Range = 750-2250μsec    
//  pulse length / period = duty cycle

	HAL_Init();
    CLK_Init();
    GPIO_Init();
//	uint32_t period = 20000;

	if (PWMLIB_Init(PWM_ID) != 0)
    {
        Error_Handler(); 	
    }

   

	PWMLIB_ChangePeriod(PWM_ID, PERIOD);

	//PWMLIB_WriteServo (PWM_ID, 0);
	PWMLIB_Write(PWM_ID, 0.075);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(5000);
	PWMLIB_Write(PWM_ID, 0.75);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(5000);
	PWMLIB_Write(PWM_ID, 0.1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(5000);
	PWMLIB_Write(PWM_ID, 0.05);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(5000);
	PWMLIB_Write(PWM_ID, 0.065);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(5000);
	PWMLIB_Write(PWM_ID, 0.5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

}





