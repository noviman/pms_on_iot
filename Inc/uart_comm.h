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
#define NB_IOT_RECEIVE_MAX 1024

extern uint8_t pc_comm_raw_data[PC_COMM_RECEIVE_MAX];
extern uint8_t pm_sensor_raw_data[PM_SENSOR_RECEIVE_MAX];
extern uint8_t nb_iot_raw_data_buffer[NB_IOT_RECEIVE_MAX];
extern uint8_t nb_iot_raw_data[NB_IOT_RECEIVE_MAX];


HAL_StatusTypeDef uart_send_message(UART_HandleTypeDef * , const char * );
// Idle Detection
void IDLE_DETECT_UART_IRQHandler(UART_HandleTypeDef *);
void IDLE_UART_Callback(UART_HandleTypeDef *);
#endif //SENSOR_PM_UART_COMM_H