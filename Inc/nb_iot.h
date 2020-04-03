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
uint8_t nb_check_device_connection();
uint8_t nb_deactivate__gprs_pdp_context();
uint8_t nb_set_apn();
uint8_t nb_bring_up_csd_connection();
uint8_t nb_get_ip();
uint8_t nb_check_register();
uint8_t nb_open_connection();
uint8_t nb_send_message();
uint8_t nb_make_standard_init();

#endif //SENSOR_PM_NB_IOT_H
