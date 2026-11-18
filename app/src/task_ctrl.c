/**
  ******************************************************************************
  * @file    task_ctrl.c
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   System state machine (S0..S5) and input coordinator FreeRTOS task.
  ******************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_ctrl.h"

#define POT_STEP_HOUR   48
#define POT_STEP_MIN    32

static TaskHandle_t xCtrlTaskHandle = NULL;

static uint8_t step_wrap(uint8_t cur, int8_t dir, uint8_t max_val)
{
    if (dir > 0) {
        return (cur >= max_val) ? 0U : (uint8_t)(cur + 1U);
    }
    return (cur == 0U) ? max_val : (uint8_t)(cur - 1U);
}

static void prvCtrlTask(void *pvParameters)
{
    (void)pvParameters;
    Evt e;
    uint16_t last_pot   = 0U;
    uint16_t arm_ref    = 0U;
    uint8_t  pot_armed  = 0U;
    uint8_t  time_dirty = 0U;

    for (;;) {
        if (xQueueReceive(xEvtQueue, &e, portMAX_DELAY) != pdPASS) {
            continue;
        }

        if (e.type == EVT_BTN) {
            uint8_t is_ringing = 0U;

            if (app_data_lock() == pdTRUE) {
                is_ringing = g_app.ring;
                app_data_unlock();
            }

            /* Khi chuong dang keu: nhan nut chi tat chuong (EV_DISMISS), khong doi buoc */
            if (is_ringing != 0U) {
                xEventGroupSetBits(xEventGroup, EV_DISMISS);
                continue;
            }

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
                uint8_t changed = 0U;
                int32_t delta = (int32_t)e.val - (int32_t)arm_ref;

                if (st == ST_SET_HOUR || st == ST_ALARM_HOUR) {
                    while (delta >= POT_STEP_HOUR) {
                        g_app.edit_h = step_wrap(g_app.edit_h, 1, 23U);
                        arm_ref = (uint16_t)(arm_ref + POT_STEP_HOUR);
                        delta -= POT_STEP_HOUR;
                        changed = 1U;
                    }
                    while (delta <= -POT_STEP_HOUR) {
                        g_app.edit_h = step_wrap(g_app.edit_h, -1, 23U);
                        arm_ref = (uint16_t)(arm_ref - POT_STEP_HOUR);
                        delta += POT_STEP_HOUR;
                        changed = 1U;
                    }
                    if (changed != 0U && st == ST_SET_HOUR) {
                        time_dirty = 1U;
                    }
                } else if (st == ST_SET_MIN || st == ST_ALARM_MIN) {
                    while (delta >= POT_STEP_MIN) {
                        g_app.edit_m = step_wrap(g_app.edit_m, 1, 59U);
                        arm_ref = (uint16_t)(arm_ref + POT_STEP_MIN);
                        delta -= POT_STEP_MIN;
                        changed = 1U;
                    }
                    while (delta <= -POT_STEP_MIN) {
                        g_app.edit_m = step_wrap(g_app.edit_m, -1, 59U);
                        arm_ref = (uint16_t)(arm_ref - POT_STEP_MIN);
                        delta += POT_STEP_MIN;
                        changed = 1U;
                    }
                    if (changed != 0U && st == ST_SET_MIN) {
                        time_dirty = 1U;
                    }
                } else if (st == ST_ALARM_EN) {
                    uint8_t en = (e.val >= POT_ONOFF_THRESHOLD) ? 1U : 0U;
                    if (en != g_app.edit_en) {
                        g_app.edit_en = en;
                        changed = 1U;
                    }
                }
                app_data_unlock();

                if (changed != 0U && st != ST_VIEW) {
                    xEventGroupSetBits(xEventGroup, EV_REDRAW);
                }
            }
        }
    }
}

BaseType_t task_ctrl_create(void)
{
    return xTaskCreate(prvCtrlTask, "CTRL", TASK_CTRL_STACK, NULL, TASK_CTRL_PRIO, &xCtrlTaskHandle);
}
