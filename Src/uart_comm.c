//
// Created by bartek on 3/18/20.
//
#include "nb_iot.h"
#include "uart_comm.h"
#include "string.h"
#include "stdlib.h"
#include "stm32f1xx_hal_dma_ex.h"
#include "stm32f1xx_hal_uart.h"
#include "helpers.h"

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
    size_t length = 0;
//    while(__HAL_DMA_GET_COUNTER(handle->hdmatx) != 0) { }
    while(handle->gState == HAL_UART_STATE_BUSY_TX) { }  // Prevent from corrupting TX Messages
    if (handle == &PC_COMM_UART)
    {
        char message_to_pc[UART_TRANSMIT_MAX] = { 0 };
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
        else
        {
            Error_Handler();
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

uint8_t send_check_message(UART_HandleTypeDef *handle, const char *mes_send, const char *mes_check,
                           uart_struct *uart_struct_handle, const uint16_t timeout)
{
    if(1 == uart_struct_handle->rx_flag)
    {
        uart_struct_handle->rx_flag = 0;
    }
    uart_send_message(handle, mes_send, NULL);
    uart_struct_handle->tim_counter = timeout;
    while(uart_struct_handle->rx_flag == 0)
    {
        if(uart_struct_handle->tim_counter == 0)
        {
            return 2;
        }
    }
    uart_struct_handle->rx_flag = 0;
    if(0 != strcmp( remove_req_from_read( (char *)uart_struct_handle->raw_data), mes_check) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

