/**
  ******************************************************************************
  * @file    main.c
  * @author  Jake Fisher
  * @version V1.0
  * @date    09-May-2017
  * @brief   UWRT safety board main function (Microcontroller A).
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include "canlib.h"
#include "safety_master_lib.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
#endif

/* GPIO */
#define GPIO_A_OUTPUTS                (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12)
#define GPIO_A_INPUTS                 (GPIO_PIN_15)
#define GPIO_B_OUTPUTS                (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4)
#define GPIO_B_INPUTS                 (GPIO_PIN_5)
#define GPIO_C_OUTPUTS                (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_13)
#define GPIO_C_INPUTS                 /* Not sure if CAN_WKUP_A is an input or an output */
#define GPIO_D_OUTPUTS                /* None */
#define GPIO_D_INPUTS                 (GPIO_PIN_2)

/* Indices in master reference */
#define SHOULDER_INDEX                0
#define ELBOW_INDEX                   1
#define WRIST_INDEX                   2
#define VISION_INDEX                  3
#define EXTRACTION_INDEX              4
#define PC_INDEX                      5

/* A unit8_t allows 8 boards to have unique indicator bits */
volatile static uint8_t HeartbeatCheck = 0;

static ReporterBoard_t HeartbeatMasterRef[MAX_NUM_BOARDS];

static TIM_HandleTypeDef s_TimerInstance = { 
    .Instance = TIM2
};

void HeartbeatMasterRef_Init(ReporterBoard_t* HeartbeatMasterRef){
	uint8_t i;
	for(i=0; i<MAX_NUM_BOARDS; i++){
		HeartbeatMasterRef[i].active = REPORTER_INACTIVE;
	}
	/* Comment out boards if they are not being used */
	ReporterBoard_Init((&(HeartbeatMasterRef[SHOULDER_INDEX])), REPORTER_ACTIVE, SHOULDER_HEARTBEAT_PRIORITY, SHOULDER_FLAG);
	ReporterBoard_Init((&(HeartbeatMasterRef[ELBOW_INDEX])), REPORTER_ACTIVE, ELBOW_HEARTBEAT_PRIORITY, ELBOW_FLAG);
	ReporterBoard_Init((&(HeartbeatMasterRef[WRIST_INDEX])), REPORTER_ACTIVE, WRIST_HEARTBEAT_PRIORITY, WRIST_FLAG);
	ReporterBoard_Init((&(HeartbeatMasterRef[VISION_INDEX])), REPORTER_ACTIVE, VISION_HEARTBEAT_PRIORITY, VISION_FLAG);
	ReporterBoard_Init((&(HeartbeatMasterRef[EXTRACTION_INDEX])), REPORTER_ACTIVE, EXTRACTION_HEARTBEAT_PRIORITY, EXTRACTION_FLAG);
	ReporterBoard_Init((&(HeartbeatMasterRef[PC_INDEX])), REPORTER_ACTIVE, PC_HEARTBEAT_PRIORITY, PC_FLAG);
}

void TIM2_IRQHandler(){
	HAL_TIM_IRQHandler(&s_TimerInstance);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	/* Set all boards to active in case they were previously turned off */
	GPIO_Reset(Appliance.reset);
	GPIO_Reset(Vision.reset);
	Relay_Reset(&Relay1);

	if(!((HeartbeatCheck & EXTRACTION_FLAG) || (HeartbeatCheck & ARM_FLAG))){
		/* Toggle appliance board(s) */
		GPIO_Set(Appliance.reset);
	}
	if(!(HeartbeatCheck & VISION_FLAG)){
		/* Toggle vision board */
		GPIO_Set(Vision.reset);
	}
	if(!(HeartbeatCheck & PC_FLAG)){
		/* Toggle PC relay */
		Relay_Set(&Relay1);
	}
	HeartbeatCheck = 0;
}

void Timer_Init(uint32_t period){
    __TIM2_CLK_ENABLE();
    s_TimerInstance.Init.Prescaler = 40000;
    s_TimerInstance.Init.CounterMode = TIM_COUNTERMODE_UP;
    s_TimerInstance.Init.Period = period;
    s_TimerInstance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    s_TimerInstance.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&s_TimerInstance);
    HAL_TIM_Base_Start_IT(&s_TimerInstance);
}
 
