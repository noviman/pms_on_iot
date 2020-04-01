//
// Created by bartek on 3/22/20.
//

#ifndef SENSOR_PM_PC_COMM_H
#define SENSOR_PM_PC_COMM_H

#include "uart_comm.h"

void pc_comm_rx_callback();
uint8_t pc_comm_handle_command(const char * );
uint8_t pms_commands(const char * command);
uint8_t nb_commands(const char * command);


#endif //SENSOR_PM_PC_COMM_H
