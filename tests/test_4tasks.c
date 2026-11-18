#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "bsp.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_button.h"
#include "task_pot.h"
#include "task_rtc.h"
#include "task_display.h"

/*
 * Integration Test Harness for 4 real tasks:
 *   T_Button + T_Pot + T_RTC + T_Display
 * Uses a lightweight mock T_Ctrl to route EVT_BTN / EVT_POT -> g_app & Q_RTC.
 */
static uint8_t map_pot(uint16_t adc, uint8_t max_val)
{
    uint32_t v = ((uint32_t)adc * (uint32_t)(max_val + 1U)) / (POT_ADC_MAX + 1U);
    if (v > max_val) {
        v = max_val;
    }
    return (uint8_t)v;
}

static void prvMockCtrlTask(void *pvParameters)
{
    (void)pvParameters;
    Evt e;
    uint16_t last_pot = 0U;
    uint16_t arm_ref  = 0U;
    uint8_t  pot_armed = 0U;
    uint8_t  time_dirty = 0U;
    uint8_t  led_on = 0U;

    for (;;) {
        if (xQueueReceive(xEvtQueue, &e, portMAX_DELAY) != pdPASS) {
            continue;
        }

        if (e.type == EVT_BTN) {
            led_on ^= 1U;
            bsp_led_set(led_on);

            if (app_data_lock() == pdTRUE) {
                uint8_t prev_state = g_app.state;
                uint8_t next_state = (uint8_t)((prev_state + 1U) % ST_COUNT);

                if (prev_state == ST_SET_MIN && time_dirty != 0U) {
                    RtcCmd cmd = {
                        .cmd = CMD_SET_TIME,
                        .h   = g_app.edit_h,
                        .m   = g_app.edit_m,
                        .en  = 0U
                    };
                    (void)xQueueSend(xRtcQueue, &cmd, pdMS_TO_TICKS(Q_RTC_SEND_TIMEOUT_MS));
                    time_dirty = 0U;
                } else if (prev_state == ST_ALARM_EN) {
                    RtcCmd cmd;
                    g_app.ah  = g_app.edit_h;
                    g_app.am  = g_app.edit_m;
                    g_app.aen = g_app.edit_en;
                    cmd.cmd = CMD_SET_ALARM;
                    cmd.h   = g_app.ah;
                    cmd.m   = g_app.am;
                    cmd.en  = g_app.aen;
                    (void)xQueueSend(xRtcQueue, &cmd, pdMS_TO_TICKS(Q_RTC_SEND_TIMEOUT_MS));
                }

                g_app.state = next_state;
                if (next_state == ST_SET_HOUR) {
                    g_app.edit_h = g_app.hour;
                    g_app.edit_m = g_app.min;
                    time_dirty   = 0U;
                } else if (next_state == ST_ALARM_HOUR) {
                    g_app.edit_h  = g_app.ah;
                    g_app.edit_m  = g_app.am;
                    g_app.edit_en = g_app.aen;
                }
                app_data_unlock();

                arm_ref   = last_pot;
                pot_armed = 0U;
                xEventGroupSetBits(xEventGroup, EV_REDRAW);
            }
        } else if (e.type == EVT_POT) {
            last_pot = e.val;

            if (pot_armed == 0U) {
                uint16_t diff = (e.val >= arm_ref) ? (e.val - arm_ref) : (arm_ref - e.val);
                if (diff < POT_ARM_DELTA) {
                    continue;
                }
                pot_armed = 1U;
            }

            if (app_data_lock() == pdTRUE) {
                uint8_t st = g_app.state;
                if (st == ST_SET_HOUR) {
                    g_app.edit_h = map_pot(e.val, 23U);
                    time_dirty   = 1U;
                } else if (st == ST_SET_MIN) {
                    g_app.edit_m = map_pot(e.val, 59U);
                    time_dirty   = 1U;
                } else if (st == ST_ALARM_HOUR) {
                    g_app.edit_h = map_pot(e.val, 23U);
                } else if (st == ST_ALARM_MIN) {
                    g_app.edit_m = map_pot(e.val, 59U);
                } else if (st == ST_ALARM_EN) {
                    g_app.edit_en = (e.val >= POT_ONOFF_THRESHOLD) ? 1U : 0U;
                }
                app_data_unlock();

                if (st != ST_VIEW) {
                    xEventGroupSetBits(xEventGroup, EV_REDRAW);
                }
            }
        }
    }
}

int main(void)
{
    NVIC_SetPriorityGrouping(3U);
    bsp_init();

    if (app_ipc_init() != pdPASS) {
        for (;;) {}
    }

    if (task_button_create() != pdPASS ||
        task_pot_create() != pdPASS ||
        task_rtc_create() != pdPASS ||
        task_display_create() != pdPASS) {
        for (;;) {}
    }

    if (xTaskCreate(prvMockCtrlTask, "CTRL_TST", TASK_CTRL_STACK, NULL, TASK_CTRL_PRIO, NULL) != pdPASS) {
        for (;;) {}
    }

    vTaskStartScheduler();

    for (;;) {}
}
