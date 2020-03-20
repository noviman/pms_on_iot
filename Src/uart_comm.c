//
// Created by bartek on 3/18/20.
//
#include "uart_comm.h"

uint8_t pc_comm_receive[PC_COMM_RECEIVE_MAX];
uint8_t pm_sensor_receive[PM_SENSOR_RECEIVE_MAX];

HAL_StatusTypeDef uart_send_message(UART_HandleTypeDef * handle, const char * message)
{
    size_t length;
    length = strlen(message);
    HAL_UART_Transmit_DMA(handle, (uint8_t *) message, (uint16_t) length); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
}

// Interruptions

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    uart_send_message(&PC_COMM_UART,  pm_sensor_receive);
    HAL_UART_Receive_DMA(&PM_SENSOR_UART, pm_sensor_receive, 32);
}