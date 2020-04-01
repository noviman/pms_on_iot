//
// Created by bartek on 3/22/20.
//
#include "pc_comm.h"
#include "pms7003.h"
#include "string.h"
#include "stdlib.h"

void pc_comm_rx_callback()
{
    pc_uart.rx_flag = 0;
    pc_comm_handle_command( (char *) pc_uart.raw_data);
}

uint8_t pms_commands(const char * command)
{
    if ( 0 == strcmp(command, "SLEEP") )
    {
        pm_sensor_host_tx(pm_sensor_changeSt_sleep);
    }
    else if ( 0 == strcmp(command, "WAKEUP") )
    {
        pm_sensor_host_tx(pm_sensor_changeSt_wakeup);
    }
    else if ( 0 == strcmp(command, "ACTIVE") )
    {
        pm_sensor_host_tx(pm_sensor_changeM_active);
    }
    else if ( 0 == strcmp(command, "PASSIVE") )
    {
        pm_sensor_host_tx(pm_sensor_changeM_passive);
    }
    else if ( 0 == strcmp(command, "READ") )
    {
        pm_sensor_host_tx(pm_sensor_req_read);
    }
    else
    {
        return 0;
    }
    return 1;
}
uint8_t pc_comm_handle_command(const char * command)
{
    char command_buffer[UART_RECEIVE_MAX];
    char command_tx_buffer[UART_TRANSMIT_MAX];
    int8_t chosen_interface = 0;
    uint8_t ret_sscanf = 0;

    memcpy(command_buffer, command, strlen(command)); // Copying buffer, because sscanf works on pointer directly
    ret_sscanf = sscanf(command_buffer, "%d;%s", &chosen_interface, command_tx_buffer); //
    if(2 != ret_sscanf)
    {
        return 0;
    }
    char *string_with_newline = add_newline_to_message(command_tx_buffer);
    switch(chosen_interface)
    {
        // PMS
        case 0:
            // Resend to PC Uart and Send full command to NB_IOT Uart
            uart_send_message(&NB_IOT_UART, string_with_newline, NULL);
            uart_send_message(&PC_COMM_UART, string_with_newline, nb_iot_uart.name);
            break;
        // NB_IOT
        case 1:
            if (0 == pms_commands(command_tx_buffer)) {
                sprintf(command_tx_buffer, "Wrong CMD Chosen."
                                           "Available:\tSLEEP\tWAKEUP\tACTIVE\tPASSIVE\tREAD\r\n");
                uart_send_message(&PC_COMM_UART, command_tx_buffer, pms_uart.name);
            }
            else
            {
                uart_send_message(&PC_COMM_UART, string_with_newline, pms_uart.name);
            }
            break;
        default:
            sprintf(command_tx_buffer, "Wrong ID Chosen. 0 - NB_IOT\t 1- PMS\r\n");
            uart_send_message(&PC_COMM_UART, command_tx_buffer, pc_uart.name);
            break;
    }
    free(string_with_newline); // Freeing allocated mem
}