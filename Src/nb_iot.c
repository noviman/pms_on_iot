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

char * remove_req_from_read(const char * mes_recv)
{
    char *ptr = strchr(mes_recv, '\n') + 1;
    return ptr;
}

uint8_t send_check_message(UART_HandleTypeDef * handle,
        const char * mes_send,
        const char * mes_check,
        uart_struct *uart_struct_handle)
{
    if(1 == uart_struct_handle->rx_flag)
    {
        uart_struct_handle->rx_flag = 0;
    }

    uart_send_message(handle, mes_send, NULL);
//    while(uart_struct_handle->rx_flag == 0) { /* Infinite Loop. Need to implement Timeout in future  */ }
    uart_struct_handle->rx_flag = 0;
    if(0 != strcmp( remove_req_from_read( (char *)uart_struct_handle->raw_data), mes_check) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
uint8_t nb_check_sim_status()
{
    uart_send_message(&PC_COMM_UART, "Checking SIM Status....\r\n", nb_iot_uart.name);
    if(send_check_message(&NB_IOT_UART, "AT\r\n", "OK\r\n", &nb_iot_uart)) {
        uart_send_message(&PC_COMM_UART, "OK - Message as expected\r\n", nb_iot_uart.name);
    }
    else
        uart_send_message(&PC_COMM_UART, remove_req_from_read( (char *) nb_iot_uart.raw_data), nb_iot_uart.name);

    return 1;
}