//
// Created by bartek on 3/18/20.
//

#ifndef SENSOR_PM_UART_COMM_H
#define SENSOR_PM_UART_COMM_H
#include "main.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"

#define PC_COMM_UART huart2
#define PM_SENSOR_UART huart3
#define NB_IOT_UART huart1

#define PC_COMM_RECEIVE_MAX 255
#define PM_SENSOR_RECEIVE_MAX 32

extern uint8_t pc_comm_receive[PC_COMM_RECEIVE_MAX];
extern uint8_t pm_sensor_receive[PM_SENSOR_RECEIVE_MAX];
//volatile uint8_t nb_iot_receive[MAX_RECEIVE_ARRAY];

HAL_StatusTypeDef uart_send_message(UART_HandleTypeDef * , const char * );

#endif //SENSOR_PM_UART_COMM_H