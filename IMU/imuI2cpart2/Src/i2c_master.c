
#include "i2c_master.h"

#define I2C_TIMEOUT             ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT             ((uint32_t)(3 * I2C_TIMEOUT))

/**
  * @brief  I2C Sensor Board Interface pins
  */  
#define sBoard_I2C                       I2C2
#define sBoard_I2C_CLK                   RCC_APB1Periph_I2C2
   
#define sBoard_I2C_SCL_PIN               GPIO_Pin_10                 /* PB.10 */
#define sBoard_I2C_SCL_GPIO_PORT         GPIOB                       /* GPIOB */
#define sBoard_I2C_SCL_GPIO_CLK          RCC_AHBPeriph_GPIOB
#define sBoard_I2C_SCL_SOURCE            GPIO_PinSource10
#define sBoard_I2C_SCL_AF                GPIO_AF_1

#define sBoard_I2C_SDA_PIN               GPIO_Pin_11                  /* PB.11 */
#define sBoard_I2C_SDA_GPIO_PORT         GPIOB                       /* GPIOB */
#define sBoard_I2C_SDA_GPIO_CLK          RCC_AHBPeriph_GPIOB
#define sBoard_I2C_SDA_SOURCE            GPIO_PinSource11
#define sBoard_I2C_SDA_AF                GPIO_AF_1

uint32_t I2C_ErrorCounter = 0;
uint32_t I2C_TimeOut;

/**
  * @brief  Initializes peripherals used by the I2C Sensor driver.
  * @param  None
  * @retval None
  */
void I2C_GPIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
      
  /*!< I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
  RCC_AHBPeriphClockCmd(sBoard_I2C_SCL_GPIO_CLK | sBoard_I2C_SDA_GPIO_CLK, ENABLE);
  
  /*!< I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(sBoard_I2C_CLK, ENABLE);
  
  /* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(sBoard_I2C_SCL_GPIO_PORT, sBoard_I2C_SCL_SOURCE, sBoard_I2C_SCL_AF);
  
  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(sBoard_I2C_SDA_GPIO_PORT, sBoard_I2C_SDA_SOURCE, sBoard_I2C_SDA_AF);
  
  /*!< GPIO configuration */  
  /*!< Configure SensorBoard_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = sBoard_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(sBoard_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure SensorBoard_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = sBoard_I2C_SDA_PIN;
  GPIO_Init(sBoard_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  DeInitializes peripherals used by the I2C Sensor driver.
  * @param  None
  * @retval None
  */
void I2C_Master_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
   
  /* I2C Peripheral Disable */
  I2C_Cmd(sBoard_I2C, DISABLE);
 
  /* I2C DeInit */
  I2C_DeInit(sBoard_I2C);

  /*!< I2C Periph clock disable */
  RCC_APB1PeriphClockCmd(sBoard_I2C_CLK, DISABLE);
    
  /*!< GPIO configuration */  
  /*!< Configure I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = sBoard_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(sBoard_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = sBoard_I2C_SDA_PIN;
  GPIO_Init(sBoard_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Initializes peripherals used by the I2C Sensor driver.
  * @param  None
  * @retval None
  */
void I2C_Master_Init(void)
{ 
  I2C_InitTypeDef  I2C_InitStructure;
  
  I2C_GPIO_Init();
  
  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_Timing = 0x00901849; //Fast Mode: 400KHz
  //I2C_InitStructure.I2C_Timing = 0x00700A57; //Fast Mode Plus: 400KHz
    
  /* Apply I2C configuration after enabling it */
  I2C_Init(sBoard_I2C, &I2C_InitStructure);
   
  /* I2C Peripheral Enable */
  I2C_Cmd(sBoard_I2C, ENABLE);
}

int I2C_Write(uint8_t DeviceAddr, uint8_t RegAddr, uint8_t* pBuffer, uint16_t NumByteToWrite)
{  
  uint16_t wLength = 0;

  /* Test on BUSY Flag */
  I2C_TimeOut = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_BUSY) != RESET)
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(sBoard_I2C, DeviceAddr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
  
  /* Wait until TXIS flag is set */
  I2C_TimeOut = I2C_LONG_TIMEOUT;  
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_TXIS) == RESET)   
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }
  
  /* Send Register address */
  I2C_SendData(sBoard_I2C, (uint8_t) RegAddr);
  
  /* Wait until TCR flag is set */
  I2C_TimeOut = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_TCR) == RESET)
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(sBoard_I2C, DeviceAddr, NumByteToWrite, I2C_AutoEnd_Mode, 0);
       
  /* Wait until TXIS flag is set */
  I2C_TimeOut = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_TXIS) == RESET)
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }  

  for(wLength=0; wLength<NumByteToWrite; wLength++)
  {
	/* Write data to TXDR */
	I2C_SendData(sBoard_I2C, pBuffer[wLength]);
	
	/* Wait until TXE flag is set */
	I2C_TimeOut = I2C_LONG_TIMEOUT;
  	while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_TXE) == RESET)
  	{
    	if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  	}  
  }	
 	  
  /* Wait until STOPF flag is set */
  I2C_TimeOut = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_STOPF) == RESET)
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }   
  
  /* Clear STOPF flag */
  I2C_ClearFlag(sBoard_I2C, I2C_ICR_STOPCF);
  
  return 0;
}

int I2C_Read(uint8_t DeviceAddr, uint8_t RegAddr, uint8_t* pBuffer, uint16_t NumByteToRead)
{    
  /* Test on BUSY Flag */
  I2C_TimeOut = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_BUSY) != RESET)
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(sBoard_I2C, DeviceAddr, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  
  /* Wait until TXIS flag is set */
  I2C_TimeOut = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_TXIS) == RESET)
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }
 
  /* Send Register address */
  I2C_SendData(sBoard_I2C, (uint8_t)RegAddr);
  
  /* Wait until TC flag is set */
  I2C_TimeOut = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_TC) == RESET)
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }  
  
  /* Configure slave address, nbytes, reload, end mode and start or stop generation */
  I2C_TransferHandling(sBoard_I2C, DeviceAddr, NumByteToRead, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  
  /* Wait until all data are received */
  while (NumByteToRead)
  {   
    /* Wait until RXNE flag is set */
    I2C_TimeOut = I2C_LONG_TIMEOUT;
    while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_RXNE) == RESET)    
    {
      if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
    }
    
    /* Read data from RXDR */
    *pBuffer = I2C_ReceiveData(sBoard_I2C);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
    
    /* Decrement the read bytes counter */
    NumByteToRead--;
  } 
  
  /* Wait until STOPF flag is set */
  I2C_TimeOut = I2C_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sBoard_I2C, I2C_ISR_STOPF) == RESET)   
  {
    if((I2C_TimeOut--) == 0) return I2C_TIMEOUT_UserCallback();
  }
  
  /* Clear STOPF flag */
  I2C_ClearFlag(sBoard_I2C, I2C_ICR_STOPCF);
  
  /* If all operations OK */
  return 0;  
}  

int I2C_TIMEOUT_UserCallback(void)
{
  /* User can add his own implementation to manage TimeOut Communication failure */
  /* Block communication and all processes */
  I2C_ErrorCounter++;
  I2C_Master_DeInit();
  I2C_Master_Init();
  return -1;
}


