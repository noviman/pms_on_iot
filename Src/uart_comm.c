//
// Created by bartek on 3/18/20.
//
#include "uart_comm.h"
#include "nb_iot.h"
#include "string.h"
#include "pms7003.h"
#include "pc_comm.h"

uint8_t pc_comm_raw_data[PC_COMM_RECEIVE_MAX];
uint8_t pm_sensor_raw_data[PM_SENSOR_RECEIVE_MAX];
uint8_t nb_iot_raw_data_buffer[NB_IOT_RECEIVE_MAX];
uint8_t nb_iot_raw_data[NB_IOT_RECEIVE_MAX];

HAL_StatusTypeDef uart_send_message(UART_HandleTypeDef * handle, const char * message)
{
    size_t length;
    length = strlen(message);
    HAL_UART_Transmit_DMA(handle, (uint8_t *) message, (uint16_t) length);
}
//  Interruptions

void IDLE_DETECT_UART_IRQHandler(UART_HandleTypeDef * handle)
{
    if(RESET != __HAL_UART_GET_FLAG(handle, UART_FLAG_IDLE))   // Check if IDLE IT
    {
        __HAL_UART_CLEAR_IDLEFLAG(handle); // Clear IDLE IT Flag
        IDLE_UART_Callback(handle);
    }
}

void IDLE_UART_Callback(UART_HandleTypeDef * handle)
{
    uint16_t data_length;
    HAL_UART_DMAStop(handle);

    // Calc the data length
    data_length = NB_IOT_RECEIVE_MAX - __HAL_DMA_GET_COUNTER(handle->hdmarx);
    if (handle == &NB_IOT_UART)
    {
        strcpy(nb_iot_raw_data, nb_iot_raw_data_buffer);
        memset(nb_iot_raw_data_buffer, 0, data_length);
        HAL_UART_Receive_DMA(handle, nb_iot_raw_data_buffer, NB_IOT_RECEIVE_MAX);
        nb_iot_receive_callback();
    }
    else
    {
        Error_Handler();
    }

//    HAL_UART_Transmit_IT(handle, nb_iot_raw_data_buffer, data_length);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &PM_SENSOR_UART)
    {
        pm_sensor_rx_flag = 1;
        HAL_UART_Receive_DMA(huart, pm_sensor_raw_data, 32);
    }
    else if (huart == &PC_COMM_UART)
    {
        pc_comm_rx_flag = 1;
        HAL_UART_Receive_DMA(huart, pc_comm_raw_data, 1);
    }
}