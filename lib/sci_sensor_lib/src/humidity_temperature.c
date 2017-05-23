/*
 * Inspired from github.com/adafruit/Adafruit_Si7021
 * Datasheet: https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf
 * Product Info: www.adafruit.com/products/3251
*/

#include "stm32f0xx.h"
#include "i2clib.h"
#include "humidity_temperature.h"

#include <math.h>

int init_ht(HT_Device_t *ht_device_ptr, uint16_t timeout) {
    I2C_slave_init(&ht_device_ptr -> device, I2C1, HT_ADDR, timeout);
    I2C_slave_mem_init(&ht_device_ptr -> device, 8);

    // Reset registers on device
    I2C_send_data(&ht_device_ptr -> device, &HT_RESET, 1);

    // Check initialized state of user register (pg. 26 of datasheet)
    uint8_t check_var = 0;
    // I2C_send_data(&ht_device_ptr -> device, &HT_READ_RHT_REG, 1);
    // I2C_receive_data(&ht_device_ptr -> device, &check_var, 1);
    I2C_mem_read(&ht_device_ptr -> device, HT_READ_RHT_REG, &check_var, 1);
    if (check_var != HT_INIT_STATE) {
        return -1;
    }

    // Reusing check_var to check success of another init task
    check_var = store_ser_num(ht_device_ptr);
    if (check_var != 0) {
        // Serial number extraction failed
        return -1;
    }   

    return 0;
}

float read_hum(HT_Device_t *ht_device_ptr) {
    uint16_t hum_data = 0;
    uint8_t hum_buffer[2] = {0, 0};

    I2C_mem_read(&ht_device_ptr -> device, HT_MEAS_RH_HOLD, &hum_buffer, 2);
    hum_data = hum_buffer[0];
    hum_data <<= 8;
    hum_data |= hum_buffer[1];

    float humidity = hum_data;
    humidity *= HUM_MULTIPLIER;
    humidity /= HUM_DIVISOR;
    humidity -= HUM_SUBTRACTOR;

    return humidity;
}

float read_temp(HT_Device_t *ht_device_ptr, uint8_t is_previous) {
    uint8_t read_temp_cmd = 0;
    if (is_previous) {
        // Requires a humidity measurement being made prior to this! (pg. 21 of datasheet)
        read_temp_cmd = HT_READ_PREV_TEMP;
    } else {
        read_temp_cmd = HT_MEAS_TEMP_HOLD;
    }

    uint16_t temp_data = 0;
    uint8_t temp_buffer[2] = {0, 0};

    I2C_mem_read(&ht_device_ptr -> device, read_temp_cmd, &temp_buffer, 2);
    temp_data = temp_buffer[0];
    temp_data <<= 8;
    temp_data |= temp_buffer[1];

    float temperature = temp_data;
    temperature *= TEMP_MULTIPLIER;
    temperature /= TEMP_DIVISOR;
    temperature -= TEMP_SUBTRACTOR;

    return temperature;
}

int store_ser_num(HT_Device_t *ht_device_ptr) {
    // See pg. 23-24 of datasheet for algorithm explanation
    // First access of serial number extraction
    const uint8_t HT_SNA_BUFFER[2] = {HT_SNA_1, HT_SNA_2};
    I2C_send_data(&ht_device_ptr -> device, &HT_SNA_BUFFER, 2);

    uint8_t first_access_buffer[8] = {0};
    I2C_receive_data(&ht_device_ptr -> device, &first_access_buffer, 8);
    ht_device_ptr -> ser_num_a = first_access_buffer[0];
    ht_device_ptr -> ser_num_a <<= 8;
    ht_device_ptr -> ser_num_a |= first_access_buffer[2];
    ht_device_ptr -> ser_num_a <<= 8;
    ht_device_ptr -> ser_num_a |= first_access_buffer[4];
    ht_device_ptr -> ser_num_a <<= 8;
    ht_device_ptr -> ser_num_a |= first_access_buffer[6];

    // Second access of serial number extraction
    const uint8_t HT_SNB_BUFFER[2] = {HT_SNB_1, HT_SNB_2};
    I2C_send_data(&ht_device_ptr -> device, &HT_SNB_BUFFER, 2);

    uint8_t second_access_buffer[6] = {0};
    I2C_receive_data(&ht_device_ptr -> device, &second_access_buffer, 6);
    // At this point tmp_data *should* be equal to 0x15 = 21 for Si7021
    if (second_access_buffer[0] != 0x15) {
        return -1;
    }
    ht_device_ptr -> ser_num_b = second_access_buffer[0];
    ht_device_ptr -> ser_num_b <<= 8;
    ht_device_ptr -> ser_num_b |= second_access_buffer[1];
    ht_device_ptr -> ser_num_b <<= 8;
    ht_device_ptr -> ser_num_b |= second_access_buffer[3];
    ht_device_ptr -> ser_num_b <<= 8;
    ht_device_ptr -> ser_num_b |= second_access_buffer[4];

    return 0;
}