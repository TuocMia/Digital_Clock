#include "FreeRTOS.h"
#include "task.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "ui_render.h"
#include "task_display.h"

static TaskHandle_t xDisplayTaskHandle = NULL;

static void prvDisplayTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        xEventGroupWaitBits(xEventGroup, EV_REDRAW, pdTRUE, pdFALSE,
                            pdMS_TO_TICKS(TASK_DISPLAY_TIMEOUT_MS));

        /* TODO: chup g_app trong xDataMutex, nha mutex, roi ui_render(&snapshot) */
    }
}

BaseType_t task_display_create(void)
{
    return xTaskCreate(prvDisplayTask, "DISPLAY", TASK_DISPLAY_STACK, NULL, TASK_DISPLAY_PRIO, &xDisplayTaskHandle);
}
