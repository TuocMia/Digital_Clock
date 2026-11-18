#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "bsp.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_button.h"
#include "task_display.h"

/*
 * Unit / Hardware Test Harness for T_Button + T_Display ONLY.
 * - T_Button scans PA0 (30 ms debounce) -> sends EVT_BTN to xEvtQueue.
 * - prvTestHarnessTask simulates T_Ctrl / T_RTC / T_Alarm:
 *     * Ticks dummy clock every 1 s and triggers EV_REDRAW.
 *     * On each EVT_BTN: toggles PC13 LED and steps through:
 *       S0 (VIEW) -> S1 (SET_HOUR) -> S2 (SET_MIN) -> S3 (ALARM_HOUR)
 *       -> S4 (ALARM_MIN) -> S5 (ALARM_EN) -> ALARM! (ring=1) -> S0.
 * - T_Display initializes ST7789 and renders updates on EV_REDRAW.
 */
static void prvTestHarnessTask(void *pvParameters)
{
    (void)pvParameters;
    Evt e;
    uint8_t step = 0U;
    uint8_t led_on = 0U;

    for (;;) {
        if (xQueueReceive(xEvtQueue, &e, pdMS_TO_TICKS(1000)) == pdPASS) {
            if (e.type == EVT_BTN) {
                led_on ^= 1U;
                bsp_led_set(led_on);

                step = (uint8_t)((step + 1U) % 7U);
                if (app_data_lock() == pdTRUE) {
                    if (step < ST_COUNT) {
                        g_app.state = step;
                        g_app.ring  = 0U;
                        if (step == ST_SET_HOUR || step == ST_SET_MIN) {
                            g_app.edit_h = g_app.hour;
                            g_app.edit_m = g_app.min;
                        } else if (step == ST_ALARM_HOUR || step == ST_ALARM_MIN) {
                            g_app.edit_h = g_app.ah;
                            g_app.edit_m = g_app.am;
                        } else if (step == ST_ALARM_EN) {
                            g_app.edit_en = (uint8_t)(g_app.aen ^ 1U);
                            g_app.aen = g_app.edit_en;
                        }
                    } else {
                        /* Step 6: test red ALARM! header banner */
                        g_app.state = ST_VIEW;
                        g_app.ring  = 1U;
                    }
                    app_data_unlock();
                }
                xEventGroupSetBits(xEventGroup, EV_REDRAW);
            }
        } else {
            /* 1 s timeout: tick dummy seconds to verify 7-seg partial redraw */
            if (app_data_lock() == pdTRUE) {
                g_app.sec++;
                if (g_app.sec >= 60U) {
                    g_app.sec = 0U;
                    g_app.min++;
                    if (g_app.min >= 60U) {
                        g_app.min = 0U;
                        g_app.hour = (uint8_t)((g_app.hour + 1U) % 24U);
                    }
                }
                app_data_unlock();
            }
            xEventGroupSetBits(xEventGroup, EV_REDRAW);
        }
    }
}

int main(void)
{
    /* FreeRTOS on Cortex-M3 requires all priority bits as preemption priority */
    NVIC_SetPriorityGrouping(3U);

    bsp_init();

    if (app_ipc_init() != pdPASS) {
        for (;;) {}
    }

    /* Seed dummy clock & alarm values for display test */
    g_app.hour  = 12U;
    g_app.min   = 34U;
    g_app.sec   = 50U;
    g_app.date  = 14U;
    g_app.month = 11U;
    g_app.year  = 26U;
    g_app.ah    = 7U;
    g_app.am    = 30U;
    g_app.aen   = 1U;
    g_app.state = ST_VIEW;
    g_app.ring  = 0U;

    if (task_button_create() != pdPASS) {
        for (;;) {}
    }
    if (task_display_create() != pdPASS) {
        for (;;) {}
    }
    if (xTaskCreate(prvTestHarnessTask, "TEST", 128, NULL, TASK_CTRL_PRIO, NULL) != pdPASS) {
        for (;;) {}
    }

    vTaskStartScheduler();

    for (;;) {}
}
