//
// Created by bartek on 3/28/20.
//
#include "nb_iot.h"
#include "uart_comm.h"
#include "string.h"
#include "stdlib.h"


const char DEVICE_KEY[10] = "L^R<8oyf";
volatile uint16_t nb_iot_state = 0;
volatile uint8_t nb_tim_flag = 0;

void nb_iot_rx_callback() {
    nb_iot_uart.rx_flag = 0;
    uart_send_message(&PC_COMM_UART, nb_iot_uart.raw_data_rx, nb_iot_uart.name);
}

uint8_t nb_make_settings_validate()
{
    if(0 == (nb_iot_state & SIM_READINESS) )
    {
        uint8_t sim_tries = 0;
        const uint8_t MAX_SIM_TRIES = 3;
        while (sim_tries < MAX_SIM_TRIES) {
            sim_tries += 1;
            if (1 == nb_check_sim_status()) {
                break;
            }
            if (sim_tries == MAX_SIM_TRIES)
                return 0;
        }
        HAL_Delay(100);
    }

    if(0 == (nb_iot_state & MODE) )
    {
        nb_set_mode(38);
        HAL_Delay(100);
    }

    if(0 == (nb_iot_state & MODE_SELECTION) )
    {
        nb_set_mode_selection(3);
        HAL_Delay(100);
    }


    if(0 == (nb_iot_state & DEVICE_CONNECTION) )
    {
        // Shut PDP Context
        nb_deactivate_gprs_pdp_context();
        HAL_Delay(100);

        nb_check_device_connection();
        HAL_Delay(100);
    }

    if(0 == (nb_iot_state & APN) )
    {
        nb_set_apn();
        HAL_Delay(100);
    }

    if(0 == (nb_iot_state & IP_APN) )
    {
        nb_set_ip_apn("1,\"IP\",\"hologram\"");
        HAL_Delay(100);
    }

    if(0 == (nb_iot_state & NETWORK) )
    {
        nb_set_network("1,2,\"26003\",7");
        HAL_Delay(100);
    }

    if(0 == (nb_iot_state & BRING_CSD_CONN) )
    {
        nb_bring_up_csd_connection();
        HAL_Delay(100);
    }

    if(0 == (nb_iot_state & GET_IP) )
    {
        nb_get_ip();
        HAL_Delay(100);
    }

    if(nb_iot_state & IOT_ALL)
    {
        nb_iot_state |= IOT_READY;
    }
}

uint8_t nb_make_standard_init() {
    uint8_t sim_tries = 0;
    const uint8_t MAX_SIM_TRIES = 3;
    while (sim_tries < MAX_SIM_TRIES) {
        sim_tries += 1;
        if (1 == nb_check_sim_status()) {
            break;
        }
        if (sim_tries == MAX_SIM_TRIES)
            return 0;
        HAL_Delay(100);
    }
    HAL_Delay(100);
    if (0 == nb_set_mode(38))
        return 0;
    HAL_Delay(100);

    if (0 == nb_set_mode_selection(3))
        return 0;
    HAL_Delay(100);

    if (0 == nb_check_device_connection()) {
        HAL_Delay(100);
        // If Status NOK, try to Shut PDP Context
        if (0 == nb_deactivate_gprs_pdp_context())
            return 0;
        HAL_Delay(100);
        // Second Try
        if (0 == nb_check_device_connection())
            return 0;
    }
    HAL_Delay(100);
    if (0 == nb_set_apn())
        return 0;

    HAL_Delay(200);
    if (0 == nb_set_ip_apn("1,\"IP\",\"hologram\""))
        return 0;

    HAL_Delay(100);
    if (0 == nb_set_network("1,2,\"26003\",7"))
        return 0;

    HAL_Delay(100);
    if (0 == nb_bring_up_csd_connection())
        return 0;

    HAL_Delay(300);
    if (0 == nb_get_ip())
        return 0;
}

