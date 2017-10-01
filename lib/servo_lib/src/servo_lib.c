/*********************************************************
    __  ___                   ____
   /  |/  /___ ___________   / __ \____ _   _____  _____
  / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
 / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
/_/  /_/\__,_/_/  /____/  /_/ |_|\____./|___/\___/_/

Copyright 2017, UW Robotics Team

@file     servo_lib.c
@author:  Aditya Matam

**********************************************************/

#include "pwmlib.h"
#include "servo_lib.h"
#include "pidlib.h"
#include "stm32f0xx.h"
#include "adclib.h"
#include "uart_lib.h"
#include <math.h>
#include <stdlib.h>

////////////////////////////////////////
//      Public Library Functions      //
////////////////////////////////////////

/*******************************************************************************************************
    PWMLIB_WriteServo
    -------------------

    Description:
    ============
    Writes a PWM signal to move a non-continuous servo to the specified degree value.

    Arguments:
    ==========
    uint32_t        pwm_id              ID of the PWM channel
    float           degrees             Number of degrees to turn servo (negative values allowed)

*******************************************************************************************************/
void WriteServo(uint32_t pwm_id, float degrees)
{
    // Limit range to 60 degrees. Note: servo is capped at 60 degrees on either side
    if (degrees>45)
        degrees=45;
    else if (degrees<-60)
        degrees=-60;
    PWMLIB_Write(pwm_id, (1500 + degrees/0.08)/20000); // From manufacturere: 0.08 degrees per us
}

/*******************************************************************************************************
    PWMLIB_WriteContinuousServo
    -------------------

    Description:
    ============
    Writes a PWM signal to move a continuous servo to the specified degree value.

    Arguments:
    ==========
    uint32_t        pwm_id              ID of the PWM channel
    uint8_t         adc_id              ID of the ADC channel
    uint16_t        degrees             Target position

*******************************************************************************************************/
void WriteContinuousServo(uint32_t pwm_id, uint8_t adc_id, uint16_t degrees)
{
    // Restrict range to 5-305 degrees
    if (degrees>300)
        degrees=300;
    degrees+=5;

    uint16_t readIn = ADC_Read(adc_id);
    int error = degrees*READING_PER_DEGREE - readIn;
    float distance = 0;
    uint16_t pulseWidth;
    // Move the servo until close enough to the destination (1 degree)
        while (abs(error)>READING_PER_DEGREE)
        {
            HAL_Delay(2);
            UART_LIB_PRINT_INT(readIn);  
            readIn = ADC_Read(adc_id);
            distance = pid_controller(readIn, degrees*READING_PER_DEGREE);
            error = degrees*READING_PER_DEGREE - readIn;
            // distance = error/4905.0;
            if (error>0)
                pulseWidth = (uint16_t)(1490 + 87.5*distance); // Define later
            else
                pulseWidth = (uint16_t)(1430 + 87.5*distance);
            PWMLIB_Write(pwm_id, pulseWidth/20000.0);
        }
    //Stop the servo from moving any further
    PWMLIB_Write(pwm_id, 1465/20000.0);
}
