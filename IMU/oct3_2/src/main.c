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
#include "stm32f072b_discovery.h"

#include "tm/tm_stm32_mpu6050.h"
#include "tm/tm_stm32_delay.h"

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

void mean_sensors();
void calibration();
void send_command(int command, void *message);
void put_char(char c);
int main(void)
{
	float i = 0;
	HAL_Init();
	if (TM_MPU6050_Init(&MPU6050, TM_MPU6050_Device_0, TM_MPU6050_Accelerometer_8G, TM_MPU6050_Gyroscope_250s) == TM_MPU6050_Result_Ok) {
			/* Green LED on */
		//	i=1;
	}
	//const char s[] = "Hello world\n";
	//uint32_t m[] = { 2/*stderr*/, (uint32_t)s, sizeof(s)/sizeof(char) };
	//send_command(0x05, m);
	//mean_sensors();
	//i = i++;
	//calibration();
	while (1) {
			/* Read everything from device */
			TM_MPU6050_ReadAll(&MPU6050);

			/* Raw data are available for use as needed */
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
			//put_char(s);
			/* Delay a little */
			Delayms(1);
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
void put_char(char c)
{
   asm("mov r0, #0x03\n"
       "mov r1, %[msg]\n"
       "bkpt #0xAB\n"
         :
		 : [msg] "r" (&c)
		 : "r0",  "r1"
		 );
}
void mean_sensors(){
	float ax_buff = 0;
	float ay_buff = 0;
	float az_buff = 0;
	float gx_buff = 0;
	float gy_buff = 0;
	float gz_buff = 0;

	int i = 0;
	while (i<(buffersize+101)){
		// read raw accel/gyro measurements from device
		TM_MPU6050_ReadAll(&MPU6050);

		ax = (float)MPU6050.Accelerometer_X;
		ay = (float)MPU6050.Accelerometer_Y;
		az = (float)MPU6050.Accelerometer_Z;
		gx = (float)MPU6050.Gyroscope_X;
		gy = (float)MPU6050.Gyroscope_Y;
		gz = (float)MPU6050.Gyroscope_Z;

		if (i>100 && i<=(buffersize+100)){ //First 100 measures are discarded
			ax_buff=ax_buff+ax;
			ay_buff=ay_buff+ay;
			az_buff=az_buff+az;
			gx_buff=gx_buff+gx;
			gy_buff=gy_buff+gy;
			gz_buff=gz_buff+gz;
		}
		if (i==(buffersize+100)){
			ax_mean=(ax_mean/buffersize)-ax_offset;
			ay_mean=(ay_mean/buffersize)-ay_offset;
			az_mean=(az_mean/buffersize)-az_offset;
			gx_mean=(gx_mean/buffersize)-gx_offset;
			gy_mean=(gy_mean/buffersize)-gy_offset;
			gz_mean=(gz_mean/buffersize)-gz_offset;
		}
		i++;
		Delayms(2); //Needed so we don't get repeated measures
	}
}
void calibration(){

	ax_offset=-ax_mean/8;
	ay_offset=-ay_mean/8;
	az_offset=(accelerometer_sensititvity - az_mean)/8;

	gx_offset=-gx_mean/4;
	gy_offset=-gy_mean/4;
	gz_offset=-gz_mean/4;


	while (1){
		int ready=0;
		mean_sensors();

		if (abs(ax_mean)<=acel_deadzone){
			ready++;
		}else {
			ax_offset=ax_offset-ax_mean/acel_deadzone;
		}

		if (abs(ay_mean)<=acel_deadzone){
			ready++;
		}else {
			ay_offset=ay_offset-ay_mean/acel_deadzone;
		}

		if (abs(16384-az_mean)<=acel_deadzone){
			ready++;
		}else {
			az_offset=az_offset+(accelerometer_sensititvity-az_mean)/acel_deadzone;
		}

		if (abs(gx_mean)<=giro_deadzone){
			ready++;
		}else {
			gx_offset=gx_offset-gx_mean/(giro_deadzone+1);
		}

		if (abs(gy_mean)<=giro_deadzone){
			ready++;
		}else{
			gy_offset=gy_offset-gy_mean/(giro_deadzone+1);
		}

		if (abs(gz_mean)<=giro_deadzone){
			ready++;
		}else {
			gz_offset=gz_offset-gz_mean/(giro_deadzone+1);
		}

		if (ready==6) break;
	}
}
