/*******************************************************************
PID library.

@file      pidlib.h
@author    Ivy Xing

How to use this library:
- Change kp, ki, and kd to tune
- Call pid_controller

*******************************************************************/


//Controller variables, should change to tune
float kp = 0;
float ki = 0;
float kd = 0;
float dt = 0.001; //time interval

uint8_t flag = 1; //flag that indicate nothing is wrong; should be set somewhere else


float pid_controller(float curr_val, float target_val);