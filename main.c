#include "FreeRTOS.h"
#include "task.h"

int main(void)
{
    /* 1. Hardware & HAL init */
    /* bsp_init(); hal_delay_init(); st7735_init(); ds1302_init(); */

    /* 2. FreeRTOS IPC object creation */
    /* app_ipc_init(); */

    /* 3. Create tasks */
    /* task_button_create(); task_pot_create(); task_ctrl_create(); */
    /* task_rtc_create(); task_alarm_create(); task_display_create(); */

    /* 4. Start scheduler */
    vTaskStartScheduler();

    for (;;);
}
