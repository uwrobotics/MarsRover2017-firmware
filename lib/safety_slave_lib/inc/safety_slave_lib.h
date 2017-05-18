/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     safety_slave_lib.h
@author:  Jake Fisher

**********************************************************/

#ifndef SAFETY_SLAVE_LIB_H_
#define SAFETY_SLAVE_LIB_H_

#include "stm32f0xx.h"

/* The period, in milliseconds, for the timer to trigger */
#define TIMER_PERIOD                          1000

/* Timer for heartbeat signals */
#define HEARTBEAT_TIMER                       TIM6

/* Interrupt priority of timer for triggering heartbeats */
#define HEARTBEAT_IRQ_PRIORITY                1

/* CAN heartbeat identifiers */
#define SHOULDER_HEARTBEAT_PRIORITY           100
#define ELBOW_HEARTBEAT_PRIORITY              101
#define WRIST_HEARTBEAT_PRIORITY              102
#define VISION_HEARTBEAT_PRIORITY             103
#define EXTRACTION_HEARTBEAT_PRIORITY         104
#define PC_HEARTBEAT_PRIORITY                 105

/* The content of a heartbeat message */
#define HEARTBEAT_MESSAGE                     1

// Public functions
void Heartbeat_Init(uint32_t priority);

#endif
