
#include "stm32f0xx.h"

void I2C_GPIO_Init(void);
void I2C_Master_DeInit(void);
void I2C_Master_Init(void);

int I2C_Write(uint8_t DeviceAddr, uint8_t RegAddr, uint8_t* pBuffer, uint16_t NumByteToWrite);
int I2C_Read(uint8_t DeviceAddr, uint8_t RegAddr, uint8_t* pBuffer, uint16_t NumByteToRead);

int I2C_TIMEOUT_UserCallback(void);