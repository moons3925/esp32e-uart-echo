#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define CONFIG_EXAMPLE_UART_TXD 23 // (1)
#define CONFIG_EXAMPLE_UART_RXD 22 // (1)
#define CONFIG_EXAMPLE_UART_PORT_NUM 1 // (2)
#define CONFIG_EXAMPLE_UART_BAUD_RATE 9600 // (3)
#define CONFIG_EXAMPLE_TASK_STACK_SIZE 4096 // (4)

#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

#define BUF_SIZE (1024)

static void echo_task(void *arg)
{
    uart_config_t uart_config = { // (5)
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM // (6)
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags)); // (7)
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config)); // (8)
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS)); // (9)

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE); // (10)

    while (1) {
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, 5, 60000 / portTICK_RATE_MS); // (11)
        uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len); // (12)
    }
}

void app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL); // (13)
}
