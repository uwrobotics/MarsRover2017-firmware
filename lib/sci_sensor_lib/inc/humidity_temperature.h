/*
 * Inspired from github.com/adafruit/Adafruit_Si7021
 * Datasheet: https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf
 * Product Info: www.adafruit.com/products/3251
*/

#ifndef HUMIDITY_TEMPERATURE_H_
#define HUMIDITY_TEMPERATURE_H_

#include "i2clib.h"

// Note: for future reference, HT = Humidity and Temperature

// I2C slave address
// TODO: CHANGE TO 8-BIT ADDRESS?
#define HT_ADDR			    (0x40)

// Command bits
static const uint8_t HT_MEAS_RH_NOHOLD = 0xF5;
static const uint8_t HT_MEAS_TEMP_NOHOLD = 0xF3;
static const uint8_t HT_RESET = 0xFE;
static const uint8_t HT_READ_RHT_REG = 0xE7;

/*
 * Note: not using these commands for now; keep for reference
 * static const uint8_t HT_MEAS_RH_HOLD = 0xE5;
 * static const uint8_t HT_MEAS_TEMP_HOLD = 0xE3;
 * static const uint8_t HT_READ_PREV_TEMP = 0xE0;
 * static const uint8_t HT_WRITE_RHT_REG = 0xE6;
 * static const uint8_t HT_WRITE_HEATER_REG = 0x51;
 * static const uint8_t HT_READ_HEATER_REG = 0x11;
 * static const uint8_t HT_ID1 = 0xFA0F;
 * static const uint8_t HT_ID2 = 0xFCC9;
 * static const uint8_t HT_FIRM_VERS = 0x84B8;
*/

// Arithmetic constants
#define HUM_MULTIPLIER	125
#define HUM_DIVISOR		65536
#define HUM_SUBTRACTOR	6
#define TEMP_MULTIPLIER	175.72
#define TEMP_DIVISOR	65536
#define TEMP_SUBTRACTOR	46.85

// Initialize sensor with I2C timeout
void init_ht(I2C_Device_t *device_ptr, uint16_t timeout);

// Read humidity value
float read_hum(I2C_Device_t *device_ptr);

// Read temperature value
float read_temp(I2C_Device_t *device_ptr);

// TODO: WAY TO IDENTIFY MULTIPLE HT SENSORS ON COLLECTION MODULE
// TODO: NEED CHECKSUM PROCESSING FUNCTIONALITY?
// TODO: NEED TEMPERATURE COMPENSATION DURING HUMIDITY MEASUREMENT?

#endif