/*
 * i2clib.h
 *
 *  Created on: May 8, 2017
 *      Initial author: Tom
 *
 *
 *
 *
 *
 *
 *  I2CLIB USAGE:
 *
 *  INITIALIZATION
 * Mandatory:
 * 1) I2C_init() -- initialize ic2 for the specified I2C bus (I2C1 or I2C2), address, and timeout
 * 2) I2C_slave_init -- initialize the specified slave device for I2C communication
 * 
VERY IMPORTANT:
There can be some ambiguity about what to use as the address for a slave. The general format of the start of a transmistion is:
AAAA_AAA(R/W), where AAAA_AAA is the 7 address bits, and (R/W) is a bit indicating read (1) or write (0).

Some sensor datasheets give the value AAAAAAA as the slave address, and others give AAAAAAA0 (this is the first one shifted left one bit)
This library uses the second format, so depending on your slave, you may need to shift its address one bit left when initializing. An example of such a slave is Arduino.


 * Optional:
 * 3) I2C_slave_mem_init -- initialize memory address size for the slave device. Required before calling I2C_mem_xxx functions
 *
 *  TRANSMITTING DATA (BLOCKING):
 *  
 * There are 2 functions for transmitting data:
 * 1) I2C_send_data -- this sends n_bytes from the specified data buffer to the specified device
 * 2) I2C_mem_write -- this writes the specified data to the memory address within the device. Must initialize mem on the device
 *                      with I2C_slave_mem_init
 *
 *  RECEIVING MESSAGES:
 * 1) I2C_send_data -- this reads n_bytes to the specified data buffer from the specified device
 * 2) I2C_mem_write -- this reads to p_data from the specified memory address within the device. Must initialize mem on the device
 *                      with I2C_slave_mem_init
 *
 *  TRANSMITTING DATA (NON-BLOCKING):
 *  
 * There are 2 functions for transmitting data:
 * 1) I2C_send_data_IT -- this sends n_bytes from the specified data buffer to the specified device, without blocking.
 *                        I2C_TxComplete() is called when completed, which the user can override if desired
 * 2) I2C_mem_write_IT -- this writes the specified data to the memory address within the device. Must initialize mem on the device
 *                        with I2C_slave_mem_init, without blocking.
 *                        I2C_TxComplete() is called when completed, which the user can override if desired
 *
 *  RECEIVING MESSAGES:
 * 1) I2C_send_data_IT -- this reads n_bytes to the specified data buffer from the specified device, without blocking.
 *                        I2C_RxComplete() is called when completed, which the user can override if desired
 * 2) I2C_mem_write_IT -- this reads to p_data from the specified memory address within the device. Must initialize mem on the device
 *                        with I2C_slave_mem_init, without blocking.
 *                        I2C_RxComplete() is called when completed, which the user can override if desired
 *
 *  CALLBACK FUNCTIONS:
 *  These functions are called when the corresponding non-blocking functions' transmissions have completed.
 *  The user can override them as desired. Both pass a pointer to the device structure with which the transmission was done
 *
 * 1) I2c_Tx_Completed
 * 2) I2C_Rx_Completed
 *
 *  ADDITIONAL NOTES:
 *
 *
 */

#ifndef I2CLIB_H_
#define I2CLIB_H_

#include "stm32f0xx.h"



typedef struct I2C_Device {
    I2C_TypeDef *I2Cx;
    uint16_t address;
    uint32_t timeout;
    uint16_t mem_add_size; //Size of the memory addresses for I2C_mem_xxx.
} I2C_Device_t;

//Initialise the library. This MUST be called before any I2C functions are used on the specified bus
// param I2Cx: The I2C bus you want to initialize (send either I2C1 or I2C2. Theses are HAL constants. Do not make your own I2C_TypeDef
//Examples:
// I2C_init(I2C1);
// I2C_init(I2C2);
int I2C_init(I2C_TypeDef *I2Cx);

