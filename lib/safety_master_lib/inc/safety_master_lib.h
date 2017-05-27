/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     safety_master_lib.h
@author:  Jake Fisher

**********************************************************/

#ifndef SAFETY_MASTER_LIB_H_
#define SAFETY_MASTER_LIB_H_

#include "stm32f0xx.h"

/* Number of boards to monitor in this framework */
#define MAX_NUM_BOARDS                        8

/* The period, in milliseconds, for the timer to trigger */
#define TIMER_PERIOD                          2000

/* CAN identifier for the master board */
/* TODO: Finalize this priority/identifier */
#define CAN_SAFETY_SEND_PRIORITY              999

/* Reporter board states */
#define REPORTER_ACTIVE                       1
#define REPORTER_INACTIVE                     0

/* CAN heartbeat identifiers */
#define SHOULDER_HEARTBEAT_PRIORITY           100
#define ELBOW_HEARTBEAT_PRIORITY              101
#define WRIST_HEARTBEAT_PRIORITY              102
#define VISION_HEARTBEAT_PRIORITY             103
#define EXTRACTION_HEARTBEAT_PRIORITY         104
#define PC_HEARTBEAT_PRIORITY                 105

/* Bitwise identifiers for received heartbeats */
#define SHOULDER_FLAG                         ((uint8_t)0x01)
#define ELBOW_FLAG                            ((uint8_t)0x02)
#define WRIST_FLAG                            ((uint8_t)0x04)
#define VISION_FLAG                           ((uint8_t)0x08)
#define EXTRACTION_FLAG                       ((uint8_t)0x10)
#define PC_FLAG                               ((uint8_t)0x20)
#define ARM_FLAG                              (SHOULDER_FLAG | ELBOW_FLAG | WRIST_FLAG)

// Structs
typedef struct GPIOPin {
	GPIO_TypeDef* port;
	uint16_t pin_num;
} GPIOPin_t;

typedef struct Relay {
	uint8_t id;
	GPIOPin_t* trigger;
	GPIOPin_t* sense;
} Relay_t;

typedef struct ToggleBoard {
	GPIOPin_t* reset;
	GPIOPin_t* control1;
	GPIOPin_t* control2;
	GPIOPin_t* control3;
} ToggleBoard_t;

typedef struct ReporterBoard {
	uint8_t active;
	uint32_t heartbeat_id;
	uint8_t bit_flag;
} ReporterBoard_t;

// Globals
extern GPIOPin_t A12;
extern GPIOPin_t A15;
extern GPIOPin_t B0;
extern GPIOPin_t B4;
extern GPIOPin_t B5;
extern GPIOPin_t C0;
extern GPIOPin_t C1;
extern GPIOPin_t C2;
extern GPIOPin_t C3;
extern GPIOPin_t C4;
extern GPIOPin_t C8;
extern GPIOPin_t C9;
extern GPIOPin_t C10;
extern GPIOPin_t C11;
extern GPIOPin_t C13;
extern GPIOPin_t D2;

extern Relay_t Relay1;
extern Relay_t Relay2;
extern Relay_t Relay3;

extern ToggleBoard_t Appliance;
//extern ToggleBoard_t BMS;
extern ToggleBoard_t Vision;
//extern ToggleBoard_t Auxiliary;

// Public functions
ReporterBoard_t* ReporterBoard_Init(ReporterBoard_t* board, uint8_t active, uint32_t heartbeat_id, uint8_t bit_flag);
void GPIO_Set(GPIOPin_t* pin);
void GPIO_Reset(GPIOPin_t* pin);
void GPIO_Toggle(GPIOPin_t* pin);
GPIO_PinState GPIO_Read(GPIOPin_t* pin);
void Relay_Set(Relay_t* relay);
void Relay_Reset(Relay_t* relay);
void Relay_Toggle(Relay_t* relay);
GPIO_PinState Relay_Read(Relay_t* relay);

#endif
