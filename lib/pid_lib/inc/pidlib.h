/*******************************************************************
PID library.

@file      pidlib.h
@author    Ivy Xing

How to use this library:
- Change kp, ki, and kd to tune
- Call pid_controller

*******************************************************************/

#ifndef PID_LIB_DEF
#define PID_LIB_DEF
//Controller variables, should change to tune
static float kp = 0.0013;
static float ki = 0;
static float kd = 0;
static float dt = 0.02; //time interval

static int flag = 1; //flag that indicate nothing is wrong; should be set somewhere else


float pid_controller(float curr_val, float target_val);
#endif