/*
 * Inspired from github.com/adafruit/Adafruit_Si7021
 * Datasheet: https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf
 * Product Info: www.adafruit.com/products/3251
*/

#include "stf32f0xx.h"
#include "i2clib.h"
#include "humidity_temperature.h"

#include <math.h>

void init_ht(I2C_Device_t *device_ptr, uint16_t timeout) {
	I2C_slave_init(device_ptr, I2C1, HT_ADDR, timeout);

	// Reset registers on device
	I2C_send_data(device_ptr, &HT_RESET, 1);

	/*
	 * Note: not using this code block for now
	 * Note: if uncomment, change function return type
	 * Note: if uncomment, change 0x3A to a #define-d const
	 * I2C_send_data(device_ptr, &HT_READ_RHT_REG, 1);
	 * uint8_t ht_read_rht_reg = 0;
	 * I2C_receive_data(device_ptr, &ht_read_rht_reg, 1);
	 * if (ht_read_rht_reg != 0x3A) { return -1; }
	*/
}

float read_hum(I2C_Device_t *device_ptr) {
	I2C_send_data(device_ptr, &HT_MEAS_RH_NOHOLD, 1);
	HAL_Delay(25);

	uint8_t tmp_data = 0; // temporary, not temperature

	I2C_receive_data(device_ptr, &tmp_data, 1);
	uint16_t hum_data = tmp_data << 8;
	I2C_receive_data(device_ptr, &tmp_data, 1);
	hum_data |= tmp_data;

	I2C_receive_data(device_ptr, &tmp_data, 1);
	// uint8_t checksum = tmp_data;

	float humidity = hum_data;
	humidity *= HUM_MULTIPLIER;
	humidity /= HUM_DIVISOR;
	humidity -= HUM_SUBTRACTOR;

	return humidity;
}

float read_temp(I2C_Device_t *device_ptr) {
	I2C_send_data(device_ptr, &HT_MEAS_TEMP_NOHOLD, 1);
	HAL_Delay(25);

	uint8_t tmp_data = 0; // temporary, not temperature

	I2C_receive_data(device_ptr, &tmp_data, 1);
	uint16_t temp_data = tmp_data << 8;
	I2C_receive_data(device_ptr, &tmp_data, 1);
	temp_data |= tmp_data;

	I2C_receive_data(device_ptr, &tmp_data, 1);
	// uint8_t checksum = tmp_data;

	float temperature = temp_data;
	temperature *= TEMP_MULTIPLIER;
	temperature /= TEMP_DIVISOR;
	temperature -= TEMP_SUBTRACTOR;

	return temperature;
}