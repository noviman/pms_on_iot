//
// Created by bartek on 3/20/20.
//

#include "pms7003.h"

#define MAX_FRAME_LEN = 32;

struct PMS7003_framestruct {
    uint8_t  frameHeader[2];
    uint16_t frameLen = MAX_FRAME_LEN;
    uint16_t concPM1_0_CF1;
    uint16_t concPM2_5_CF1;
    uint16_t concPM10_0_CF1;
    uint16_t concPM1_0_amb;
    uint16_t concPM2_5_amb;
    uint16_t concPM10_0_amb;
    uint16_t rawGt0_3um;
    uint16_t rawGt0_5um;
    uint16_t rawGt1_0um;
    uint16_t rawGt2_5um;
    uint16_t rawGt5_0um;
    uint16_t rawGt10_0um;
    uint8_t  version;
    uint8_t  errorCode;
    uint16_t checksum;
} thisFrame;

uint8_t sensor_new_data_flag = 0;
uint8_t validate_checksum(const uint8_t * sensor_data)
{
    uint8_t sum = 0;
    for(uint8_t i = 0; i < 32 - 2; i++)
    {
        sum += sensor_data[i];
    }
    return sum;
}

uint8_t get_sensor_data_size (const uint8_t * sensor_data)
{
    return sensor_data[2] + sensor_data[3];
}

uint8_t get_checksum(const uint8_t * sensor_data)
{
    return (sensor_data[30] << 8) + sensor_data[31];
}