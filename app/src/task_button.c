#include "FreeRTOS.h"
#include "task.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_button.h"

static TaskHandle_t xButtonTaskHandle = NULL;

static void prvButtonTask(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(TASK_BUTTON_PERIOD_MS);

    for (;;) {
        /* TODO: quet nut, chong doi 3 mau, gui Evt{EVT_BTN} vao xEvtQueue */

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

BaseType_t task_button_create(void)
{
    return xTaskCreate(prvButtonTask, "BUTTON", TASK_BUTTON_STACK, NULL, TASK_BUTTON_PRIO, &xButtonTaskHandle);
}
