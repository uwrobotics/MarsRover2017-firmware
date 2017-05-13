/*
 * Inspired from github.com/adafruit/Adafruit_VEML6070
 * Datasheet: http://www.vishay.com/docs/84277/veml6070.pdf
 * Product Info: www.adafruit.com/products/2899
*/

#include "i2clib.h"

// Addresses for write and read (pg. 6 of datasheet)
static const uint8_t UV_ADDR_W = 0x70;
static const uint8_t UV_ADDR_R_LSB = 0x71;
static const uint8_t UV_ADDR_R_MSB = 0x73;

// Default timeout for this I2C device (ms)
static const uint16_t UV_TO = 5000;

// Possible integration times (pg. 8 of datasheet)
typedef enum {HALF, ONE, TWO, FOUR} uv_int_time_t;

// Initialize sensor with specified integration time
void init_uv(I2C_Device_t *device_ptr, uv_int_time_t int_time);

// Read fixed number of bytes from fixed addres in sensor
void read_uv(I2C_Device_t *device_ptr, uint16_t *data_ptr);