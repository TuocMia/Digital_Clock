#include "FreeRTOS.h"
#include "task.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_rtc.h"

static TaskHandle_t xRtcTaskHandle = NULL;

static void prvRtcTask(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(TASK_RTC_PERIOD_MS);

    for (;;) {
        /* TODO: doc DS1302 vao g_app (giu xDataMutex), xu ly RtcCmd tu xRtcQueue, dat EV_REDRAW */

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

BaseType_t task_rtc_create(void)
{
    return xTaskCreate(prvRtcTask, "RTC", TASK_RTC_STACK, NULL, TASK_RTC_PRIO, &xRtcTaskHandle);
}