uint8_t nb_check_sim_status() {
    uint8_t error_handler = 0;

    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT\r\n");
    uart_send_message(&PC_COMM_UART, "Checking SIM Status....\r\n", nb_iot_uart.name);
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= SIM_READINESS;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
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

    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CNMP=%d\r\n", mode);
    uart_send_message(&PC_COMM_UART, "Setting Mode to NB-IoT....\r\n", nb_iot_uart.name);
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= MODE;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
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
uint8_t nb_set_mode_selection(const uint8_t mode) {
    uint8_t error_handler;
    uart_send_message(&PC_COMM_UART, "Setting preffered mode to NB-IoT....\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CMNB=%d\r\n", mode);
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= MODE_SELECTION;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_check_device_connection() {
    uint8_t error_handler;

    uart_send_message(&PC_COMM_UART, "Checking device connection...\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CIPSTATUS\r\n");
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n\r\nSTATE: IP INITIAL\r\n",
                                       &nb_iot_uart, Message_Expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= DEVICE_CONNECTION;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_deactivate_gprs_pdp_context() {
    uint8_t error_handler;
    uart_send_message(&PC_COMM_UART, "Shutting down GPRS PDP Context\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CIPSHUT\r\n");
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "SHUT OK\r\n", &nb_iot_uart,
                                       Message_Expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_set_network(const char *network) {
    uint8_t error_handler;
    uart_send_message(&PC_COMM_UART, "Setting network to Orange...\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+COPS=%s\r\n", network);
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 15000);
    if (1 == error_handler) {
        0;
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= NETWORK;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_set_ip_apn(const char *apn) {
    uint8_t error_handler;
    uart_send_message(&PC_COMM_UART, "Setting IP APN to 'hologram'\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CGDCONT=%s\r\n", apn);
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 1000);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= IP_APN;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_set_apn() {
    uint8_t error_handler;

    uart_send_message(&PC_COMM_UART, "Setting APN to 'hologram'\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CSTT=\"hologram\"\r\n");
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= APN;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_bring_up_csd_connection() {
    uint8_t error_handler;

    uart_send_message(&PC_COMM_UART, "Bringing up CSD Connection'\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CIICR\r\n");
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= BRING_CSD_CONN;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_get_ip() {
    uint8_t error_handler;

    uart_send_message(&PC_COMM_UART, "Getting IP...'\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CIFSR\r\n");
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_not_expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        nb_iot_state |= GET_IP;
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_check_register() {
    uint8_t error_handler;

    uart_send_message(&PC_COMM_UART, "Checking Registration...\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CREG?\r\n");
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 300);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_open_connection()
{
    uint8_t error_handler;
    char type[5] = "TCP";
    char host[30] = "cloudsocket.hologram.io";
    uint16_t port = 9999;

    uart_send_message(&PC_COMM_UART, "Opening connection...\r\n", nb_iot_uart.name);
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CIPSTART=\"%s\",\"%s\",%d\r\n", type, host, port);
    error_handler = send_check_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, "OK\r\n", &nb_iot_uart,
                                       Message_Expected, 1500);
    if (1 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_OK]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
        return 1;
    } else if (0 == error_handler) {
        char message[UART_TRANSMIT_MAX];
        sprintf(message, "[CMD_RX_FAIL]%s", nb_iot_uart.raw_data_rx);
        uart_send_message(&PC_COMM_UART, message, nb_iot_uart.name);
    } else {
        uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
    }
    return 0;
}

uint8_t nb_send_message(const char *message)
{
    uint16_t timeout = 5000;
    uint8_t error_handler = 0;
    if(0 == nb_open_connection())
        return 0;

    nb_iot_uart.tim_counter = timeout;
    while(0 != strcmp(nb_iot_uart.raw_data_rx, "\r\nCONNECT OK\r\n") )
    {
        if(nb_iot_uart.tim_counter == 0)
        {
            uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
            return 2;
        }
    }
    sprintf(nb_iot_uart.raw_data_tx_buffer, "AT+CIPSEND=%d\r\n", strlen(message));
    uart_send_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, NULL);

    char expected_message[UART_RECEIVE_MAX];
    sprintf(expected_message, "AT+CIPSEND=%d\r\r\n> ",strlen(message));
    nb_iot_uart.tim_counter = timeout;
    while( 0 != strcmp(nb_iot_uart.raw_data_rx, expected_message) )
    {
        if(nb_iot_uart.tim_counter == 0)
        {
            uart_send_message(&PC_COMM_UART, "Timeout exceed without response!\r\n", nb_iot_uart.name);
            return 2;
        }
    }
    // memset(nb_iot_uart.raw_data_tx_buffer, 0, UART_TRANSMIT_MAX);
    // sprintf(nb_iot_uart.raw_data_tx_buffer, "%s", message);
    memcpy(nb_iot_uart.raw_data_tx_buffer, message, strlen(message));
    uart_send_message(&NB_IOT_UART, nb_iot_uart.raw_data_tx_buffer, NULL);

}