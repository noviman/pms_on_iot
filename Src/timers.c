//
// Created by bartek on 4/1/20.
//

#include "timers.h"
#include "stm32f1xx.h"
#include "uart_comm.h"
#include "pms7003.h"
#include "nb_iot.h"
#include "timers.h"

#define PMS_CYCL_TIM 5 * 60 // Seconds
#define NB_INIT_TIM 10000 // Miliseconds

static uint16_t timer_s = 1000;
static uint16_t nb_init_timer = NB_INIT_TIM;

// Interrupt every 1 ms
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 1 Second handler
    if(timer_s > 0)
    {
        timer_s -= 1;
    } else
    {
        timer_s = 1000;
    }

    nb_tim_application();
    pms_tim_application();

}

void nb_tim_application()
{
    // For Timeouts in send_check_message function
    if(nb_iot_uart.tim_counter > 0)
    {
        nb_iot_uart.tim_counter -= 1;
    }
    // Init Handler. Init after NB_INIT_TIM time in ms. If something is going to be wrong, it will execute again
    if (0 == (nb_iot_state & IOT_READY) )
    {
        if(nb_init_timer == 0)
        {
            nb_init_timer = NB_INIT_TIM;
            nb_tim_flag = 1;
        }
        else
        {
            nb_init_timer -= 1;
        }
    }
    else
    {
        nb_init_timer = 0;
    }
}
void pms_tim_application()
{
    // Timer to wake up PM Sensor cyclically. Reads 10 frames then go sleep
    // PMS_CYCL_TIM constant sets interruption frequency.
    if ( (1 == PM_SEND_TO_IOT) && (IOT_READY == (nb_iot_state & IOT_READY)) )
    {
        if(timer_s == 1000)
        {
            pms_uart.tim_counter -= 1; // Counter decrement every 1 second
        }
        if(pms_uart.tim_counter == 0)
        {
            pms_uart.tim_counter = PMS_CYCL_TIM;
            pm_sensor_host_tx(pm_sensor_changeSt_wakeup);
        }
    }
}
