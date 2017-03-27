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
