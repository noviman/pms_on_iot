//
// Created by bartek on 3/22/20.
//
#include "pc_comm.h"
#include "pms7003.h"

void pc_comm_rx_callback()
{
    pc_uart.flag = 0;
    switch(pc_uart.raw_data[0]) {
        case 0x01:
            pm_sensor_host_tx(pm_sensor_changeSt_sleep);
            break;
        case 0x02:
            pm_sensor_host_tx(pm_sensor_changeSt_wakeup);
            break;
        case 0x03:
            pm_sensor_host_tx(pm_sensor_changeM_active);
            break;
        case 0x04:
            pm_sensor_host_tx(pm_sensor_changeM_passive);
            break;
        case 0x05:
            pm_sensor_host_tx(pm_sensor_req_read);
            break;
        default:
            uart_send_message(&PC_COMM_UART, (char *) pc_uart.raw_data);
            break;
    }
}