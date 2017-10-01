/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
//#include "stm32f072b_discovery.h"
ADC_HandleTypeDef AdcHandle;

void ConfigureADC()
{
    GPIO_InitTypeDef gpioInit;

    __GPIOC_CLK_ENABLE();
    __ADC1_CLK_ENABLE();

    gpioInit.Pin = GPIO_PIN_0; // this pin is what is where the ADC will look at
                               // can also initialize multiple pins for ADC
    gpioInit.Mode = GPIO_MODE_ANALOG;
    gpioInit.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &gpioInit);

    __ADC1_CLK_ENABLE();

     // Configure ADC
     AdcHandle.Instance                   = ADC1;
     AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
     AdcHandle.Init.Resolution            = ADC_RESOLUTION12b;
     AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
     AdcHandle.Init.ScanConvMode          = DISABLE;
     AdcHandle.Init.EOCSelection          = EOC_SINGLE_CONV;
     AdcHandle.Init.LowPowerAutoWait      = DISABLE;
     AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;
     AdcHandle.Init.ContinuousConvMode    = DISABLE;
     AdcHandle.Init.DiscontinuousConvMode = DISABLE;
     AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
     AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
     AdcHandle.Init.DMAContinuousRequests = DISABLE;
     AdcHandle.Init.Overrun               = OVR_DATA_OVERWRITTEN;
     if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
       while(1) {
         HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); //Toggle the state of red LED
         HAL_Delay(100); //delay 100ms
       }
     }
     // Run the ADC calibration
     if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK) {
       while(1) {
         HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8); //Toggle the state of orange LED
         HAL_Delay(100); //delay 100ms
       }
     }
}


int main(void)
{
	HAL_Init();

	RCC_ClkInitTypeDef RCC_ClkStruct;
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

	/* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
	RCC_ClkStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	/* TODO: handle clock config failure */
	(void)HAL_RCC_ClockConfig(&RCC_ClkStruct, FLASH_LATENCY_1);


	/* Enable appropriate peripheral clocks */
	__SYSCFG_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();


	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin = GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6, //for STM32f0-disco board
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_HIGH
	};

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  ConfigureADC();

	while(1) {
    uint16_t adc_reading = 0;

    ADC_ChannelConfTypeDef sConfig;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;

    // this need to be modified depending on which pin you are reading from
    sConfig.Channel = ADC_CHANNEL_10;
    AdcHandle.Instance->CHSELR = 0;
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

    HAL_ADC_Start(&AdcHandle); // Start conversion
    // Wait end of conversion and get value
    if (HAL_ADC_PollForConversion(&AdcHandle, 10) == HAL_OK) {
       adc_reading =  (HAL_ADC_GetValue(&AdcHandle));
    } else {
       adc_reading = 0;
    }

    if (adc_reading < 0x400) {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    } else if (adc_reading < 0x800) {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    } else if (adc_reading < 0xC00) {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    } else {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    }

    HAL_Delay(100);

	}
}
