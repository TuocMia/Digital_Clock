/**
 * @file    main.c
 * @brief   Entry point for the Digital Clock FreeRTOS application.
 */

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "app_ipc.h"
#include "task_button.h"
#include "task_pot.h"
#include "task_ctrl.h"
#include "task_rtc.h"
#include "task_alarm.h"
#include "task_display.h"

/**
 * @brief  Initialize hardware, FreeRTOS IPC objects, create all 6 tasks, and start the scheduler.
 * @return Never returns under normal operation.
 */
int main(void)
{
    /* 1. Hardware & BSP init */
    NVIC_SetPriorityGrouping(3U);
    bsp_init();

    /* 2. FreeRTOS IPC Object Creation */
    if (app_ipc_init() != pdPASS) {
        for (;;) {}
    }

    /* 3. Create all 6 FreeRTOS Tasks */
    if (task_button_create()  != pdPASS) { for (;;) {} }
    if (task_pot_create()     != pdPASS) { for (;;) {} }
    if (task_ctrl_create()    != pdPASS) { for (;;) {} }
    if (task_rtc_create()     != pdPASS) { for (;;) {} }
    if (task_alarm_create()   != pdPASS) { for (;;) {} }
    if (task_display_create() != pdPASS) { for (;;) {} }

    /* 4. Start FreeRTOS Scheduler */
    vTaskStartScheduler();

    /* Chuong trinh khong bao gio chay den day neu RTOS du heap */
    for (;;) {}
}
