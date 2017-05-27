/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     safety_slave_lib.c
@author:  Jake Fisher

**********************************************************/

#include "safety_slave_lib.h"
#include "canlib.h"

/* Comment out the following line when compiling the heartbeat master */
//#define HEARTBEAT_SLAVE

static uint32_t HeartbeatPriority;
static TIM_HandleTypeDef HeartbeatTimer = { 
    .Instance = HEARTBEAT_TIMER
};

//////////////////////////////////////////
//      Internal Library Functions      //
//////////////////////////////////////////

void Heartbeat_Init(uint32_t priority){
    __GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    switch(CANLIB_Init(HeartbeatPriority, CANLIB_LOOPBACK_OFF)){
        case 0:
            //Initialization of CAN handler successful
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
            break;
        case -1:
            //Initialization of CAN handler not successful
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
            break;
        default:
            break;
    }
    /* FOR TESTING RECEIVED CAN MESSAGES */
    //CANLIB_AddFilter(123);

    HeartbeatPriority = priority;
    if(HEARTBEAT_TIMER == TIM2){
        __TIM2_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM2_IRQn, HEARTBEAT_IRQ_PRIORITY, HEARTBEAT_IRQ_PRIORITY);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }else if(HEARTBEAT_TIMER == TIM3){
        __TIM3_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM3_IRQn, HEARTBEAT_IRQ_PRIORITY, HEARTBEAT_IRQ_PRIORITY);
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
    }else if(HEARTBEAT_TIMER == TIM6){
        __TIM6_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM6_IRQn, HEARTBEAT_IRQ_PRIORITY, HEARTBEAT_IRQ_PRIORITY);
        HAL_NVIC_EnableIRQ(TIM6_IRQn);
    }else if(HEARTBEAT_TIMER == TIM7){
        __TIM7_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM7_IRQn, HEARTBEAT_IRQ_PRIORITY, HEARTBEAT_IRQ_PRIORITY);
        HAL_NVIC_EnableIRQ(TIM7_IRQn);
    }else if(HEARTBEAT_TIMER == TIM14){
        __TIM14_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM14_IRQn, HEARTBEAT_IRQ_PRIORITY, HEARTBEAT_IRQ_PRIORITY);
        HAL_NVIC_EnableIRQ(TIM14_IRQn);
    }else{
        return;
    }
    HeartbeatTimer.Init.Prescaler = 40000;
    HeartbeatTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
    HeartbeatTimer.Init.Period = TIMER_PERIOD;
    HeartbeatTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HeartbeatTimer.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&HeartbeatTimer);
    HAL_TIM_Base_Start_IT(&HeartbeatTimer);
}

#ifdef HEARTBEAT_TIM2
void TIM2_IRQHandler(){
    HAL_TIM_IRQHandler(&HeartbeatTimer);
}
#endif

#ifdef HEARTBEAT_TIM3
void TIM3_IRQHandler(){
    HAL_TIM_IRQHandler(&HeartbeatTimer);
}
#endif 

#ifdef HEARTBEAT_TIM6
void TIM6_IRQHandler(){
    HAL_TIM_IRQHandler(&HeartbeatTimer);
}
#endif

#ifdef HEARTBEAT_TIM7
void TIM7_IRQHandler(){
    HAL_TIM_IRQHandler(&HeartbeatTimer);
}
#endif

#ifdef HEARTBEAT_TIM14
void TIM14_IRQHandler(){
    HAL_TIM_IRQHandler(&HeartbeatTimer);
}
#endif

#ifdef HEARTBEAT_SLAVE
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    CANLIB_ChangeID(HeartbeatPriority);
    CANLIB_Tx_SetUint(HEARTBEAT_MESSAGE, CANLIB_INDEX_0);
    CANLIB_Tx_SendData(CANLIB_DLC_FOUR_BYTES);
}
#endif

/* FOR TESTING RECEIVED CAN MESSAGES */
#ifdef HEARTBEAT_SLAVE
void CANLIB_Rx_OnMessageReceived(void){
    uint8_t sender_id;
    uint32_t data_received;
    sender_id = CANLIB_Rx_GetSenderID();
    data_received = CANLIB_Rx_GetAsUint(0);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
}
#endif
