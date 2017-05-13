/*
 * Inspired from github.com/adafruit/Adafruit_VEML6070
 * Datasheet: http://www.vishay.com/docs/84277/veml6070.pdf
 * Product Info: www.adafruit.com/products/2899
*/

#include "i2clib.h"
#include "uv.h"

void init_uv(I2C_Device_t *device_ptr, uv_int_time_t int_time) {
	// Create and initialize I2C device
	I2C_TypeDef *I2Cx_ptr;
	I2C_init(I2Cx_ptr);
	I2C_slave_init(device_ptr, I2Cx_ptr, UV_ADDR_W, UV_TO);
	I2C_slave_mem_init(device_ptr, 8);

	// Initialize command byte on sensor (pg. 6 of datasheet)
	uint8_t data = (int_time << 2) | 0x02;
	I2C_mem_write(device_ptr, UV_ADDR_W, &data, 1);
}

void read_uv(I2C_Device_t *device_ptr, uint16_t *data_ptr) {
	// Read and construct 2-byte data (pg. 7 of datasheet)
	uint8_t tmp_data;
	I2C_mem_read(device_ptr, UV_ADDR_R_MSB, &tmp_data, 1);
	*data_ptr = tmp_data << 8;
	I2C_mem_read(device_ptr, UV_ADDR_R_LSB, &tmp_data, 1);
	*data_ptr |= tmp_data;
}
