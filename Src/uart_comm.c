//
// Created by bartek on 3/18/20.
//
#include "uart_comm.h"
#include "nb_iot.h"
#include "string.h"
#include "pms7003.h"
#include "pc_comm.h"

 // Initialize with 0 every elem

uart_struct pc_uart = { .name = "PC_UART" };
uart_struct pms_uart = { .name = "PMS_UART" };
uart_struct nb_iot_uart = { .name = "NB-IOT_UART" };

void start_dma_uart_rx(void)
{
    HAL_UART_Receive_DMA(&PM_SENSOR_UART, pms_uart.raw_data_buffer, UART_RECEIVE_MAX);
    HAL_UART_Receive_DMA(&PC_COMM_UART, pc_uart.raw_data_buffer, UART_RECEIVE_MAX);
    HAL_UART_Receive_DMA(&NB_IOT_UART, nb_iot_uart.raw_data_buffer, UART_RECEIVE_MAX);
}

HAL_StatusTypeDef uart_send_message(UART_HandleTypeDef *handle, const char *message, const char *receiver)
{
    size_t length;
    if (handle == &PC_COMM_UART)
    {
        char message_to_pc[UART_TRANSMIT_MAX];
        sprintf(message_to_pc, "[%s] %s", receiver, message);
        length = strlen(message_to_pc);
        HAL_UART_Transmit_DMA(handle, (uint8_t *) message_to_pc, (uint16_t) length);
    }
    else
    {
        length = strlen(message);
        HAL_UART_Transmit_DMA(handle, (uint8_t *) message, (uint16_t) length);
    }

}
//  Interruptions

void IDLE_DETECT_UART_IRQHandler(UART_HandleTypeDef * handle)
{
    if(RESET != __HAL_UART_GET_FLAG(handle, UART_FLAG_IDLE))   // Check if IDLE IT
    {
        __HAL_UART_CLEAR_IDLEFLAG(handle); // Clear IDLE IT Flag
        if (handle == &PC_COMM_UART)
        {
            IDLE_UART_Callback(handle, &pc_uart);
        }

        else if (handle == &NB_IOT_UART)
        {
            IDLE_UART_Callback(handle, &nb_iot_uart);
        }

        else if (handle == &PM_SENSOR_UART)
        {
            IDLE_UART_Callback(handle, &pms_uart);
        }
    }
}

void IDLE_UART_Callback(UART_HandleTypeDef *handle, uart_struct *uart_struct_handle) {
    HAL_UART_DMAStop(handle);
    uart_struct_handle->data_length = UART_RECEIVE_MAX - __HAL_DMA_GET_COUNTER(handle->hdmarx);
    memset(uart_struct_handle->raw_data, 0, UART_RECEIVE_MAX); // Clear whole buffer
    memcpy(uart_struct_handle->raw_data, uart_struct_handle->raw_data_buffer, uart_struct_handle->data_length);
    memset(uart_struct_handle->raw_data_buffer, 0, uart_struct_handle->data_length);
    HAL_UART_Receive_DMA(handle, uart_struct_handle->raw_data_buffer, UART_RECEIVE_MAX);
    uart_struct_handle->rx_flag = 1;
}
