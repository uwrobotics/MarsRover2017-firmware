/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
//#include "stm32f072b_discovery.h"

#include "tm/tm_stm32_mpu6050.h"
#include "tm/tm_stm32_delay.h"
#include "tm/tm_stm32_rcc.h"
#include "tm/tm_stm32_exti.h"

#include <math.h>
/*
SCL         PB6           Clock line for I2C
SDA         PB7           Data line for I2C
IRQ         -             User selectable pin if needed. Interrupts for STM must be manually enabled by user.
VCC         3.3V
GND         GND
AD0         -             If pin is low, I2C address is 0xD0, if pin is high, the address is 0xD2
*/


float ax = 0;
float ay = 0;
float az = 0;
float gx = 0;
float gy = 0;
float gz = 0;
float t  = 0;
float ax_offset = 0;
float ay_offset = 0;
float az_offset = 0;
float gx_offset = 0;
float gy_offset = 0;
float gz_offset = 0;
float t_offset  = 0;
float ax_mean = 0;
float ay_mean = 0;
float az_mean = 0;
float gx_mean = 0;
float gy_mean = 0;
float gz_mean = 0;

///////////////////////////////////   CONFIGURATION   /////////////////////////////
//Change this 3 variables if you want to fine tune the skecth to your needs.
int buffersize=1000;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone=8;     //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
int giro_deadzone=1;     //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)
int accelerometer_sensititvity = 4096; //Value to divide the values by
int gyroscope_sensitivity = 131;
TM_MPU6050_t MPU6050;
TM_MPU6050_Interrupt_t MPU6050_Interrupts;


#define IRQ_PORT    GPIOA
#define IRQ_PIN GPIO_PIN_0

/* Flag when we should read */
uint32_t read = 0;

/* Interrupts structure */
void send_command(int command, void *message);


#define MillisecondsIT 1e3

int main(void)
{
	//SystemInit(); //Ensure CPU is running at correctly set clock speed
	//SystemCoreClockUpdate(); //Update SystemCoreClock variable to current clock speed
	SysTick_Config(SystemCoreClock/MillisecondsIT); //Set up a systick interrupt every millisecond


	TM_RCC_InitSystem();

	float i = 0;
	HAL_Init();
	if (TM_MPU6050_Init(&MPU6050, TM_MPU6050_Device_0, TM_MPU6050_Accelerometer_8G, TM_MPU6050_Gyroscope_250s) == TM_MPU6050_Result_Ok) {
			/* Green LED on */
		//	i=1;
	}
	TM_MPU6050_SetDataRate(&MPU6050, TM_MPU6050_DataRate_100Hz);


	/* Enable MPU interrupts */
	TM_MPU6050_EnableInterrupts(&MPU6050);


	/* Enable interrupts on STM32Fxxx device, rising edge */
	TM_EXTI_Attach(IRQ_PORT, IRQ_PIN, TM_EXTI_Trigger_Rising);


	while (1) {
		/* If IRQ happen */
		if (read) {
			/* Reset */
			read = 0;

			/* Read interrupts */
			TM_MPU6050_ReadInterrupts(&MPU6050, &MPU6050_Interrupts);

			/* Check if motion is detected */
			if (MPU6050_Interrupts.F.MotionDetection) {
				/* Toggle RED */
				//TM_DISCO_LedToggle(LED_RED);
			}

			/* Check if data ready */
			if (MPU6050_Interrupts.F.DataReady) {
				/* Read everything from device */
				TM_MPU6050_ReadAll(&MPU6050);

				ax = (float)(MPU6050.Accelerometer_X-ax_offset)/accelerometer_sensititvity ;
				ay = (float)(MPU6050.Accelerometer_Y-ay_offset)/accelerometer_sensititvity ;
				az = (float)(MPU6050.Accelerometer_Z-az_offset)/accelerometer_sensititvity ;
				gx = (float)(MPU6050.Gyroscope_X-gx_offset)/gyroscope_sensitivity;
				gy = (float)(MPU6050.Gyroscope_Y-gy_offset)/gyroscope_sensitivity;
				gz = (float)(MPU6050.Gyroscope_Z-gz_offset)/gyroscope_sensitivity;
				t = (float)MPU6050.Temperature;
				i = sqrt((powf(ax,2)+powf(ay,2)+powf(az,2)));
				//const char s[] = "Hello world\n";
				char s[200] = {0};
				sprintf(s, "ax: %f, ay: %f, az: %f, gx: %f, gy: %f, gz: %f total: %f\n", ax, ay,az,gx,gy,gz,i);
				uint32_t m[] = { 2/*stderr*/, (uint32_t)s, strlen(s)/sizeof(char) };
				send_command(0x05, m);
			}
		}
	}
}

/* EXTI handler */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
	/* Check for PIN */
	if (GPIO_Pin == IRQ_PIN) {
		/* Read interrupts from MPU6050 */
		read = 1;
	}
}

void send_command(int command, void *message)
{
   asm("mov r0, %[cmd];"
       "mov r1, %[msg];"
       "bkpt #0xAB"
         :
         : [cmd] "r" (command), [msg] "r" (message)
         : "r0", "r1", "memory");
}
