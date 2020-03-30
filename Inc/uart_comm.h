//
// Created by bartek on 3/18/20.
//

#ifndef SENSOR_PM_UART_COMM_H
#define SENSOR_PM_UART_COMM_H

#include "main.h"

#define PC_COMM_UART huart2
#define PM_SENSOR_UART huart3
#define NB_IOT_UART huart1

#define PC_COMM_RECEIVE_MAX 255
#define PM_SENSOR_RECEIVE_MAX 32
#define NB_IOT_RECEIVE_MAX 255
#define MAX_NAME_UART 10

typedef struct pc_uart_struct
{
    char name[MAX_NAME_UART];
    uint16_t data_length;
    uint8_t rx_flag;
    uint8_t raw_data[PC_COMM_RECEIVE_MAX];
    uint8_t raw_data_buffer[PC_COMM_RECEIVE_MAX];
} pc_uart_struct;

typedef struct pms_uart_struct
{
    char name[MAX_NAME_UART];
    uint16_t data_length;
    uint8_t rx_flag;
    uint8_t raw_data[PM_SENSOR_RECEIVE_MAX];
    uint8_t raw_data_buffer[PM_SENSOR_RECEIVE_MAX];
} pms_uart_struct;

typedef struct nb_iot_uart_struct
{
    char name[MAX_NAME_UART];
    uint16_t data_length;
    uint8_t rx_flag;
    uint8_t raw_data[NB_IOT_RECEIVE_MAX];
    uint8_t raw_data_buffer[NB_IOT_RECEIVE_MAX];
} nb_iot_uart_struct;

extern pc_uart_struct pc_uart;
extern pms_uart_struct pms_uart;
extern nb_iot_uart_struct nb_iot_uart;

void start_dma_uart_rx(void);
HAL_StatusTypeDef uart_send_message(UART_HandleTypeDef *handle, const char *message, const char *receiver);
// Idle Detection
void IDLE_DETECT_UART_IRQHandler(UART_HandleTypeDef *);
void IDLE_UART_Callback(UART_HandleTypeDef *);
#endif //SENSOR_PM_UART_COMM_H