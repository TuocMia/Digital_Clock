#include "FreeRTOS.h"
#include "task.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_alarm.h"

static TaskHandle_t xAlarmTaskHandle = NULL;

static void prvAlarmTask(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(TASK_ALARM_PERIOD_MS);

    for (;;) {
        /* TODO: so gio voi gio hen, dieu khien buzzer/LED, nhan EV_DISMISS */

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

BaseType_t task_alarm_create(void)
{
    return xTaskCreate(prvAlarmTask, "ALARM", TASK_ALARM_STACK, NULL, TASK_ALARM_PRIO, &xAlarmTaskHandle);
}
