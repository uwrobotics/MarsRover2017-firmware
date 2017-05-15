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
static const uint8_t HT_SNA_1 = 0xFA;
static const uint8_t HT_SNA_2 = 0x0F;
static const uint8_t HT_SNB_1 = 0xFC;
static const uint8_t HT_SNB_2 = 0xC9;

/*
 * Note: not using these commands for now; keep for reference
 * static const uint8_t HT_MEAS_RH_HOLD = 0xE5;
 * static const uint8_t HT_MEAS_TEMP_HOLD = 0xE3;
 * static const uint8_t HT_READ_PREV_TEMP = 0xE0;
 * static const uint8_t HT_WRITE_RHT_REG = 0xE6;
 * static const uint8_t HT_WRITE_HEATER_REG = 0x51;
 * static const uint8_t HT_READ_HEATER_REG = 0x11;
 * static const uint8_t HT_FIRM_VERS = 0x84B8;
*/

// Arithmetic constants
#define HUM_MULTIPLIER	125
#define HUM_DIVISOR		65536
#define HUM_SUBTRACTOR	6
#define TEMP_MULTIPLIER	175.72
#define TEMP_DIVISOR	65536
#define TEMP_SUBTRACTOR	46.85

typedef struct HT_Device {
	I2C_Device_t *device_ptr;
	// 64-bit unique serial number, retreived in 2 phases (pg. 23-24 of datasheet)
	uint32_t ser_num_a;
	uint32_t ser_num_b;
} HT_Device_t;

// Initialize sensor with I2C timeout
void init_ht(HT_Device_t *ht_device_ptr, uint16_t timeout);

// Read humidity value
float read_hum(HT_Device_t *ht_device_ptr);

// Read temperature value
float read_temp(HT_Device_t *ht_device_ptr);

// Record unique serial number of different ht sensors
void store_ser_num(HT_Device *ht_device_ptr);

// TODO: NEED CHECKSUM PROCESSING FUNCTIONALITY?
// TODO: NEED TEMPERATURE COMPENSATION DURING HUMIDITY MEASUREMENT?

#endif