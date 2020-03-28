//
// Created by bartek on 3/22/20.
//
#include "pc_comm.h"
#include "pms7003.h"

uint8_t pc_comm_rx_flag = 0;

void pc_comm_rx_callback()
{
    char mes_ret;
    pc_comm_rx_flag = 0;
    switch(pc_comm_raw_data[0]) {
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
            mes_ret = 0xFF;
            uart_send_message(&PC_COMM_UART, &mes_ret);
            break;
    }
}