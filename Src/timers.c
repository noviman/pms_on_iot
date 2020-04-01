//
// Created by bartek on 4/1/20.
//

#include "timers.h"
#include "stm32f1xx.h"
#include "uart_comm.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(nb_iot_uart.tim_counter > 0)
    {
        nb_iot_uart.tim_counter -= 1;
    }

}