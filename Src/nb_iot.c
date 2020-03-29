//
// Created by bartek on 3/28/20.
//
#include "nb_iot.h"
#include "uart_comm.h"

void nb_iot_rx_callback()
{
    char message[100];
    nb_iot_uart.flag = 0;
    sprintf(message, "'%s'", nb_iot_uart.raw_data);
    uart_send_message(&NB_IOT_UART, message);

}
