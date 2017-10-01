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
       1     |   PA0
       2     |   PA1
       3     |   PA4
       4     |   PA5
       5     |   PC0
       6     |   PC3
       7     |   PC4
       8     |   PC5

*************************************************/

#ifndef ADCLIB_H_
#define ADCLIB_H_

#include "stm32f0xx.h"

//Public Functions
int ADC_Init(uint32_t adc_id);
int ADC_Read(uint32_t adc_id);

#endif