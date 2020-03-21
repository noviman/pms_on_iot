//
// Created by bartek on 3/20/20.
//

#ifndef SENSOR_PM_PMS7003_H
#define SENSOR_PM_PMS7003_H

#include "main.h"

#define PMS_ACTIVEMODE 1
#define MAX_FRAME_LEN 32

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

extern PMS7003_struct pm_sensor;
extern uint8_t sensor_new_data_flag;

uint8_t update_data_struct(const uint8_t *);
uint16_t validate_checksum(const uint8_t *raw_data);

#endif //SENSOR_PM_PMS7003_H
