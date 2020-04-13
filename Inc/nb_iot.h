//
// Created by bartek on 3/28/20.
//

#ifndef SENSOR_PM_NB_IOT_H
#define SENSOR_PM_NB_IOT_H
#include "stdio.h"

#define NB_IOT_MAX 100

#define SIM_READINESS 1 << 0
#define DEVICE_CONNECTION 1 << 1
#define GET_IP 1 << 2
#define MODE_SELECTION 1 << 3
#define MODE 1 << 4
#define APN 1 << 5
#define IP_APN 1 << 6
#define NETWORK 1 << 7
#define BRING_CSD_CONN 1 << 8
#define IOT_ALL (SIM_READINESS | DEVICE_CONNECTION | GET_IP | MODE_SELECTION | MODE | APN | IP_APN | NETWORK | BRING_CSD_CONN)
#define IOT_READY 1 << 9


/* State map:
xxxx xxxx xxxx xxx1 - GET_SIM_READINESS
xxxx xxxx xxxx xx1x - GET_DEVICE_CONNECTION
xxxx xxxx xxxx x1xx - GET_IP
xxxx xxxx xxxx 1xxx - SET_MODE_SELECTION
xxxx xxxx xxx1 xxxx - SET_MODE
xxxx xxxx xx1x xxxx - SET_APN
xxxx xxxx x1xx xxxx - SET_IP_APN
xxxx xxxx 1xxx xxxx - SET_NETWORK
xxxx xxx1 xxxx xxxx - SET_BRING_CSD_CONN
xxxx xxx1x xxxx xxxx - SET_BRING_CSD_CONN

Whole State is in variable 'nb_iot_state'
*/


enum message_consistency { Message_Expected, Message_not_expected };
extern const char DEVICE_KEY[10];
extern volatile uint16_t nb_iot_state;
extern volatile uint8_t nb_tim_flag;

void nb_iot_rx_callback();
uint8_t nb_check_sim_status();
uint8_t nb_set_mode(const uint8_t);
uint8_t nb_set_mode_selection(const uint8_t);
uint8_t nb_set_apn();
uint8_t nb_set_ip_apn(const char *);
uint8_t nb_set_network(const char *);
uint8_t nb_deactivate_gprs_pdp_context();
uint8_t nb_check_device_connection();
uint8_t nb_check_register();
uint8_t nb_bring_up_csd_connection();
uint8_t nb_get_ip();
uint8_t nb_open_connection();
uint8_t nb_send_message(const char *message);
uint8_t nb_make_standard_init();
uint8_t nb_make_settings_validate();

#endif //SENSOR_PM_NB_IOT_H
