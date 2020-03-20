//
// Created by bartek on 3/20/20.
//

#ifndef SENSOR_PM_PMS7003_H
#define SENSOR_PM_PMS7003_H

#include "main.h"

#define PMS_ACTIVEMODE 1
extern uint8_t sensor_new_data_flag;
uint8_t validate_checksum(const uint8_t *);
uint8_t get_sensor_data_size (const uint8_t * );
uint8_t get_checksum(const uint8_t * );
#endif //SENSOR_PM_PMS7003_H
