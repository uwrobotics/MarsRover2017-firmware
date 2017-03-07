/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     pwmlib.h
@author:  Archie Lee

**********************************************************/
#ifndef PWMLIB_H_
#define PWMLIB_H_

#include "stm32f0xx.h"
#include "pins.h"


#define MAX_PWM_CHANNELS            4

// PWM GPIO configurations
#define PWM_GPIO_MODE               GPIO_MODE_AF_PP
#define PWM_GPIO_PULL               GPIO_NOPULL
#define PWM_GPIO_SPEED              GPIO_SPEED_HIGH
#define PWM1_GPIO_ALTERNATE         GPIO_AF2_TIM1
#define PWM2_GPIO_ALTERNATE         GPIO_AF2_TIM1
#define PWM3_GPIO_ALTERNATE         GPIO_AF2_TIM1
#define PWM4_GPIO_ALTERNATE         GPIO_AF1_TIM3

/* -----------------------------------------------------------------------
TIMx Configuration: generate 4 PWM signals with 4 different duty cycles.

In this example TIMx input clock (TIMxCLK) is set to APB1 clock (PCLK1),
since APB1 prescaler is equal to 1.
  TIMxCLK = PCLK1
  PCLK1 = HCLK
  => TIMxCLK = HCLK = SystemCoreClock

To get TIMx counter clock at 48 MHz, the prescaler is computed as follows:
   Prescaler = (TIMxCLK / TIMx counter clock) - 1
   Prescaler = ((SystemCoreClock) / 48 MHz) - 1

To get TIMx output clock at 24 KHz, the period (ARR)) is computed as follows:
   ARR = (TIMx counter clock / TIMx output clock) - 1
       = 1999

TIMx Channel1 duty cycle = (TIMx_CCR1/ TIMx_ARR + 1)* 100
TIMx Channel2 duty cycle = (TIMx_CCR2/ TIMx_ARR + 1)* 100
TIMx Channel3 duty cycle = (TIMx_CCR3/ TIMx_ARR + 1)* 100
TIMx Channel4 duty cycle = (TIMx_CCR4/ TIMx_ARR + 1)* 100

Note:
 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
 Each time the core clock (HCLK) changes, user had to update SystemCoreClock
 variable value. Otherwise, any configuration based on this variable will be incorrect.
 This variable is updated in three ways:
  1) by calling CMSIS function SystemCoreClockUpdate()
  2) by calling HAL API function HAL_RCC_GetSysClockFreq()
  3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
----------------------------------------------------------------------- */
// PWM timer configuration
#define PWM_PRESCALER               0       // Timer at 48 KHz
#define PWM_PERIOD                  1999    // 48 MHz / 24 KHz - 1 --> 24 KHz output
#define PWM_CLOCK_DIVISION          TIM_CLOCKDIVISION_DIV1
#define PWM_COUNTER_MODE            TIM_COUNTERMODE_UP

// PWM channel configuration
#define PWM_OC_MODE                 TIM_OCMODE_PWM1
#define PWM_OC_POLARITY             TIM_OCPOLARITY_HIGH
#define PWM_OC_IDLE_STATE           TIM_OCIDLESTATE_RESET
#define PWM_OC_FAST_MODE            TIM_OCFAST_DISABLE
#define PWM_OC_DUTY_CYCLE           0 // Initialize to 0

// Externally defined variables for ISR(s)
extern volatile uint32_t pwm_value[MAX_PWM_CHANNELS];

int PWMLIB_Init(uint32_t pwm_id);


#endif
