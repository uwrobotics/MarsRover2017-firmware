/*
 * Inspired from github.com/adafruit/Adafruit_VEML6070
 * Datasheet: http://www.vishay.com/docs/84277/veml6070.pdf
 * Product Info: www.adafruit.com/products/2899
 * Application Note: http://vishay.com/docs/84310/designingveml6070.pdf
*/

#include "stm32f0xx.h"
#include "i2clib.h"
#include "uv.h"

void init_uv(I2C_Device_t *device_ptr, const uv_it_t *INT_TIME_PTR, uint16_t timeout) {
    // Initialize I2C device with nominal slave address as UV_ADDR_W (pg. 6 of datasheet)
    I2C_slave_init(device_ptr, I2C1, UV_ADDR_W, timeout);

    // Initialize command byte on sensor (pg. 6 of datasheet)
    uint8_t data = ((*INT_TIME_PTR + 1) << 2) | 0x02;
    I2C_send_data(device_ptr, &data, 1);
}

uint16_t read_uv(I2C_Device_t *device_ptr, const uv_it_t *INT_TIME_PTR) {
    // Read and construct 2-byte data (pg. 7 of datasheet)
    uint8_t tmp_data = 0; // temporary, not temperature

    device_ptr -> address = UV_ADDR_R_MSB;
    I2C_receive_data(device_ptr, &tmp_data, 1);
    uint16_t uv_data = tmp_data << 8;

    device_ptr -> address = UV_ADDR_R_LSB;
    I2C_receive_data(device_ptr, &tmp_data, 1);
    uv_data |= tmp_data;

    // Revert to nominal address
    device_ptr -> address = UV_ADDR_W;

    // Wait one period of int_time
    HAL_Delay(UV_BASE_WAIT << *INT_TIME_PTR);

    return uv_data;
}

uv_class_t get_uv_class(I2C_Device_t *device_ptr, uint16_t *uv_data_ptr, const uv_it_t *INT_TIME_PTR) {
    // See pg. 5 of application note
    uv_class_t uv_class;

    for (uv_class = LOW; uv_class < EXTREME; uv_class += 1) {
        if (*uv_data_ptr <= (UV_BASE_THRESHOLDS[uv_class] << *INT_TIME_PTR)) {
            return uv_class;
        }
    }
    return EXTREME;
}
