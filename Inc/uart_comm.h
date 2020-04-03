//
// Created by bartek on 3/18/20.
//

#ifndef SENSOR_PM_UART_COMM_H
#define SENSOR_PM_UART_COMM_H

#include "main.h"

#define PC_COMM_UART huart2
#define PM_SENSOR_UART huart3
#define NB_IOT_UART huart1

#define UART_RECEIVE_MAX 512
#define UART_TRANSMIT_MAX 512
#define MAX_NAME_UART 15

typedef struct uart_struct
{
    char name[MAX_NAME_UART];
    uint16_t data_length;
    uint8_t rx_flag;
    char raw_data_rx[UART_RECEIVE_MAX];
    char raw_data_rx_buffer[UART_RECEIVE_MAX];
    char raw_data_tx_buffer[UART_TRANSMIT_MAX];
    uint16_t concatenate_counter;
    uint16_t tim_counter;
} uart_struct;

extern uart_struct pc_uart;
extern uart_struct pms_uart;
extern uart_struct nb_iot_uart;

void start_dma_uart_rx(void);

HAL_StatusTypeDef uart_send_message(UART_HandleTypeDef *, const char *, const char *);
uint8_t send_check_message(UART_HandleTypeDef *, const char *, const char *,
                           uart_struct *, const uint16_t );
// Idle Detection
void IDLE_DETECT_UART_IRQHandler(UART_HandleTypeDef *);
void IDLE_UART_String_Callback(UART_HandleTypeDef *handle, uart_struct *uart_struct_handle);
void IDLE_UART_Raw_Callback(UART_HandleTypeDef *handle, uart_struct *uart_struct_handle);
#endif //SENSOR_PM_UART_COMM_H