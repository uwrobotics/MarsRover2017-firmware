/*
 * Inspired from github.com/adafruit/Adafruit_VEML6070
 * Datasheet: http://www.vishay.com/docs/84277/veml6070.pdf
 * Product Info: www.adafruit.com/products/2899
 * Application Note: http://vishay.com/docs/84310/designingveml6070.pdf
*/

#ifndef UV_H_
#define UV_H_

#include "i2clib.h"

// Addresses for write and read (pg. 6 of datasheet)
static const uint8_t UV_ADDR_W = 0x70;
static const uint8_t UV_ADDR_R_LSB = 0x71;
static const uint8_t UV_ADDR_R_MSB = 0x73;

// Possible integration times (pg. 8 of datasheet). Ignoring HALF_T since no UV index classifications available.
typedef enum {ONE_T, TWO_T, FOUR_T} uv_it_t;
static const uint16_t UV_BASE_WAIT = 125; // Wait time (ms) corresponding to ONE_T

// Possible UV index classifications, see pg. 5 of application note
typedef enum {LOW, MODERATE, HIGH, VERY_HIGH, EXTREME} uv_class_t;
static const uint16_t UV_BASE_THRESHOLDS[4] = {560, 1120, 1494, 2054}; // UV index classification thresholds corresponding to ONE_T

// Initialize sensor with specified sensor integration time and I2C timeout
void init_uv(I2C_Device_t *device_ptr, const uv_it_t *INT_TIME_PTR, uint16_t timeout);

// Read fixed number of bytes from fixed addres in sensor
uint16_t read_uv(I2C_Device_t *device_ptr, const uv_it_t *INT_TIME_PTR);

// Get UV index classification for retrieved UV light data
uv_class_t get_uv_class(I2C_Device_t *device_ptr, uint16_t *uv_data_ptr, const uv_it_t *INT_TIME_PTR);

#endif