//
// Created by bartek on 3/28/20.
//
#include "nb_iot.h"
#include "uart_comm.h"
#include "string.h"
#include "stdlib.h"

void nb_iot_rx_callback()
{
    char message[100];
    nb_iot_uart.rx_flag = 0;
    sprintf(message, "'%s'", nb_iot_uart.raw_data);

}

uint8_t nb_check_sim_status()
{
    uint8_t error_handler = 0;
    uart_send_message(&PC_COMM_UART, "Checking SIM Status....\r\n", nb_iot_uart.name);
    error_handler = send_check_message(&NB_IOT_UART, "AT\r\n", "OK\r\n", &nb_iot_uart, 300);
    if(1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s",(char *) nb_iot_uart.raw_data);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        return 1;
    }
    else if(0 == error_handler)
    {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s",(char *) nb_iot_uart.raw_data);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    }
    else
    {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}
/*
 *  Preferred Mode Selection
 *  2 - Automatic
 *  13 - GSM Only
 *  38 - LTE Only
 *  51 - GSM and LTE
 */
uint8_t nb_set_mode(const uint8_t mode) {
    uint8_t error_handler = 0;
    char command[UART_TRANSMIT_MAX];

    sprintf(command, "AT+CNMP=%d\r\n", mode);
    uart_send_message(&PC_COMM_UART, "Setting Mode to NB-IoT....\r\n", nb_iot_uart.name);
    error_handler = send_check_message(&NB_IOT_UART, command, "OK\r\n", &nb_iot_uart, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", (char *) nb_iot_uart.raw_data);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", (char *) nb_iot_uart.raw_data);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

/*
 * Preferred Selection between CAT-M and NB-IOT
 * 1 - CAT-M
 * 2 - NB-IoT
 * 3 - CAT-M & NB-IOT
 */
uint8_t nb_set_mode_selection(const uint8_t mode)
{
    uint8_t error_handler;
    char command[UART_TRANSMIT_MAX];

    sprintf(command, "AT+CMNB=%d\r\n", mode);
    error_handler = send_check_message(&NB_IOT_UART, command, "OK\r\n", &nb_iot_uart, 300);
    if (1 == error_handler)
    {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s",(char *) nb_iot_uart.raw_data);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        return 1;
    }
    else if(0 == error_handler)
    {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s",(char *) nb_iot_uart.raw_data);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    }
    else
    {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_make_standard_init()
{
    uint8_t sim_tries = 0;
    const uint8_t MAX_SIM_TRIES = 3;
    while (sim_tries < MAX_SIM_TRIES)
    {
        sim_tries += 1;
        if(1 == nb_check_sim_status())
        {
            break;
        }
        if (sim_tries == MAX_SIM_TRIES)
            return 0;
        HAL_Delay(100);
    }
    HAL_Delay(100);
    if(0 == nb_set_mode(38))
        return 0;
    HAL_Delay(100);

    if(0 == nb_set_mode_selection(2))
        return 0;
}