/*********************************************************
     __  ___                   ____
    /  |/  /___ ___________   / __ \____ _   _____  _____
   / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
  / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
 /_/  /_/\__,_/_/  /____/  /_/ |_|\____/|___/\___/_/

 Copyright 2017, UW Robotics Team

 @file     pwmiclib.h
 @author:  Archie Lee

**********************************************************/
#ifndef PWMICLIB_H_
#define PWMICLIB_H_

#define MAX_INPUT_CAPTURE       4

#define ENCODER1                1
#define ENCODER2                2
#define ENCODER3                3
#define ENCODER4                4

#define ENCODER_PWM1_PIN        ENCODER_1A_PIN
#define ENCODER_PWM1_PORT       ENCODER_1A_PORT
#define ENCODER_PWM2_PIN        ENCODER_1B_PIN
#define ENCODER_PWM2_PORT       ENCODER_1B_PORT
#define ENCODER_PWM3_PIN        ENCODER_2A_PIN
#define ENCODER_PWM3_PORT       ENCODER_2A_PORT
#define ENCODER_PWM4_PIN        ENCODER_2B_PIN
#define ENCODER_PWM4_PORT       ENCODER_2B_PORT

#define ENCODER_PRESCALER       47
#define ENCODER_PERIOD          4096


int PWMICLIB_Init(uint32_t encoder);
uint32_t PWMICLIB_Read(uint32_t encoder);

#endif /* PWMICLIB_H_ */
