/*********************************************************
     __  ___                   ____
    /  |/  /___ ___________   / __ \____ _   _____  _____
   / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
  / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
 /_/  /_/\__,_/_/  /____/  /_/ |_|\____/|___/\___/_/ 

 Copyright 2017, UW Robotics Team

 @file     encoderlib.h
 @author:  Jerry Li

**********************************************************/
#ifndef ENCODERLIB_H_
#define ENCODERLIB_H_

#define ENCODER1 1
#define ENCODER2 2

int EncoderLib_Init(uint32_t encoder);
uint32_t EncoderLib_ReadCount(uint32_t encoder);

#endif /* ENCODERLIB_H_ */