#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "st7789.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "ui_render.h"
#include "task_display.h"

static TaskHandle_t xDisplayTaskHandle = NULL;

static void prvDisplayTask(void *pvParameters)
{
    (void)pvParameters;
    AppData snap;
    uint8_t first_draw = 1U;
    uint8_t prev_state = 0xFFU;
    uint8_t prev_ring  = 0xFFU;

    /* T_Display la task duy nhat truy cap LCD */
    LCD_Init();

    for (;;) {
        (void)xEventGroupWaitBits(
            xEventGroup,
            EV_REDRAW,
            pdTRUE,
            pdFALSE,
            pdMS_TO_TICKS(TASK_DISPLAY_TIMEOUT_MS)
        );

        if (app_data_lock() != pdTRUE) {
            continue;
        }
        snap = g_app;
        app_data_unlock();

        if (first_draw || snap.state != prev_state || snap.ring != prev_ring) {
            ui_draw_frame(&snap);
            first_draw = 0U;
            prev_state = snap.state;
            prev_ring  = snap.ring;
        }

        ui_render(&snap);
    }
}

BaseType_t task_display_create(void)
{
    return xTaskCreate(prvDisplayTask, "DISPLAY", TASK_DISPLAY_STACK, NULL, TASK_DISPLAY_PRIO, &xDisplayTaskHandle);
}