/* Initialise a slave device for communication. This must be called fot the specified slave device before communicating with it
 * @param device: a pointer to an I2C_Device_t you have defined locally. It will store the info that the libreary needs about your device
 * @param I2Cx: either I2C1 or I2C2. This specifies the I2C bus to use to communicate with the device
 * @param address: The slave's I2C address. See the warning below
 * @param timeout: A timeout length, in milliseconds. If the blocking I2C functions do not complete within this time, they will cancel and return an error
 * @return: -1 if a parameter wan invalid, 0 otherwise
 */
int I2C_slave_init(I2C_Device_t *device, I2C_TypeDef *I2Cx, uint16_t address, uint32_t timeout);
/*VERY IMPORTANT:
There can be some ambiguity about what to use as the address for a slave. The general format of the start of a transmistion is:
AAAA_AAA(R/W), where AAAA_AAA is the 7 address bits, and (R/W) is a bit indicating read (1) or write (0).

Some sensor datasheets give the value AAAAAAA as the slave address, and others give AAAAAAA0 (this is the first one shifted left one bit)
This library uses the second format, so depending on your slave, you may need to shift its address one bit left when initializing. An example of such a slave is Arduino.
*/

//Required for I2C_mem_xxx functions
//mem_address_size = 8 or 16, depending on slave
//returns 0 unless a prameter was invalid, in which case returns a -1
int I2C_slave_mem_init(I2C_Device_t *device, uint16_t mem_address_size);

  /**************************************/
 /*** Polling (Blocking) Functions    **/
/**************************************/
//These functions will block until their transmission has completed.
//Use if the results of the transmission are crucial to the next steps

// Write n_bytes of data to device
int I2C_send_data(I2C_Device_t *device, uint8_t *p_data, uint16_t n_bytes);

//Read n_bytes of data from device
int I2C_receive_data(I2C_Device_t *device, uint8_t *p_data, uint16_t n_bytes);

// Write to MemAddress in device
int I2C_mem_write(I2C_Device_t *device, uint16_t MemAddress, uint8_t *p_data, uint16_t n_bytes);

// Read from MemAddress in device
int I2C_mem_read(I2C_Device_t *device, uint16_t MemAddress, uint8_t *p_data, uint16_t n_bytes);

  /*****************************************/
 /*** Interrupt (Non-Blocking) Functions **/
/*****************************************/
//These functions do not block, so the code after these are called may be executed before the transmission has completed
//Good for sending data without worrying whether it succeeds or not
//Or for receiving data well in advance of when the data is needed.
//Make sure not to retrieve data before the transmission is complete.
//Make sure not to call an I2C function while one of these functions is still executing, behaviour resulting from this is undefined.


// Write n_bytes of data to device, without blocking. I2C_Tx_Complete() will be called when action is complete, which the user can overload.
int I2C_send_data_IT(I2C_Device_t *device, uint8_t *pdata, uint16_t n_bytes);

//Read n_bytes of data from device, without blocking. I2C_Rx_Complete() will be called when action is complete, which the user can overload.
int I2C_receive_data_IT(I2C_Device_t *device, uint8_t *pdata, uint16_t n_bytes);

// Write to MemAddress in device, without blocking. I2C_Tx_Complete() will be called when action is complete, which the user can overload.
int I2C_mem_write_IT(I2C_Device_t *device, uint16_t MemAddress, uint8_t *p_data, uint16_t n_bytes);

// Read from MemAddress in device, without blocking. I2C_Rx_Complete() will be called when action is complete, which the user can overload.
int I2C_mem_read_IT(I2C_Device_t *device, uint16_t MemAddress, uint8_t *p_data, uint16_t n_bytes);


/* INTERRUPT CALLBACK FUNCTIONS
 * Overload this in your file and they will be called as an interrupt when a corresponding non-blocking I2C operation has completed
 * It gives the pointer to the device most recently used with a non-blocking function on the bus for which the interrupt occured
*/
void I2C_Tx_Complete(I2C_Device_t *device);
void I2C_Rx_Complete(I2C_Device_t *device);


#endif
