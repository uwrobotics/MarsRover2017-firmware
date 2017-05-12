#include "pidlib.h"

/**********************************************************************
    pid_controller

    Description:
    PID controller.

    Arguments:
    float         curr_val          current value you ar at
    float         target_val        the value you want to get to

    Returns:
    float         output            a duty cycle (probably voltage)
**********************************************************************/
float pid_controller(float curr_val, float target_val){
    //variables that stores values between function calls
    static float prev_err = 0;
    static float accu_err = 0;

    //calculations
    float curr_err = target_val - curr_val;
    if (flag == 0){
    	accu_err = 0;
    }
    else{
        accu_err += curr_err * dt;	
    }
    float rate_err = (curr_err - prev_err) / dt;
    float output = kp * curr_err + ki * accu_err + kd * rate_err;
    prev_err = curr_err;

    //limit output to a duty cycle
    if (output < -1.0) {
    	output = -1.0;
    }
    if (output > 1.0){
    	output = 1.0;
    }

    return output;
}

