/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

// #include "stm32f0xx.h"
//#include "stm32f072b_discovery.h"
#include "adclib.h"

ADC_HandleTypeDef AdcHandle;

//Internal library function that should not be called by users
int ConfigureADC(uint32_t adc_id)
{
    GPIO_InitTypeDef gpioInit;
    // GPIO_InitTypeDef gpiocInit;

    __GPIOA_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __ADC1_CLK_ENABLE();

    switch(adc_id){
      //PA0
      case 1:
        gpioInit.Pin = GPIO_PIN_0;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpioInit);
        break;

      //PA1
      case 2:
        gpioInit.Pin = GPIO_PIN_1;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpioInit);
        break;

      //PA4
      case 3:
        gpioInit.Pin = GPIO_PIN_4;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpioInit);
        break;

      //PA5
      case 4:
        gpioInit.Pin = GPIO_PIN_5;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpioInit);
        break;

      //PC0
      case 5:
        gpioInit.Pin = GPIO_PIN_0;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &gpioInit);
        break;

      //PC3
      case 6:
        gpioInit.Pin = GPIO_PIN_3;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &gpioInit);
        break;

      //PC4
      case 7:
        gpioInit.Pin = GPIO_PIN_4;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &gpioInit);
        break;

      //PC5
      case 8:
        gpioInit.Pin = GPIO_PIN_5;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &gpioInit);
        break;

      //Invalid ADC ID
      default:
        return -1;
    }


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
      return -2;
    }
    // Run the ADC calibration
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK) {
      return -3;
    }

    return 0;
}

/***************************************************************************
    ADC_Init

    Description: 
    Initializing ADC. Should be called before using ADC.

    Arguments:
    uint32_t        adc_id         ID of the ADC pin you are reading from

    Returns:
    0               Success
    -1              Invalid adc_id
    -2              HAL failed to initialize ADC
    -3              HAL failed to calibrate ADC
***************************************************************************/
int ADC_Init(uint32_t adc_id)
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

  return ConfigureADC(adc_id);
}

/***************************************************************************
    ADC_Read

    Description:
    Read the ADC value from the chosen pin. Returns the reading. Do not call
    this function without calling ADC_Init.

    Arguments:
    uint32_t        adc_id         ID of the ADC pin you are reading from
***************************************************************************/
int ADC_Read(uint32_t adc_id){
  int adc_reading = 0;

  ADC_ChannelConfTypeDef sConfig;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;

  switch(adc_id){
      //PA0
      case 1:
        sConfig.Channel = ADC_CHANNEL_0;
        break;

      //PA1
      case 2:
        sConfig.Channel = ADC_CHANNEL_1;
        break;

      //PA4
      case 3:
        sConfig.Channel = ADC_CHANNEL_4;
        break;

      //PA5
      case 4:
        sConfig.Channel = ADC_CHANNEL_5;
        break;

      //PC0
      case 5:
        sConfig.Channel = ADC_CHANNEL_10;
        break;

      //PC3
      case 6:
        sConfig.Channel = ADC_CHANNEL_13;
        break;

      //PC4
      case 7:
        sConfig.Channel = ADC_CHANNEL_14;
        break;

      //PC5
      case 8:
        sConfig.Channel = ADC_CHANNEL_15;
        break;

      //Invalid ADC ID
      default:
        while(1){
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        }
    }

  AdcHandle.Instance->CHSELR = 0;
  HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

  HAL_ADC_Start(&AdcHandle); // Start conversion
  // Wait end of conversion and get value
  if (HAL_ADC_PollForConversion(&AdcHandle, 10) == HAL_OK) {
    adc_reading =  (HAL_ADC_GetValue(&AdcHandle));
  } else {
    adc_reading = 0;
  }

  return adc_reading;
}
