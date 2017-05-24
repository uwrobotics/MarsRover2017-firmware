/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     servo_lib.h
@author:  Aditya Matam

See full function descriptions in pwmlib.c

How to use this library:

1. As usual, initialize the HAL and system clock.
2. Initialize the ADC and PWM channels with ServoLibInit.
3. Write to the PWM channel with WriteServo() or WriteContinuousServo.

Notes:
- On the Discovery boards, PC9 may not work correctly.
  This pin is also used for the onboard LED so the output
  may not work as expected.

**********************************************************/

#ifndef SERVO_LIB_DEF
#define SERVO_LIB_DEF

#include "stm32f0xx.h"
#include "pins.h"

// Potentiometer reading corresponding to 1 degree
#define READING_PER_DEGREE 	(13.20968)

int ServoLibInit(uint8_t pwm_id, uint8_t adc_id);
void WriteServo(uint32_t pwm_id, float degrees);
void WriteContinuousServo(uint32_t pwm_id, uint8_t adc_id, uint16_t degrees);
#endif
