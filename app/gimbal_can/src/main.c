#include "stm32f0xx.h"
#include "pwmlib.h"
#include "canlib.h"
#include "servo_lib.h"
#include "pins.h"
#include "adclib.h"
#include "uart_lib.h"
#include <math.h>
#include <string.h>

#define PWM_ID 	1
#define ADC_ID	1

//TODO: set these values to the correct ones for the pins used
#define HORZ_PWM 1
#define VERT_PWM 2
#define HORZ_ADC 1
#define PERIOD  19999 // 1 tick = 1 us

#define GIMBAL_CAN_NODE 600



volatile int horizontal_angle = 0;

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
            .Pin        = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &LED_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

}

void CANLIB_Rx_OnMessageReceived() {
//We have recieved a message, find out what it is telling us to do.
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    int horz_angle = CANLIB_Rx_GetAsInt(0);

    int vert_angle = CANLIB_Rx_GetAsInt(1);
    horizontal_angle = horz_angle;
	//WriteContinuousServo(HORZ_PWM, HORZ_ADC, horz_angle);
    WriteServo(VERT_PWM, vert_angle);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	
}

int main(void)
{

	HAL_Init();
    CLK_Init();
    GPIO_Init();
    //ServoLibInit(PWM_ID, ADC_ID);
    //TODO: initialise safety board code
    
    PWMLIB_Init(HORZ_PWM);
    PWMLIB_Init(VERT_PWM);
    ADC_Init(HORZ_ADC);

    CANLIB_Init(GIMBAL_CAN_NODE, CANLIB_LOOPBACK_OFF);
    CANLIB_AddFilter(600);

	PWMLIB_ChangePeriod(VERT_PWM, 20000); // Change period to 50 Hz //Is this necessary? (taken from gimbal test example)
    PWMLIB_ChangePeriod(HORZ_PWM, 20000);

    //This is just testing. Remove for comp
    /*
    WriteServo(VERT_PWM,-60);
    HAL_Delay(1000);

    WriteServo(VERT_PWM,-30);
    HAL_Delay(1000);

    WriteServo(VERT_PWM,0);
    HAL_Delay(1000);

    WriteServo(VERT_PWM,30);
    HAL_Delay(1000);
    WriteServo(VERT_PWM,59);
    HAL_Delay(1000);
    */

    //Actual Code
	while(1) {
        WriteContinuousServo(HORZ_PWM,HORZ_ADC, horizontal_angle);
    }
}





