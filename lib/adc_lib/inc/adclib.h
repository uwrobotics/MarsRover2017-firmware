/*************************************************

@file     adclib.h
@author   Ivy Xing

How to use this library:
- Initialize the ADC pin with ADC_Init()
- Read values from the pin with ADC_Read()

Note:
- Supported ADC pins
    adc_id   |   pin
    ---------|-------
      1      |   PA1
      2      |   PA2
      3      |   PA3
      4      |   PC1

*************************************************/

#ifndef ADCLIB_H_
#define ADCLIB_H_

#include "stm32f0xx.h"

//Public Functions
int ADC_Init(uint32_t adc_id);
int ADC_Read(uint32_t adc_id);

#endif