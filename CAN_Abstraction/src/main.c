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
#include "can_properties.h"
#include "string.h"
			

int main(void)
{

	uint64_t this = 3;
	int64_t signed_this = -3;
	CANAbstract_Transmit_Uint(this);
	CANAbstract_Transmit_Int(signed_this);

}
