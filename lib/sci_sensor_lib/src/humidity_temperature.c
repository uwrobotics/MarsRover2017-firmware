/*
 * Inspired from github.com/adafruit/Adafruit_Si7021
 * Datasheet: https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf
 * Product Info: www.adafruit.com/products/3251
*/

#include "stf32f0xx.h"
#include "i2clib.h"
#include "humidity_temperature.h"

#include <math.h>

void init_ht(HT_Device_t *ht_device_ptr, uint16_t timeout) {
	I2C_slave_init(ht_device_ptr -> device_ptr, I2C1, HT_ADDR, timeout);

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

float read_hum(HT_Device_t *ht_device_ptr) {
	I2C_send_data(ht_device_ptr -> device_ptr, &HT_MEAS_RH_NOHOLD, 1);
	HAL_Delay(25);

	uint8_t tmp_data = 0; // temporary, not temperature

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	uint16_t hum_data = tmp_data << 8;
	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	hum_data |= tmp_data;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// uint8_t checksum = tmp_data; // processing checksum not yet implemented

	float humidity = hum_data;
	humidity *= HUM_MULTIPLIER;
	humidity /= HUM_DIVISOR;
	humidity -= HUM_SUBTRACTOR;

	return humidity;
}

float read_temp(HT_Device_t *ht_device_ptr) {
	I2C_send_data(ht_device_ptr -> device_ptr, &HT_MEAS_TEMP_NOHOLD, 1);
	HAL_Delay(25);

	uint8_t tmp_data = 0; // temporary, not temperature

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	uint16_t temp_data = tmp_data << 8;
	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	temp_data |= tmp_data;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// uint8_t checksum = tmp_data; // processing checksum not yet implemented

	float temperature = temp_data;
	temperature *= TEMP_MULTIPLIER;
	temperature /= TEMP_DIVISOR;
	temperature -= TEMP_SUBTRACTOR;

	return temperature;
}

void store_ser_num(HT_Device *ht_device_ptr) {
	// See pg. 23-24 of datasheet for algorithm explanation
	uint8_t tmp_data = 0;

	// First phase of serial number extraction

	I2C_send_data(ht_device_ptr -> device_ptr, &HT_SNA_1, 1);
	I2C_send_data(ht_device_ptr -> device_ptr, &HT_SNA_2, 1);

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	ht_device_ptr -> ser_num_a = tmp_data;
	ht_device_ptr -> ser_num_a <<= 8;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// uint8_t checksum = tmp_data; // processing checksum not yet implemented

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	ht_device_ptr -> ser_num_a |= tmp_data;
	ht_device_ptr -> ser_num_a <<= 8;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// checksum = tmp_data;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	ht_device_ptr -> ser_num_a |= tmp_data;
	ht_device_ptr -> ser_num_a <<= 8;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// checksum = tmp_data;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	ht_device_ptr -> ser_num_a |= tmp_data;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// checksum = tmp_data;

	// Second phase of serial number extraction

	I2C_send_data(ht_device_ptr -> device_ptr, &HT_SNB_1, 1);
	I2C_send_data(ht_device_ptr -> device_ptr, &HT_SNB_2, 1);

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// At this point tmp_data *should* be equal to 0x15 = 21 for Si7021
	ht_device_ptr -> ser_num_b = tmp_data;
	ht_device_ptr -> ser_num_b <<= 8;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	ht_device_ptr -> ser_num_b |= tmp_data;
	ht_device_ptr -> ser_num_b <<= 8;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// checksum = tmp_data;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	ht_device_ptr -> ser_num_b |= tmp_data;
	ht_device_ptr -> ser_num_b <<= 8;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	ht_device_ptr -> ser_num_b |= tmp_data;

	I2C_receive_data(ht_device_ptr -> device_ptr, &tmp_data, 1);
	// checksum = tmp_data;
}