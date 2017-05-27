/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     safety_master_lib.c
@author:  Jake Fisher

**********************************************************/

#include "safety_master_lib.h"

// Globals
GPIOPin_t A12 = { .port = GPIOA, .pin_num = GPIO_PIN_12 };
GPIOPin_t A15 = { .port = GPIOA, .pin_num = GPIO_PIN_15 };
GPIOPin_t B0 = { .port = GPIOB, .pin_num = GPIO_PIN_0 };
GPIOPin_t B4 = { .port = GPIOB, .pin_num = GPIO_PIN_4 };
GPIOPin_t B5 = { .port = GPIOB, .pin_num = GPIO_PIN_5 };
GPIOPin_t C0 = { .port = GPIOC, .pin_num = GPIO_PIN_0 };
GPIOPin_t C1 = { .port = GPIOC, .pin_num = GPIO_PIN_1 };
GPIOPin_t C2 = { .port = GPIOC, .pin_num = GPIO_PIN_2 };
GPIOPin_t C3 = { .port = GPIOC, .pin_num = GPIO_PIN_3 };
GPIOPin_t C4 = { .port = GPIOC, .pin_num = GPIO_PIN_4 };
GPIOPin_t C8 = { .port = GPIOC, .pin_num = GPIO_PIN_8 };
GPIOPin_t C9 = { .port = GPIOC, .pin_num = GPIO_PIN_9 };
GPIOPin_t C10 = { .port = GPIOC, .pin_num = GPIO_PIN_10 };
GPIOPin_t C11 = { .port = GPIOC, .pin_num = GPIO_PIN_11 };
GPIOPin_t C13 = { .port = GPIOC, .pin_num = GPIO_PIN_13 };
GPIOPin_t D2 = { .port = GPIOD, .pin_num = GPIO_PIN_2 };

Relay_t Relay1 = { .id = 1, .trigger = &A12, .sense = &A15 };
Relay_t Relay2 = { .id = 2, .trigger = &B4, .sense = &B5 };
Relay_t Relay3 = { .id = 3, .trigger = &C13, .sense = &D2 };

ToggleBoard_t Appliance = { .reset = &C0, .control1 = &C1, .control2 = &C2, .control3 = &C3 };
//ToggleBoard_t BMS = { .reset = &C4, .control1 = &C5, .control2 = &C6, .control3 = &C7 };
ToggleBoard_t Vision = { .reset = &C8, .control1 = &C9, .control2 = &C10, .control3 = &C11 };
//ToggleBoard_t Auxiliary = { .reset = &B0, .control1 = &B1, .control2 = &B2, .control3 = &B3 };

//////////////////////////////////////////
//      Internal Library Functions      //
//////////////////////////////////////////

ReporterBoard_t* ReporterBoard_Init(ReporterBoard_t* board, uint8_t active, uint32_t heartbeat_id, uint8_t bit_flag){
	board->active = active;
	board->heartbeat_id = heartbeat_id;
	board->bit_flag = bit_flag;
	return board;
}

void GPIO_Set(GPIOPin_t* pin){
	if(!(IS_GPIO_PIN(pin->pin_num))){
		return;
	}
	HAL_GPIO_WritePin(pin->port, pin->pin_num, GPIO_PIN_SET);
}

void GPIO_Reset(GPIOPin_t* pin){
	if(!(IS_GPIO_PIN(pin->pin_num))){
		return;
	}
	HAL_GPIO_WritePin(pin->port, pin->pin_num, GPIO_PIN_RESET);
}

void GPIO_Toggle(GPIOPin_t* pin){
	if(!(IS_GPIO_PIN(pin->pin_num))){
		return;
	}
	HAL_GPIO_TogglePin(pin->port, pin->pin_num);
}

GPIO_PinState GPIO_Read(GPIOPin_t* pin){
	if(!(IS_GPIO_PIN(pin->pin_num))){
		return -1;
	}
	return HAL_GPIO_ReadPin(pin->port, pin->pin_num);
}

void Relay_Set(Relay_t* relay){
	GPIO_Set(relay->trigger);
}

void Relay_Reset(Relay_t* relay){
	GPIO_Reset(relay->trigger);
}

void Relay_Toggle(Relay_t* relay){
	GPIO_Toggle(relay->trigger);
}

GPIO_PinState Relay_Read(Relay_t* relay){
	return GPIO_Read(relay->sense);
}
