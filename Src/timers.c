//
// Created by bartek on 4/1/20.
//

#include "timers.h"
#include "stm32f1xx.h"
#include "uart_comm.h"
#include "pms7003.h"

static uint16_t timer_s = 1000;
const uint16_t pms_cycl = 2 * 60; // 5 minutes timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(nb_iot_uart.tim_counter > 0)
    {
        nb_iot_uart.tim_counter -= 1;
    }

    if(timer_s > 0)
    {
        timer_s -= 1;
    } else
    {
        timer_s = 1000;
        pms_uart.tim_counter -= 1; // Counter decrement every 1 second
    }
    if (1 == PM_SEND_TO_IOT)
    {
        if(pms_uart.tim_counter == 0)
        {
            pms_uart.tim_counter = pms_cycl;
            pm_sensor_host_tx(pm_sensor_req_read);
        }
//        else if(pms_uart.tim_counter == 60) // Wake up 1 minute before readout
//        {
//            pm_sensor_host_tx(pm_sensor_changeSt_wakeup_standby);
//        }
//        else if(pms_uart.tim_counter == (pms_cycl - 10) ) // Sleep 10 seconds after readout done
//        {
//            pm_sensor_host_tx(pm_sensor_changeSt_sleep);
//        }
    }
}
