/*
 * Testing humidity and temperature (HT) sensor code
*/

#include "stm32f0xx.h"
#include "i2clib.h"
#include "humidity_temperature.h"
#include "uart_lib.h"

// Timeout for I2C init
static const uint16_t TIMEOUT = 5000;

// Flag for whether to make a new temp. measurement or to use the
// same value that was taken during previous humidity measurement
// See pg. 21 of datasheet
#define IS_PREVIOUS 1

static void Error_Handler(void);
void CLK_Init(void);

int main(void)
{
    HAL_Init();
    CLK_Init();
    I2C_init(I2C1);
    UART_LIB_INIT();

    // Variable to store return values of functions
    uint8_t check_var = HT_FOOBAR;

    HT_Device_t ht_sensor;
    check_var = init_ht(&ht_sensor, TIMEOUT);
    if (check_var == -1) {
        // Error during init.
        return -1;
    }

    float humidity = HT_FOOBAR;
    float temperature = HT_FOOBAR;

    // Debug statements
    const uint8_t SNA_LABEL[] = "Ser Num A: ";
    const uint8_t SNB_LABEL[] = "Ser Num B: ";
    const uint8_t HUM_LABEL[] = "Hum: ";
    const uint8_t TEMP_LABEL[] = "Temp: ";
    const uint8_t SEPARATOR[] = " | ";
    const uint8_t NEW_LINE[] = "\n";
    UART_LIB_PRINT_CHAR_ARRAY(SNA_LABEL, sizeof(SNA_LABEL));
    UART_LIB_PRINT_INT(ht_sensor.ser_num_a);
    UART_LIB_PRINT_CHAR_ARRAY(SEPARATOR, sizeof(SEPARATOR));
    UART_LIB_PRINT_CHAR_ARRAY(SNB_LABEL, sizeof(SNB_LABEL));
    UART_LIB_PRINT_INT(ht_sensor.ser_num_b);
    UART_LIB_PRINT_CHAR_ARRAY(NEW_LINE, sizeof(NEW_LINE));

    while(1) {
        humidity = read_hum(&ht_sensor);
        temperature = read_temp(&ht_sensor, IS_PREVIOUS);
        
        // Debug statements
        UART_LIB_PRINT_CHAR_ARRAY(HUM_LABEL, sizeof(HUM_LABEL));
        UART_LIB_PRINT_DOUBLE(humidity);
        UART_LIB_PRINT_CHAR_ARRAY(SEPARATOR, sizeof(SEPARATOR));
        UART_LIB_PRINT_CHAR_ARRAY(TEMP_LABEL, sizeof(TEMP_LABEL));
        UART_LIB_PRINT_DOUBLE(temperature);
        UART_LIB_PRINT_CHAR_ARRAY(NEW_LINE, sizeof(NEW_LINE));

        HAL_Delay(500);
    }

    return 0;
}

static void Error_Handler(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    while (1) {}
}

void CLK_Init(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Select HSI48 Oscillator as PLL source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
    {
        Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
    {
        Error_Handler();
    }
}
