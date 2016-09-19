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
#include "i2c_master.h"
			

int main(void)
{
	SystemInit();
	/*
	STM_EVAL_LEDInit(LED3);

	STM_EVAL_LEDInit(LED4);

	STM_EVAL_LEDInit(LED5);

	STM_EVAL_LEDInit(LED6);*/
	int i;
	I2C_Master_Init ();

	   uint8_t* i2c_tmp = 0 ;

	  // The Reset Device .
	  *i2c_tmp = 0x80;
	  I2C_Write ( 0xD0 , 0x6B , i2c_tmp , 0x01 );
	while(1){
		/*SystemCoreClockUpdate();
		STM_EVAL_LEDOn(LED3);
		while(i<100000){
			i++;
		}
		i=0;
		STM_EVAL_LEDOff(LED3);
		STM_EVAL_LEDOn(LED4);
		while(i<100000){
			i++;
		}
		i=0;
		STM_EVAL_LEDOff(LED4);
		STM_EVAL_LEDOn(LED5);
		while(i<100000){
			i++;
		}
		i=0;
		STM_EVAL_LEDOff(LED5);
		STM_EVAL_LEDOn(LED6);
		while(i<100000){
			i++;
		}
		i=0;
		STM_EVAL_LEDOff(LED6);
*/

		  /* I2C Test (mpu6050) */

		  // Delay some Time
		  //for ( int i = 0 ; i < 2000 ; i ++) { the __asm ​​( "NOP" );}
		  while(i<2000){
		  			i++;
		  		}
		  i=0;
		  // The Wake up Chip .
		  *i2c_tmp = 0x00 ;
		  I2C_Write ( 0xD0 , 0x6B , i2c_tmp , 0x01 );

		  // The Read Chip the above mentioned id
		  I2C_Read ( 0xD0 , 0x75 , i2c_tmp , 0x01 );
		  //Parse i2c_tmp now???
	}
}
