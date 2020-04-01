//
// Created by bartek on 3/28/20.
//

#ifndef SENSOR_PM_NB_IOT_H
#define SENSOR_PM_NB_IOT_H
#include "stdio.h"

void nb_iot_rx_callback();
uint8_t nb_check_sim_status();
uint8_t nb_set_mode(const uint8_t);
uint8_t nb_set_mode_selection(const uint8_t);
uint8_t nb_make_standard_init();

#endif //SENSOR_PM_NB_IOT_H
