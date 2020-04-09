//
// Created by bartek on 3/20/20.
//

#ifndef SENSOR_PM_PMS7003_H
#define SENSOR_PM_PMS7003_H

#include "main.h"

#define MAX_FRAME_LEN 32

extern const char TAG[10];
extern const uint8_t PM_SEND_TO_IOT;
extern volatile uint8_t pm_ready_to_nb_transmit_flag;
typedef struct PMS7003_struct {
    uint8_t  frameHeader[2];
    uint16_t frameLen;
    uint16_t PM1_0_CF1;
    uint16_t PM2_5_CF1;
    uint16_t PM10_0_CF1;
    uint16_t PM1_0_amb;
    uint16_t PM2_5_amb;
    uint16_t PM10_0_amb;
    uint16_t rawGt0_3um;
    uint16_t rawGt0_5um;
    uint16_t rawGt1_0um;
    uint16_t rawGt2_5um;
    uint16_t rawGt5_0um;
    uint16_t rawGt10_0um;
    uint16_t  probe_count;
    uint16_t checksum;
} PMS7003_struct;

static PMS7003_struct pm_sensor;

extern uint8_t pm_sensor_tx_frame[7];
extern uint8_t pm_sensor_changeM_passive[4];
extern uint8_t pm_sensor_changeM_active[4];
extern uint8_t pm_sensor_changeSt_sleep[4];
extern uint8_t pm_sensor_changeSt_wakeup[4];
extern uint8_t pm_sensor_req_read[4];
extern uint8_t pm_sensor_changeSt_wakeup_standby[4];

uint8_t pm_sensor_update_data(const uint8_t *);
uint16_t pm_sensor_validate_checksum(const uint8_t *);
void pm_sensor_rx_callback();
void pm_sensor_read_cycl();
void pm_sensor_transmit_callback();
void pm_sensor_host_tx(const uint8_t *);
#endif //SENSOR_PM_PMS7003_H
