//
// Created by bartek on 3/20/20.
//

#include "pms7003.h"

uint8_t sensor_new_data_flag = 0;
PMS7003_struct pm_sensor = { }; // Initialize with 0 every elem
uint8_t update_data_struct(const uint8_t * raw_data)
{
    uint16_t calc_checksum = validate_checksum(raw_data);
    if (!calc_checksum)
        return 0;
    pm_sensor.frameHeader[0] = raw_data[0];
    pm_sensor.frameHeader[1] = raw_data[1];
    pm_sensor.frameLen = MAX_FRAME_LEN;
    pm_sensor.PM1_0_CF1     = (raw_data[4] << 8)    | raw_data[5];
    pm_sensor.PM2_5_CF1     = (raw_data[6] << 8)    | raw_data[7];
    pm_sensor.PM10_0_CF1    = (raw_data[8] << 8)    | raw_data[9];
    pm_sensor.PM1_0_amb     = (raw_data[10] << 8)   | raw_data[11];
    pm_sensor.PM2_5_amb     = (raw_data[12] << 8)   | raw_data[13];
    pm_sensor.PM10_0_amb    = (raw_data[14] << 8)   | raw_data[15];
    pm_sensor.rawGt0_3um    = (raw_data[16] << 8)   | raw_data[17];
    pm_sensor.rawGt0_5um    = (raw_data[18] << 8)   | raw_data[19];
    pm_sensor.rawGt1_0um    = (raw_data[20] << 8)   | raw_data[21];
    pm_sensor.rawGt2_5um    = (raw_data[22] << 8)   | raw_data[23];
    pm_sensor.rawGt5_0um    = (raw_data[24] << 8)   | raw_data[25];
    pm_sensor.rawGt10_0um   = (raw_data[26] << 8)   | raw_data[27];
    pm_sensor.checksum      = (raw_data[30] << 8)   | raw_data[31];
    pm_sensor.probe_count   += 1;
    return 1;
}

uint16_t validate_checksum(const uint8_t * raw_data)
{
    uint16_t calc_sum = 0;
    uint16_t check_code = (raw_data[30] << 8) | raw_data[31];
    for(uint8_t i = 0; i < 32 - 2; i++) // -2 because of 2 check bytes at the end
        calc_sum += raw_data[i];

    return calc_sum & check_code;
}
