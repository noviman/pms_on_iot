//
// Created by bartek on 3/22/20.
//
#include "pc_comm.h"
#include "pms7003.h"
#include "string.h"
#include "stdlib.h"
#include "nb_iot.h"
#include "helpers.h"

void pc_comm_rx_callback()
{
    pc_uart.rx_flag = 0;
    pc_comm_handle_command( (char *) pc_uart.raw_data_rx);
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
    else if ( 0 == strcmp(command, "WAKEUPSTB") )
    {
        pm_sensor_host_tx(pm_sensor_changeSt_wakeup_standby);
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
uint8_t nb_commands(const char * command)
{
    if ( 0 == strcmp(command, "CHECKSIM") )
    {
        nb_check_sim_status();
        return 1;
    }
    else if ( 0 == strcmp(command, "INIT") )
    {
//        nb_make_standard_init();
        nb_make_settings_validate();
        return 1;
    }
    else if ( 0 == strcmp(command, "SETMODE") )
    {
        nb_set_mode(38);
        return 1;
    }
    else if ( 0 == strcmp(command, "SETMODESEL") )
    {
        nb_set_mode_selection(1);
        return 1;
    }
    else if ( 0 == strcmp(command, "CHECKCONN") )
    {
        nb_check_device_connection();
        return 1;
    }
    else if ( 0 == strcmp(command, "PDPDEACT") )
    {
        nb_deactivate_gprs_pdp_context();
        return 1;
    }
    else if ( 0 == strcmp(command, "SETAPN") )
    {
        nb_set_apn();
        return 1;
    }
    else if ( 0 == strcmp(command, "BRINGCSD") )
    {
        nb_bring_up_csd_connection();
        return 1;
    }
    else if ( 0 == strcmp(command, "GETIP") )
    {
        nb_get_ip();
        return 1;
    }
    else if ( 0 == strcmp(command, "CREG") )
    {
        nb_check_register();
        return 1;
    }
    else if ( 0 == strcmp(command, "OPENCONN") )
    {
        nb_open_connection();
        return 1;
    }
    else if ( 0 == strcmp(command, "SENDMESSAGE") )
    {
        char message[100];
        sprintf(message, "{\"k\":\"%s\",\"d\":\"RazDwaTri\",\"t\":\"%s\"}\r\n", DEVICE_KEY, TAG);
        nb_send_message(message);
        return 1;
    }
    else if( (command[0] == 'A' && command[1] == 'T' && command[2] == ';') )
    {
        char * command_with_r_n = malloc(sizeof(char) * (strlen(&command[3]+3) ) );
        sprintf(command_with_r_n, "%s\r\n", &command[3] );
        uart_send_message(&NB_IOT_UART, command_with_r_n, NULL);
        free(command_with_r_n);
        return 1;
    }
    return 0;
}

uint8_t pc_comm_handle_command(const char * command)
{
    char command_tx_buffer[UART_TRANSMIT_MAX];
    int8_t chosen_interface = 0;
    uint8_t ret_sscanf = 0;

    ret_sscanf = sscanf(command, "%d;%s", &chosen_interface, command_tx_buffer); //
    if(2 != ret_sscanf)
    {
        return 0;
    }
    switch(chosen_interface)
    {
        // NB-ioT
        case 0:
            // Resend to PC Uart and Send full command to NB_IOT Uart
            if (0 == nb_commands(command_tx_buffer)) {
                sprintf(command_tx_buffer, "Wrong CMD Chosen.\r\n"
                                           "CHECKSIM/INIT/SETMODE/SETMODESEL/CHECKCONN/PDPDEACT/SETAPN/BRINGCSD/GETIP/CREG\r\n");
                uart_send_message(&PC_COMM_UART, command_tx_buffer, nb_iot_uart.name);
            }
            break;
        // PMS
        case 1:
            if (0 == pms_commands(command_tx_buffer)) {
                sprintf(command_tx_buffer, "Wrong CMD Chosen."
                                           "Available:\tSLEEP\tWAKEUP\tACTIVE\tPASSIVE\tREAD\r\n");
                uart_send_message(&PC_COMM_UART, command_tx_buffer, pms_uart.name);
            }
            break;
        default:
            sprintf(command_tx_buffer, "Wrong ID Chosen. 0 - NB_IOT\t 1- PMS\r\n");
            uart_send_message(&PC_COMM_UART, command_tx_buffer, pc_uart.name);
            break;
    }
}
