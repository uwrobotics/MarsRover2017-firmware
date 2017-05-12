/*********************************************************
     __  ___                   ____
    /  |/  /___ ___________   / __ \____ _   _____  _____
   / /|_/ / __ `/ ___/ ___/  / /_/ / __ \ | / / _ \/ ___/
  / /  / / /_/ / /  (__  )  / _, _/ /_/ / |/ /  __/ /
 /_/  /_/\__,_/_/  /____/  /_/ |_|\____/|___/\___/_/ 

 Copyright 2017, UW Robotics Team

 @file     uart_lib.h
 @author:  Jerry Li

**********************************************************/
#ifndef _UARTLIB_H_
#define _UARTLIB_H_

int UART_LIB_INIT(void);
int UART_LIB_PRINT_CHAR_ARRAY(uint8_t* data, uint16_t len);
int UART_LIB_PRINT_INT(int data);
int UART_LIB_PRINT_DOUBLE(double data);

#endif /* _UARTLIB_H_ */