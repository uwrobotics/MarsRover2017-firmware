/*
 * Inspired from github.com/adafruit/Adafruit_Si7021
 * Datasheet: https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf
 * Product Info: www.adafruit.com/products/3251
 * Application Note: https://www.silabs.com/documents/public/application-notes/AN607.pdf
*/

#include "stm32f0xx.h"
#include "i2clib.h"
#include "humidity_temperature.h"

#include <math.h>

int init_ht(HT_Device_t *ht_device_ptr, uint16_t timeout) {
    // Initialize struct members
    ht_device_ptr -> ser_num_a = HT_FOOBAR;
    ht_device_ptr -> ser_num_b = HT_FOOBAR;
    ht_device_ptr -> temp_ambient = HT_FOOBAR;
    ht_device_ptr -> correction_factor = HT_FOOBAR;

    // Initialize I2C device
    I2C_slave_init(&ht_device_ptr -> device, I2C1, HT_ADDR, timeout);
    I2C_slave_mem_init(&ht_device_ptr -> device, 8);

    // Reset registers on device
    I2C_send_data(&ht_device_ptr -> device, &HT_RESET, 1);
    HAL_Delay(50);

    // Variable to store return values of functions
    uint8_t check_var = HT_FOOBAR;

    // Check initialized state of user register (pg. 26 of datasheet)
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

    read_temp_ambient(ht_device_ptr);

    return 0;
}

// #include "uart_lib.h"
// const uint8_t ORH_LABEL[] = "Orig. RH: ";
// const uint8_t SEPARATOR[] = " | ";

float read_hum(HT_Device_t *ht_device_ptr) {
    uint16_t hum_data = HT_FOOBAR; // Variable to store raw humidity data
    uint8_t hum_buffer[3] = {0, 0, 0};

    I2C_mem_read(&ht_device_ptr -> device, HT_MEAS_RH_HOLD, &hum_buffer, 3);
    if (check_CRC(&hum_buffer) == -1) {
        return -1;
    }
    hum_data = hum_buffer[0];
    hum_data <<= 8;
    hum_data |= hum_buffer[1];

    float humidity = hum_data; // Variable to store processed humidity data
    humidity *= HUM_MULTIPLIER;
    humidity /= HUM_DIVISOR;
    humidity -= HUM_SUBTRACTOR;

    // UART_LIB_PRINT_CHAR_ARRAY(ORH_LABEL, sizeof(ORH_LABEL));
    // UART_LIB_PRINT_DOUBLE(humidity);
    // UART_LIB_PRINT_CHAR_ARRAY(SEPARATOR, sizeof(SEPARATOR));

    // temp. compensation (see pg. 29 of app. note)
    // float temp_meas = read_temp(ht_device_ptr, 1);
    // humidity = humidity / (1 - (ht_device_ptr -> correction_factor) * (temp_meas - ht_device_ptr -> temp_ambient));

    return humidity;
}

float read_temp(HT_Device_t *ht_device_ptr, uint8_t is_previous) {
    uint8_t read_temp_cmd = HT_FOOBAR; // Command to write during read
    uint8_t num_bytes = HT_FOOBAR; // Num. bytes to request during read

    if (is_previous) {
        // Fetch the temperature that was recorded during previous humidity measurement
        // Note: requires a humidity measurement being made prior to this! (pg. 21 of datasheet)
        read_temp_cmd = HT_READ_PREV_TEMP;
        num_bytes = 2;
    } else {
        // Make a new temperature measurement
        read_temp_cmd = HT_MEAS_TEMP_HOLD;
        num_bytes = 3;
    }

    uint16_t temp_data = HT_FOOBAR; // Variable to store raw temp. data
    uint8_t temp_buffer[3] = {0, 0, 0};

    I2C_mem_read(&ht_device_ptr -> device, read_temp_cmd, &temp_buffer, num_bytes);
    if (num_bytes == 3 && check_CRC(&temp_buffer) == -1) {
        // Only check CRC if a new temperature measurement was made
        // No checksum is returned if a previous temp. was fetched
        return -1;
    }
    temp_data = temp_buffer[0];
    temp_data <<= 8;
    temp_data |= temp_buffer[1];

    float temperature = temp_data; // Variable to store temp. humidity data
    temperature *= TEMP_MULTIPLIER;
    temperature /= TEMP_DIVISOR;
    temperature -= TEMP_SUBTRACTOR;

    return temperature;
}

float read_temp_ambient(HT_Device_t *ht_device_ptr) {
    // Read and record surrounding temperature
    float total = 0; // Total for average calculation
    const uint8_t NUM_ITER = 5; // Num. of samples for average calculation

    uint8_t idx = 0;
    for (idx = 0; idx < NUM_ITER; idx += 1) {
        // Make a NEW temperature measurement each time and add up
        total += read_temp(ht_device_ptr, 0);
        HAL_Delay(100);
    }

    // Record average temp. measurement value
    ht_device_ptr -> temp_ambient = total / NUM_ITER;
    // Record correction factor in case temperature compensation is to be done; see pg. 29 of application note
    ht_device_ptr -> correction_factor = 0.0598 - 0.000346 * ht_device_ptr -> temp_ambient;
}

int store_ser_num(HT_Device_t *ht_device_ptr) {
    // See pg. 23-24 of datasheet for algorithm explanation
    // Not yet implemented: CRC checking for serial number
    
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
    // At this point SNB3 *should* be equal to 0x15 = 21 for Si7021 according to pg. 24 of datasheet
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

int check_CRC(uint8_t *data_buffer) {
    // Checksum verification; data validation
    // NOTE: ASSUMES I/P IS ARRAY WITH INDICES 0 AND 1 BEING DATA BYTES AND INDEX 2 BEING CRC BYTE
    // See http://community.silabs.com/t5/Optical-RH-Temp-Sensor/how-to-calculate-CRC-in-SI7021/m-p/141929#M257
    uint8_t crc_byte = 0x00;
    uint8_t i, j;
    for (i = 0; i < 2; i += 1) {
        crc_byte ^= data_buffer[i];
        for (j = 8; j > 0; j -= 1) {
            if (crc_byte & 0x80) {
                crc_byte = (crc_byte << 1) ^ 0x131;
            } else {
                crc_byte = (crc_byte << 1);
            }
        }
    }
    if (crc_byte != data_buffer[2]) {
        return -1;
    }
    return 0;
}