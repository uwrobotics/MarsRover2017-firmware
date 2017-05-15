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

// LEDs for discovery board
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

// Encoders
#define ENCODER_1A_PIN      GPIO_PIN_15
#define ENCODER_1A_PORT     GPIOA
#define ENCODER_1B_PIN      GPIO_PIN_3
#define ENCODER_1B_PORT     GPIOB
#define ENCODER_2A_PIN      GPIO_PIN_6
#define ENCODER_2A_PORT     GPIOC
#define ENCODER_2B_PIN      GPIO_PIN_7
#define ENCODER_2B_PORT     GPIOC

// I2C
#define I2C_SCL_GPIO_PIN GPIO_PIN_6
#define I2C_SDA_GPIO_PIN GPIO_PIN_7
#define I2C_GPIO_PORT GPIOB

#endif
