#include "FreeRTOS.h"
#include "task.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_pot.h"

static TaskHandle_t xPotTaskHandle = NULL;

static void prvPotTask(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(TASK_POT_PERIOD_MS);

    for (;;) {
        /* TODO: doc ADC trung binh 8 mau, gui Evt{EVT_POT, val} khi lech >= POT_REPORT_DELTA */

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

BaseType_t task_pot_create(void)
{
    return xTaskCreate(prvPotTask, "POT", TASK_POT_STACK, NULL, TASK_POT_PRIO, &xPotTaskHandle);
}
