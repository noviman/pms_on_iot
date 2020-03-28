//
// Created by bartek on 3/28/20.
//
#include "nb_iot.h"
#include "uart_comm.h"

void nb_iot_receive_callback()
{
    char message[100];
    sprintf(message, "Hello WORLD !: %s", nb_iot_raw_data);
    uart_send_message(&NB_IOT_UART, message);
}
