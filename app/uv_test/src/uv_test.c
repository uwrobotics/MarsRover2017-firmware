/*
 * Testing UV sensor code
*/

#include "stm32f0xx.h"
#include "i2clib.h"
#include "uv.h"
#include "uart_lib.h"

static const uint16_t TIMEOUT = 5000;

static void Error_Handler(void);
void CLK_Init(void);

int main(void)
{
    HAL_Init();
    CLK_Init();
    I2C_init(I2C1);
    UART_LIB_INIT();

    I2C_Device_t uv_sensor;
    const uv_it_t INT_TIME = TWO_T;
    init_uv(&uv_sensor, &INT_TIME, TIMEOUT);

    uint16_t uv_data = 0;
    uv_class_t uv_class = 0;

    const uint8_t DATA_LABEL[] = "UV Data: ";
    const uint8_t SEPARATOR[] = " | ";
    const uint8_t CLASS_LABEL[] = "UV Class: ";
    const uint8_t NEW_LINE[] = "\n";

    while(1) {
        uv_data = read_uv(&uv_sensor, &INT_TIME);
        uv_class = get_uv_class(&uv_sensor, &uv_data, &INT_TIME);

        UART_LIB_PRINT_CHAR_ARRAY(DATA_LABEL, sizeof(DATA_LABEL));
        UART_LIB_PRINT_INT(uv_data);
        UART_LIB_PRINT_CHAR_ARRAY(SEPARATOR, sizeof(SEPARATOR));
        UART_LIB_PRINT_CHAR_ARRAY(CLASS_LABEL, sizeof(CLASS_LABEL));
        UART_LIB_PRINT_INT(uv_class);
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
