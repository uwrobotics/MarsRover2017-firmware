/*
 * Inspired from github.com/adafruit/Adafruit_VEML6070
 * Datasheet: http://www.vishay.com/docs/84277/veml6070.pdf
 * Product Info: www.adafruit.com/products/2899
*/

#include "stf32f0xx.h"
#include "i2clib.h"
#include "uv.h"

void init_uv(I2C_Device_t *device_ptr, uv_int_time_t int_time) {
	// Create and initialize I2C device
	// Use nominal slave address as UV_ADDR_W (pg. 6 of datasheet)
	I2C_init(I2C1);
	I2C_slave_init(device_ptr, I2C1, UV_ADDR_W, UV_TO);

	// Initialize command byte on sensor (pg. 6 of datasheet)
	uint8_t data = (int_time << 2) | 0x02;
	I2C_send_data(device_ptr, &data, 1);
}

void read_uv(I2C_Device_t *device_ptr, uint16_t *data_ptr) {
	// Read and construct 2-byte data (pg. 7 of datasheet)
	uint8_t tmp_data;

	device_ptr -> address = UV_ADDR_R_MSB;
	I2C_receive_data(device_ptr, &tmp_data, 1);
	*data_ptr = tmp_data << 8;

	device_ptr -> address = UV_ADDR_R_LSB;
	I2C_mem_read(device_ptr, &tmp_data, 1);
	*data_ptr |= tmp_data;
}

