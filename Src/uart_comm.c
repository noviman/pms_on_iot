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

static uint16_t concatenate_counter = 0;
uart_struct pc_uart = { .name = "PC_UART", .concatenate_counter = 0};
uart_struct pms_uart = { .name = "PMS_UART", .concatenate_counter = 0 };
uart_struct nb_iot_uart = { .name = "NB-IOT_UART", .concatenate_counter = 0 };

void start_dma_uart_rx(void)
{
    HAL_UART_Receive_DMA(&PM_SENSOR_UART, (uint8_t *) pms_uart.raw_data_rx_buffer, UART_RECEIVE_MAX);
    HAL_UART_Receive_DMA(&PC_COMM_UART, (uint8_t *) pc_uart.raw_data_rx_buffer, UART_RECEIVE_MAX);
    HAL_UART_Receive_DMA(&NB_IOT_UART, (uint8_t *) nb_iot_uart.raw_data_rx_buffer, UART_RECEIVE_MAX);

}

HAL_StatusTypeDef uart_send_message(UART_HandleTypeDef *handle, const char *message, const char *receiver)
{
    size_t length = 0;
//    while(__HAL_DMA_GET_COUNTER(handle->hdmatx) != 0) { }
    while(handle->gState != HAL_UART_STATE_READY) { }  // Prevent from corrupting TX Messages
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
            IDLE_UART_String_Callback(handle, &pc_uart);
        }

        else if (handle == &NB_IOT_UART)
        {
            IDLE_UART_String_Callback(handle, &nb_iot_uart);
        }

        else if (handle == &PM_SENSOR_UART)
        {
            IDLE_UART_Raw_Callback(handle, &pms_uart);
        }
        else
        {
            Error_Handler();
        }
    }
}

uint8_t concatenate_message_buffer(uart_struct *uart_struct_handle, uint16_t counter)
{
    memcpy(&uart_struct_handle->raw_data_rx[counter],
            uart_struct_handle->raw_data_rx_buffer,
            uart_struct_handle->data_length);
}

void IDLE_UART_Raw_Callback(UART_HandleTypeDef *handle, uart_struct *uart_struct_handle)
{
    HAL_UART_DMAStop(handle);
    uart_struct_handle->data_length = UART_RECEIVE_MAX - __HAL_DMA_GET_COUNTER(handle->hdmarx);
    memset(uart_struct_handle->raw_data_rx, 0, UART_RECEIVE_MAX); // Clear whole buffer
    concatenate_message_buffer(uart_struct_handle, 0);
    memset(uart_struct_handle->raw_data_rx_buffer, 0, UART_RECEIVE_MAX); // Clear whole buffer
    uart_struct_handle->rx_flag = 1;
    HAL_UART_Receive_DMA(handle, (uint8_t *) uart_struct_handle->raw_data_rx_buffer, UART_RECEIVE_MAX);
}
/* Character '/n' terminates the string! In case when not spotted, concatenation is performed */
/* When RX Buffer is longer than UART_RECEIVE_MAX then send message on PC to handle this! */
void IDLE_UART_String_Callback(UART_HandleTypeDef *handle, uart_struct *uart_struct_handle)
{
    HAL_UART_DMAStop(handle);
    uart_struct_handle->data_length = UART_RECEIVE_MAX - __HAL_DMA_GET_COUNTER(handle->hdmarx);
    if ((uart_struct_handle->data_length + uart_struct_handle->concatenate_counter) >= UART_RECEIVE_MAX) {
        /* If concatenated buffer is longer. Clear everything and notify user on PC Uart*/
        sprintf(pc_uart.raw_data_tx_buffer,
                "RX Buffer exceeds UART_RECEIVE_MAX %d!. Not handling this one!\r\n", UART_RECEIVE_MAX);
        uart_send_message(&PC_COMM_UART, pc_uart.raw_data_tx_buffer, uart_struct_handle->name);
        memset(uart_struct_handle->raw_data_rx_buffer, 0, UART_RECEIVE_MAX);
        memset(uart_struct_handle->raw_data_rx, 0, UART_RECEIVE_MAX);
        uart_struct_handle->data_length = 0;
        uart_struct_handle->concatenate_counter = 0;
        uart_struct_handle->concatenate_counter = 0;
        HAL_UART_Receive_DMA(handle, (uint8_t *) uart_struct_handle->raw_data_rx_buffer, UART_RECEIVE_MAX);
        return;
    }
    // Clear RX Buffer at the beginning
    if (0 == uart_struct_handle->concatenate_counter)
    {
        memset(uart_struct_handle->raw_data_rx, 0, UART_RECEIVE_MAX); // Clear whole buffer
    }
    if (uart_struct_handle->raw_data_rx_buffer[(uart_struct_handle->data_length) - 1] == '\n')
    {
        concatenate_message_buffer(uart_struct_handle, uart_struct_handle->concatenate_counter);
        uart_struct_handle->concatenate_counter += uart_struct_handle->data_length;
        uart_struct_handle->data_length = uart_struct_handle->concatenate_counter;
        uart_struct_handle->rx_flag = 1;
        uart_struct_handle->concatenate_counter = 0;

    }
    else
    {
        concatenate_message_buffer(uart_struct_handle, uart_struct_handle->concatenate_counter);
        uart_struct_handle->concatenate_counter += uart_struct_handle->data_length;
    }
    memset(uart_struct_handle->raw_data_rx_buffer, 0, UART_RECEIVE_MAX);
    HAL_UART_Receive_DMA(handle, (uint8_t *) uart_struct_handle->raw_data_rx_buffer, UART_RECEIVE_MAX);
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
    uart_struct_handle->tim_counter = 0;
    uart_struct_handle->rx_flag = 0;
    if(0 != strcmp(remove_req_from_read( (char *)uart_struct_handle->raw_data_rx), mes_check) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
