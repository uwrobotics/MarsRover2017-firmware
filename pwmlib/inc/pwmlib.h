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
#define PWM_GPIO_PULL               GPIO_PULLUP
#define PWM_GPIO_SPEED              GPIO_SPEED_FREQ_HIGH
#define PWM1_GPIO_ALTERNATE         GPIO_AF2_TIM1
#define PWM2_GPIO_ALTERNATE         GPIO_AF2_TIM1
#define PWM3_GPIO_ALTERNATE         GPIO_AF2_TIM1
#define PWM4_GPIO_ALTERNATE         GPIO_AF1_TIM3

// PWM timer configuration
  /* -----------------------------------------------------------------------
  TIM2 Configuration: generate 4 PWM signals with 4 different duty cycles.

    In this example TIM2 input clock (TIM2CLK) is set to APB1 clock (PCLK1),
    since APB1 prescaler is equal to 1.
      TIM2CLK = PCLK1
      PCLK1 = HCLK
      => TIM2CLK = HCLK = SystemCoreClock

    To get TIM2 counter clock at 16 MHz, the prescaler is computed as follows:
       Prescaler = (TIM2CLK / TIM2 counter clock) - 1
       Prescaler = ((SystemCoreClock) /16 MHz) - 1

    To get TIM2 output clock at 24 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM2 counter clock / TIM2 output clock) - 1
           = 665

    TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM2_ARR + 1)* 100 = 50%
    TIM2 Channel2 duty cycle = (TIM2_CCR2/ TIM2_ARR + 1)* 100 = 37.5%
    TIM2 Channel3 duty cycle = (TIM2_CCR3/ TIM2_ARR + 1)* 100 = 25%
    TIM2 Channel4 duty cycle = (TIM2_CCR4/ TIM2_ARR + 1)* 100 = 12.5%

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */
#define PWM_PRESCALER               47      // System clock frequency / Timer frequency - 1
#define PWM_PERIOD                  19999   // Timer frequency / PWM frequency - 1
#define PWM_CLOCK_DIVISION          TIM_CLOCKDIVISION_DIV1
#define PWM_COUNTER_MODE            TIM_COUNTERMODE_UP

// PWM channel configuration
#define PWM_OC_MODE                 TIM_OCMODE_PWM1
#define PWM_OC_POLARITY             TIM_OCPOLARITY_HIGH
#define PWM_OC_NPOLARITY            TIM_OCNPOLARITY_HIGH
#define PWM_OC_IDLE_STATE           TIM_OCIDLESTATE_RESET
#define PWM_OC_NIDLE_STATE          TIM_OCNIDLESTATE_RESET
#define PWM_OC_FAST_MODE            TIM_OCFAST_DISABLE

// Functions
int PWMLIB_Init(uint32_t pwm_id);
int PWMLIB_Write(uint32_t pwm_id, float duty_cycle);

#endif
