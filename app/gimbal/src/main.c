#include "stm32f0xx.h"
#include "pwmlib.h"
#include "servo_lib.h"
#include "pins.h"
#include "adclib.h"
#include "uart_lib.h"
#include <math.h>
#include <string.h>

#define PWM_ID 	1
#define ADC_ID	1
#define PERIOD  19999 // 1 tick = 1 us


volatile float dc = 1300;
// volatile int mill = 1300;
// static TIM_HandleTypeDef s_TimerInstance =
// {
//     .Instance = TIM14
// };


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




// void HAL_MspInit(void)
// {
//     /* System interrupt init*/
//     /* SysTick_IRQn interrupt configuration */
//     (void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

//     //Must be set to priority of 1 or else will have higher priority than CAN IRQ
//     HAL_NVIC_SetPriority(SysTick_IRQn, 2, 2);
// }



// void TIM14_IRQHandler()
// {
//     HAL_TIM_IRQHandler(&s_TimerInstance);
// }

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
// 	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
// 	// if (mill<1800)
// 	// 	mill+=5;
// 	// else
// 	// 	mill-=5;
//     // PWMLIB_Write(PWM_ID, dc);
//     // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);

// }

// void Timer_Init(uint32_t period)
// {
//     __HAL_RCC_TIM14_CLK_ENABLE();
//     s_TimerInstance.Init.Prescaler = 47; // 1 tick = 1 us
//     s_TimerInstance.Init.CounterMode = TIM_COUNTERMODE_UP;
//     s_TimerInstance.Init.Period = 200000;
//     s_TimerInstance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//     s_TimerInstance.Init.RepetitionCounter = 0;
//     HAL_TIM_Base_Init(&s_TimerInstance);
//     HAL_TIM_Base_Start_IT(&s_TimerInstance);
// }




int main(void)
{

	HAL_Init();
    CLK_Init();
    GPIO_Init();
    ServoLibInit(PWM_ID, ADC_ID);
 //    Timer_Init(20000);
	// // // uint32_t period = 20000;
 //    HAL_NVIC_SetPriority(TIM14_IRQn, 1, 1);
 //   	HAL_NVIC_EnableIRQ(TIM14_IRQn);


	if (UART_LIB_INIT() != 0)
    {
        Error_Handler(); 	
    }
    PWMLIB_Init(PWM_ID);
    ADC_Init(ADC_ID);
    uint8_t a[] = "Test";
	UART_LIB_PRINT_CHAR_ARRAY(a, sizeof(a));

	//PWMLIB_WriteServo (PWM_ID, 0);
	PWMLIB_ChangePeriod(PWM_ID, 20000);
	// PWMLIB_Write(PWM_ID, 0.05);
	// HAL_Delay(5000);
	// PWMLIB_Write(PWM_ID, 0.1);

	// PWMLIB_Write(PWM_ID, 0.1);
	// HAL_Delay(2000);

	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// WriteContinuousServo(PWM_ID, ADC_ID, 45);	
	// HAL_Delay(2000);

	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// WriteContinuousServo(PWM_ID, ADC_ID, 90);
	// HAL_Delay(2000);

	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// WriteContinuousServo(PWM_ID, ADC_ID, 180);	
	// HAL_Delay(2000);

	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// WriteContinuousServo(PWM_ID, ADC_ID, 270);
	// HAL_Delay(2000);

	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// WriteContinuousServo(PWM_ID, ADC_ID, 0);	




	WriteServo(PWM_ID, 0);	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	WriteServo(PWM_ID, 45);	
	HAL_Delay(2000);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	WriteServo(PWM_ID, -45);
	HAL_Delay(2000);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	WriteServo(PWM_ID, 50);	
	HAL_Delay(2000);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	WriteServo(PWM_ID, 60);
	HAL_Delay(2000);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	WriteServo(PWM_ID, -75);
	HAL_Delay(2000);	
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	WriteServo(PWM_ID, 0);
	
	while(1);





















	


	// while (1) {

	// 	for(int pos = 700; pos < 2300; pos += 5)  // goes from 0 degrees to 180 degrees 
	// 	{                                  // in steps of 1 degree 
	// 		// myservo.write(pos);              // tell servo to go to position in variable 'pos'
	// 		dc = pos / 20000.0;
	// 		PWMLIB_Write(PWM_ID, dc);
	// 		HAL_Delay(20);		
	// 	} 
	// 	for(int pos = 700; pos>=2300; pos-=5)     // goes from 180 degrees to 0 degrees 
	// 	{                                
	// 		// myservo.write(pos);              // tell servo to go to position in variable 'pos'
	// 		dc = pos / 20000.0;
	// 		PWMLIB_Write(PWM_ID, dc);
	// 		HAL_Delay(20);
	// 	} 
	// 	UART_LIB_PRINT_INT(ADC_Read(ADC_ID));
	// }

	// }

	// }
	
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	// WriteContinuousServo(PWM_ID, ADC_ID, 0);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// WriteContinuousServo(PWM_ID, ADC_ID, 50);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// WriteContinuousServo(PWM_ID, ADC_ID, 0);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// PWMLIB_WriteContinuousServo(PWM_ID, ADC_ID, 60);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// PWMLIB_WriteContinuousServo(PWM_ID, ADC_ID, 180);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// PWMLIB_WriteContinuousServo(PWM_ID, ADC_ID, 0);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(2000);



	// PWMLIB_WriteContinuousServo(PWM_ID, ADC_ID, 270);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// PWMLIB_WriteContinuousServo(PWM_ID, ADC_ID, 250);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// PWMLIB_WriteContinuousServo(PWM_ID, ADC_ID, 100);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// PWMLIB_WriteContinuousServo(PWM_ID, ADC_ID, 150);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(2000);

	// PWMLIB_WriteContinuousServo(PWM_ID, ADC_ID, 0);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);


	// PWMLIB_WriteServo(PWM_ID, 8);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(5000);

	// PWMLIB_WriteServo(PWM_ID, 7);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(5000);
	
	// PWMLIB_WriteServo(PWM_ID, 6);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(5000);
	
	// PWMLIB_WriteServo(PWM_ID, 5);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(5000);

	// PWMLIB_WriteServo(PWM_ID, 4);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(5000);

	// PWMLIB_WriteServo(PWM_ID, 3);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(5000);

	// PWMLIB_WriteServo(PWM_ID, 2);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	// HAL_Delay(5000);


/*

	// Not moving
	PWMLIB_WriteServo(PWM_ID, -5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(2000);
	
	PWMLIB_WriteServo(PWM_ID, -4.5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(2000);
	
	PWMLIB_WriteServo(PWM_ID, -4);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(2000);


	// Not moving
	PWMLIB_WriteServo(PWM_ID, -3.5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(5000);
	
	PWMLIB_WriteServo(PWM_ID, -3);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(5000);
	
	PWMLIB_WriteServo(PWM_ID, -2.5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(5000);
	
	PWMLIB_WriteServo(PWM_ID, -2);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(5000);

	PWMLIB_WriteServo(PWM_ID, -1.5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(5000);

/*
	// Starts moving CCW at -1
	PWMLIB_WriteServo(PWM_ID, -1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(5000);

	PWMLIB_WriteServo(PWM_ID, -0.5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(5000);

	PWMLIB_WriteServo(PWM_ID, 0);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(5000);

	PWMLIB_WriteServo(PWM_ID, 0.5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(5000);
	// PWMLIB_Write(PWM_ID, 0.065);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	// HAL_Delay(5000);
	// PWMLIB_Write(PWM_ID, 0.5);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);


*/




	// while (1);

}





