/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     pwmlib.h
@author:  Archie Lee

See full function descriptions in pwmlib.c

How to use this library:

1. As usual, initialize the HAL and system clock.
2. Initialize the required PWM channels with PWMLIB_Init().
3. Write to the PWM channel with PWMLIB_Write().

Notes:
- All channels on the same timer have the same period
  (i.e. PA8, PA9, and PA10 all use TIM1 so they have the
  same PWM period)
- Don't use the PWMLIB_ChangePeriod() function unless you
  know what you're doing
- More channels can be added as required, just modify the
  .c and .h files
- On the Discovery boards, PC9 may not work correctly.
  This pin is also used for the onboard LED so the output
  may not work as expected.

**********************************************************/

#ifndef PWMLIB_H_
#define PWMLIB_H_

#include "stm32f0xx.h"
#include "pins.h"


#define MAX_PWM_CHANNELS            3

// PWM GPIO configurations
#define PWM_GPIO_MODE               GPIO_MODE_AF_PP
#define PWM_GPIO_PULL               GPIO_PULLUP
#define PWM_GPIO_SPEED              GPIO_SPEED_FREQ_HIGH
#define PWM1_GPIO_ALTERNATE         GPIO_AF2_TIM1
#define PWM2_GPIO_ALTERNATE         GPIO_AF2_TIM1
#define PWM3_GPIO_ALTERNATE         GPIO_AF2_TIM1

// PWM timer configuration
#define PWM_PRESCALER               47  // 1 tick = 1 us
#define PWM_PERIOD                  999 // 1000 us = 1 KHz
#define PWM_CLOCK_DIVISION          TIM_CLOCKDIVISION_DIV1
#define PWM_COUNTER_MODE            TIM_COUNTERMODE_UP

// PWM channel configuration
#define PWM_OC_MODE                 TIM_OCMODE_PWM1
#define PWM_OC_POLARITY             TIM_OCPOLARITY_HIGH
#define PWM_OC_NPOLARITY            TIM_OCNPOLARITY_HIGH
#define PWM_OC_IDLE_STATE           TIM_OCIDLESTATE_RESET
#define PWM_OC_NIDLE_STATE          TIM_OCNIDLESTATE_RESET
#define PWM_OC_FAST_MODE            TIM_OCFAST_DISABLE

// Public Functions
int PWMLIB_Init(uint32_t pwm_id);
int PWMLIB_Write(uint32_t pwm_id, float duty_cycle);
int PWMLIB_ChangePeriod(uint32_t pwm_id, uint32_t period);

#endif
