//
// Created by bartek on 3/20/20.
//

#include "pms7003.h"
#include "uart_comm.h"
#include "nb_iot.h"
#include <string.h>
// TODO string.h to remove after refactor of nb_send_message and add queue

const char TAG[10] = "PMS7003";
const uint8_t PM_MAX_COUNT_READ = 10;
// Turn this flag to 0 if dont want to send to IOT
const uint8_t PM_SEND_TO_IOT = 1;
volatile uint8_t pm_ready_to_nb_transmit_flag = 0;

uint8_t pm_sensor_tx_frame[7] = {0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00};
// Last Byte decides if Answer is expected
uint8_t pm_sensor_changeM_passive[4] = {0xE1, 0x00, 0x00, 0x01};
uint8_t pm_sensor_changeM_active[4] = {0xE1, 0x00, 0x01, 0x01};
uint8_t pm_sensor_changeSt_sleep[4] = {0xE4, 0x00, 0x00, 0x01};
uint8_t pm_sensor_changeSt_wakeup[4] = {0xE4, 0x00, 0x01, 0x00};
uint8_t pm_sensor_changeSt_wakeup_standby[4] = {0xE4, 0x00, 0x01, 0x01};
uint8_t pm_sensor_req_read[4] = {0xE2, 0x00, 0x01, 0x00};

static PMS7003_struct pm_sensor = { }; // Initialize with 0 every elem

uint8_t pm_sensor_update_data(const uint8_t *raw_data)
{
    uint16_t calc_checksum = pm_sensor_validate_checksum(raw_data);
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

uint16_t pm_sensor_validate_checksum(const uint8_t * raw_data)
{
    uint16_t calc_sum = 0;
    uint16_t check_code = (raw_data[30] << 8) | raw_data[31];
    for(uint8_t i = 0; i < 32 - 2; i++) // -2 because of 2 check bytes at the end
        calc_sum += raw_data[i];

    return calc_sum & check_code;
}

void pm_sensor_rx_callback()
{
    char mes_to_pc[200] = { 0 };
    pms_uart.rx_flag = 0; // Flag Clear

    if (pms_uart.data_length == MAX_FRAME_LEN) {
        if (pm_sensor_update_data(pms_uart.raw_data_rx)) {
            sprintf(mes_to_pc, "%d:PM2.5 Ambient: %d\tPM10 Ambient: %d. Checksum: %d\n\r",
                    pm_sensor.probe_count,
                    pm_sensor.PM2_5_amb,
                    pm_sensor.PM10_0_amb,
                    pm_sensor.checksum);
            if(PM_SEND_TO_IOT == 1)
            {
                pm_sensor_read_cycl();
            }
        } else {
            sprintf(mes_to_pc, "Checksum is not correct.Full Frame;\n\r");
            for (uint16_t i = 0; i < pms_uart.data_length; i++)
                sprintf(mes_to_pc, "%s 0x%02X", mes_to_pc, pms_uart.raw_data_rx[i]);
            sprintf(mes_to_pc, "%s\n\r", mes_to_pc);
        }
    } else{
        sprintf(mes_to_pc, "Full Frame;\n\r");
        for (uint16_t i = 0; i < pms_uart.data_length; i++)
            sprintf(mes_to_pc, "%s 0x%02X", mes_to_pc, pms_uart.raw_data_rx[i]);
        sprintf(mes_to_pc, "%s\n\r", mes_to_pc);
    }
    uart_send_message(&PC_COMM_UART, mes_to_pc, pms_uart.name);
}

void pm_sensor_host_tx(const uint8_t * frame)
{
    uint16_t checksum = 0x0000;
    char message[UART_TRANSMIT_MAX];
    // Assign Bytes to Send Buff
    for(uint8_t i = 0; i < 3; i++)
        pm_sensor_tx_frame[i+2] = frame[i];

    // Calculate checksum
    for (uint8_t i = 0; i < 5; i++)
        checksum += pm_sensor_tx_frame[i];

    pm_sensor_tx_frame[5] = (uint8_t)((checksum & 0xFF00) >> 8);
    pm_sensor_tx_frame[6] = (uint8_t)(checksum & 0x00FF);

    // Transmit Message
    HAL_UART_Transmit_DMA(&PM_SENSOR_UART, pm_sensor_tx_frame, 7);
}

void pm_sensor_read_cycl()
{
    if(pm_sensor.probe_count >= PM_MAX_COUNT_READ)
    {
        pm_ready_to_nb_transmit_flag = 1;
        pm_sensor.probe_count = 0;
        pm_sensor_host_tx(pm_sensor_changeSt_sleep);
    }

}

void pm_sensor_transmit_callback()
{
    pm_ready_to_nb_transmit_flag = 0;
    memset(pms_uart.raw_data_tx_buffer, 0, UART_TRANSMIT_MAX);
    sprintf(pms_uart.raw_data_tx_buffer, "{\"k\":\"%s\",\"d\":\"PM2.5:%d;PM10:%d\",\"t\":\"%s\"}\r\n", DEVICE_KEY, pm_sensor.PM2_5_amb,
            pm_sensor.PM10_0_amb, TAG);
    nb_send_message(pms_uart.raw_data_tx_buffer);
}
