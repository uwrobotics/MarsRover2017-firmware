#include "stm32f0xx.h"
#include "pwmlib.h"
#include "canlib.h"
#include "servo_lib.h"
#include "pins.h"
#include "adclib.h"
#include "uart_lib.h"
#include <math.h>
#include <string.h>

//TODO: set these values to the correct ones for the pins used
#define PAN_PWM 1 //PA8
#define TILT_PWM 2 //PA9
#define PAN_ADC 7 //PA0
#define PERIOD  19999 // 1 tick = 1 us

//state led is PC2
//can led is PC3

//yellow LED to the right of CAN is PC2
//yellow LED on CAN ethernet is PC3

//pan = 130 tilt = -5 will put the camera centered
#define PAN_OFFSET 130
#define TILT_OFFSET -5

#define GIMBAL_CAN_NODE 600
//pwm1 is yellow and is pan
//pwm2 is green and is tilt
//pc4 is pot


volatile int horizontal_angle = PAN_OFFSET;

static void Error_Handler(void)
{
	while (1){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(500);
	}
    
}

void HAL_MspInit(void)
{
    /* System interrupt init*/
    /* SysTick_IRQn interrupt configuration */
    (void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    //Must be set to priority of 1 or else will have higher priority than CAN IRQ
    HAL_NVIC_SetPriority(SysTick_IRQn, 2, 2);

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
            .Pin        = GPIO_PIN_2 | GPIO_PIN_3,
            .Mode       = GPIO_MODE_OUTPUT_PP,
            .Pull       = GPIO_NOPULL,
            .Speed      = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(GPIOC, &LED_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

}

void CANLIB_Rx_OnMessageReceived() {
//We have recieved a message, find out what it is telling us to do.
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
    
    int horz_angle = CANLIB_Rx_GetAsInt(0) + PAN_OFFSET; //offsets to bring to center
    int vert_angle = CANLIB_Rx_GetAsInt(1) + TILT_OFFSET; //offset to bring to center
    horizontal_angle = horz_angle;
	//WriteContinuousServo(PAN_PWM, PAN_ADC, horz_angle);
    WriteServo (TILT_PWM, vert_angle);
    //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	
}

int main(void)
{

	HAL_Init();
    CLK_Init();
    GPIO_Init();
    //TODO: initialise safety board code
    
    PWMLIB_Init(PAN_PWM);
    PWMLIB_Init (TILT_PWM);
    ADC_Init(PAN_ADC);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
    if (CANLIB_Init(543, CANLIB_LOOPBACK_OFF) != 0) Error_Handler();
    if(CANLIB_AddFilter(600)!= 0) Error_Handler();

	PWMLIB_ChangePeriod (TILT_PWM, 20000); // Change period to 50 Hz //Is this necessary? (taken from gimbal test example)
    PWMLIB_ChangePeriod(PAN_PWM, 20000);

    WriteServo(TILT_PWM, TILT_OFFSET);
    WriteContinuousServo(PAN_PWM, PAN_ADC, PAN_OFFSET);
    HAL_Delay(5000);

    //This is just testing. Remove for comp
    
    // WriteServo (TILT_PWM,-60);
    // HAL_Delay(1000);

    // WriteServo (TILT_PWM,-30);
    // HAL_Delay(1000);

    // WriteServo (TILT_PWM,0);
    // HAL_Delay(1000);

    // WriteServo (TILT_PWM,30);
    // HAL_Delay(1000);
    // WriteServo (TILT_PWM,59);
    // HAL_Delay(1000);

    //Actual Code
	while(1) {
        WriteContinuousServo(PAN_PWM, PAN_ADC, horizontal_angle);
    }
}