void GPIO_Init(){
    /* Ensure peripheral-specific (SPI, I2C, UART) GPIOs get set elsewhere */

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_A_OUT_InitStruct, GPIO_B_OUT_InitStruct, GPIO_C_OUT_InitStruct, GPIO_A_IN_InitStruct, GPIO_B_IN_InitStruct, GPIO_D_IN_InitStruct;
    
    GPIO_A_OUT_InitStruct.Pin = GPIO_A_OUTPUTS;
    GPIO_A_OUT_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_A_OUT_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_A_OUT_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_A_OUT_InitStruct);
    GPIO_B_OUT_InitStruct.Pin = GPIO_B_OUTPUTS;
    GPIO_B_OUT_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_B_OUT_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_B_OUT_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_B_OUT_InitStruct);
    GPIO_C_OUT_InitStruct.Pin = GPIO_C_OUTPUTS;
    GPIO_C_OUT_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_C_OUT_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_C_OUT_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_C_OUT_InitStruct);

    GPIO_A_IN_InitStruct.Pin = GPIO_A_INPUTS;
    GPIO_A_IN_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_A_IN_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_A_IN_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_A_IN_InitStruct);
    GPIO_B_IN_InitStruct.Pin = GPIO_B_INPUTS;
    GPIO_B_IN_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_B_IN_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_B_IN_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_B_IN_InitStruct);
    GPIO_D_IN_InitStruct.Pin = GPIO_D_INPUTS;
    GPIO_D_IN_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_D_IN_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_D_IN_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_D_IN_InitStruct);
}
 
void Oscillator_Init(){
	RCC_OscInitTypeDef RCC_OscStruct;
	/* Select HSI48 Oscillator as PLL source */
	RCC_OscStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
	RCC_OscStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
	RCC_OscStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
	RCC_OscStruct.PLL.PLLMUL = RCC_PLL_MUL2;
	
	/* TODO: handle oscillator config failure */
	(void)HAL_RCC_OscConfig(&RCC_OscStruct);
}

void Clock_Init(){
	/* Enable appropriate peripheral clocks */
	__SYSCFG_CLK_ENABLE();
	
	RCC_ClkInitTypeDef RCC_ClkStruct;
	/* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
	RCC_ClkStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	
	/* TODO: handle clock config failure */
	(void)HAL_RCC_ClockConfig(&RCC_ClkStruct, FLASH_LATENCY_1);
}

void CAN_Init(uint32_t id)
{
    switch(CANLIB_Init(id, CANLIB_LOOPBACK_OFF))
    {
        case 0:
            //Initialization of CAN handler successful
            break;
        case -1:
            //Initialization of CAN handler not successful
            break;
        default:
            break;
    }
    CANLIB_AddFilter(SHOULDER_HEARTBEAT_PRIORITY);
    CANLIB_AddFilter(ELBOW_HEARTBEAT_PRIORITY);
    CANLIB_AddFilter(WRIST_HEARTBEAT_PRIORITY);
    CANLIB_AddFilter(VISION_HEARTBEAT_PRIORITY);
    CANLIB_AddFilter(EXTRACTION_HEARTBEAT_PRIORITY);
    CANLIB_AddFilter(PC_HEARTBEAT_PRIORITY);
}

void CANLIB_Rx_OnMessageReceived(void){
	uint8_t i, sender_id;
	uint32_t data_received;
	sender_id = CANLIB_Rx_GetSenderID();
	data_received = CANLIB_Rx_GetAsUint(0);
	for(i=0; i<MAX_NUM_BOARDS; i++){
		if(HeartbeatMasterRef[i].active == REPORTER_ACTIVE){
			if(sender_id == HeartbeatMasterRef[i].heartbeat_id){
				HeartbeatCheck |= HeartbeatMasterRef[i].bit_flag;
			}
		}
	}
}

void Initialization(void){
	HAL_Init();
	Oscillator_Init();
	Clock_Init();
	GPIO_Init();
	Timer_Init(TIMER_PERIOD);

	HeartbeatMasterRef_Init(HeartbeatMasterRef);

	CAN_Init(CAN_SAFETY_SEND_PRIORITY);

	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

int main(void){
    Initialization();
    while(1);
}
