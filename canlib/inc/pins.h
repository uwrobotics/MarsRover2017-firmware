/*
 * pins.h
 *
 *  Created on: Jan. 30, 2017
 *      Author: Archie Lee
 *
 *  Description:
 *  Defines common pin IDs and ports
 */

#ifndef PINS_H_
#define CANLIB_H_

#include "stm32f0xx.h"

// LEDs
#define LED1_GPIO_PIN       GPIO_PIN_9
#define LED2_GPIO_PIN       GPIO_PIN_8
#define LED3_GPIO_PIN       GPIO_PIN_7
#define LED4_GPIO_PIN       GPIO_PIN_6
#define LED_GPIO_PORT       GPIOC

// CAN
#define CAN_GPIO_RX_PIN     GPIO_PIN_8
#define CAN_GPIO_TX_PIN     GPIO_PIN_9
#define CAN_GPIO_PORT       GPIOB

// PWM
#define PWM1_GPIO_PIN       GPIO_PIN_8
#define PWM1_GPIO_PORT      GPIOA
#define PWM2_GPIO_PIN       GPIO_PIN_9
#define PWM2_GPIO_PORT      GPIOA
#define PWM3_GPIO_PIN       GPIO_PIN_10
#define PWM3_GPIO_PORT      GPIOA
#define PWM4_GPIO_PIN       GPIO_PIN_9
#define PWM4_GPIO_PORT      GPIOC


#endif
