/**
  ******************************************************************************
  * @file    task_ctrl.c
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   System state machine (S0..S8) and input coordinator FreeRTOS task.
  ******************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_ctrl.h"

#define POT_STEP_HOUR   48U
#define POT_STEP_MIN    32U
#define POT_STEP_DATE   48U
#define POT_STEP_MONTH  96U
#define POT_STEP_YEAR   48U

static TaskHandle_t xCtrlTaskHandle = NULL;

/**
 * @brief  Increment or decrement a bounded integer with wrap-around.
 * @param  cur     Current field value.
 * @param  dir     Step direction (> 0 to increment, <= 0 to decrement).
 * @param  min_val Inclusive lower bound.
 * @param  max_val Inclusive upper bound.
 * @return Updated value wrapped within [min_val, max_val].
 */
static uint8_t step_wrap_range(uint8_t cur, int8_t dir, uint8_t min_val, uint8_t max_val)
{
    if (dir > 0) {
        return (cur >= max_val) ? min_val : (uint8_t)(cur + 1U);
    }
    return (cur <= min_val) ? max_val : (uint8_t)(cur - 1U);
}

/**
 * @brief  Apply relative potentiometer ADC steps to an edited field.
 * @param  field   Pointer to the edited field to update.
 * @param  min_val Inclusive lower bound.
 * @param  max_val Inclusive upper bound.
 * @param  step    ADC delta required per single step.
 * @param  adc_val Latest 12-bit ADC reading.
 * @param  arm_ref In/out reference ADC value updated by each consumed step.
 * @return 1 if *field changed, 0 otherwise.
 */
static uint8_t apply_pot_steps(uint8_t *field, uint8_t min_val, uint8_t max_val,
                               uint16_t step, uint16_t adc_val, uint16_t *arm_ref)
{
    uint8_t changed = 0U;
    int32_t delta = (int32_t)adc_val - (int32_t)(*arm_ref);

    while (delta >= (int32_t)step) {
        *field = step_wrap_range(*field, 1, min_val, max_val);
        *arm_ref = (uint16_t)(*arm_ref + step);
        delta -= (int32_t)step;
        changed = 1U;
    }
    while (delta <= -(int32_t)step) {
        *field = step_wrap_range(*field, -1, min_val, max_val);
        *arm_ref = (uint16_t)(*arm_ref - step);
        delta += (int32_t)step;
        changed = 1U;
    }

    return changed;
}

/**
 * @brief  FreeRTOS task managing the S0..S8 state machine and dispatching Q_RTC / EV_REDRAW / EV_DISMISS.
 * @param  pvParameters Unused task argument.
 */
static void prvCtrlTask(void *pvParameters)
{
    (void)pvParameters;
    Evt e;
    uint16_t last_pot   = 0U;
    uint16_t arm_ref    = 0U;
    uint8_t  pot_armed  = 0U;
    uint8_t  time_dirty = 0U;
    uint8_t  date_dirty = 0U;

    for (;;) {
        if (xQueueReceive(xEvtQueue, &e, portMAX_DELAY) != pdPASS) {
            continue;
        }

        if (e.type == EVT_BTN) {
            uint8_t is_ringing = 0U;

            if (app_data_lock() == pdTRUE) {
                is_ringing = g_app.alarm_ringing;
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
                        .command = CMD_SET_TIME,
                        .hour    = g_app.edit_hour,
                        .minute  = g_app.edit_minute
                    };
                    (void)xQueueSend(xRtcQueue, &cmd, pdMS_TO_TICKS(Q_RTC_SEND_TIMEOUT_MS));
                    time_dirty = 0U;
                } else if (prev_state == ST_SET_YEAR && date_dirty != 0U) {
                    RtcCmd cmd = {
                        .command = CMD_SET_DATE,
                        .date    = g_app.edit_date,
                        .month   = g_app.edit_month,
                        .year    = g_app.edit_year
                    };
                    (void)xQueueSend(xRtcQueue, &cmd, pdMS_TO_TICKS(Q_RTC_SEND_TIMEOUT_MS));
                    date_dirty = 0U;
                } else if (prev_state == ST_ALARM_EN) {
                    RtcCmd cmd = {
                        .command       = CMD_SET_ALARM,
                        .hour          = g_app.edit_hour,
                        .minute        = g_app.edit_minute,
                        .alarm_enabled = g_app.edit_alarm_enabled
                    };
                    g_app.alarm_hour    = g_app.edit_hour;
                    g_app.alarm_minute  = g_app.edit_minute;
                    g_app.alarm_enabled = g_app.edit_alarm_enabled;
                    (void)xQueueSend(xRtcQueue, &cmd, pdMS_TO_TICKS(Q_RTC_SEND_TIMEOUT_MS));
                }

                g_app.state = next_state;
                if (next_state == ST_SET_HOUR) {
                    g_app.edit_hour   = g_app.hour;
                    g_app.edit_minute = g_app.minute;
                    g_app.edit_date   = (g_app.date >= 1U && g_app.date <= 31U) ? g_app.date : 1U;
                    g_app.edit_month  = (g_app.month >= 1U && g_app.month <= 12U) ? g_app.month : 1U;
                    g_app.edit_year   = (g_app.year <= 99U) ? g_app.year : 26U;
                    time_dirty = 0U;
                    date_dirty = 0U;
                } else if (next_state == ST_ALARM_HOUR) {
                    g_app.edit_hour          = g_app.alarm_hour;
                    g_app.edit_minute        = g_app.alarm_minute;
                    g_app.edit_alarm_enabled = g_app.alarm_enabled;
                }
                app_data_unlock();

                arm_ref   = last_pot;
                pot_armed = 0U;
                xEventGroupSetBits(xEventGroup, EV_REDRAW);
            }
        } else if (e.type == EVT_POT) {
            last_pot = e.value;

            if (pot_armed == 0U) {
                uint16_t diff = (e.value >= arm_ref) ? (e.value - arm_ref) : (arm_ref - e.value);
                if (diff < POT_ARM_DELTA) {
                    continue;
                }
                pot_armed = 1U;
            }

            if (app_data_lock() == pdTRUE) {
                uint8_t st = g_app.state;
                uint8_t changed = 0U;

                switch (st) {
                case ST_SET_HOUR:
                    changed = apply_pot_steps(&g_app.edit_hour, 0U, 23U, POT_STEP_HOUR, e.value, &arm_ref);
                    if (changed != 0U) { time_dirty = 1U; }
                    break;
                case ST_SET_MIN:
                    changed = apply_pot_steps(&g_app.edit_minute, 0U, 59U, POT_STEP_MIN, e.value, &arm_ref);
                    if (changed != 0U) { time_dirty = 1U; }
                    break;
                case ST_SET_DATE:
                    changed = apply_pot_steps(&g_app.edit_date, 1U, 31U, POT_STEP_DATE, e.value, &arm_ref);
                    if (changed != 0U) { date_dirty = 1U; }
                    break;
                case ST_SET_MONTH:
                    changed = apply_pot_steps(&g_app.edit_month, 1U, 12U, POT_STEP_MONTH, e.value, &arm_ref);
                    if (changed != 0U) { date_dirty = 1U; }
                    break;
                case ST_SET_YEAR:
                    changed = apply_pot_steps(&g_app.edit_year, 0U, 99U, POT_STEP_YEAR, e.value, &arm_ref);
                    if (changed != 0U) { date_dirty = 1U; }
                    break;
                case ST_ALARM_HOUR:
                    changed = apply_pot_steps(&g_app.edit_hour, 0U, 23U, POT_STEP_HOUR, e.value, &arm_ref);
                    break;
                case ST_ALARM_MIN:
                    changed = apply_pot_steps(&g_app.edit_minute, 0U, 59U, POT_STEP_MIN, e.value, &arm_ref);
                    break;
                case ST_ALARM_EN: {
                    uint8_t en = (e.value >= POT_ONOFF_THRESHOLD) ? 1U : 0U;
                    if (en != g_app.edit_alarm_enabled) {
                        g_app.edit_alarm_enabled = en;
                        changed = 1U;
                    }
                    break;
                }
                default:
                    break;
                }
                app_data_unlock();

                if (changed != 0U && st != ST_VIEW) {
                    xEventGroupSetBits(xEventGroup, EV_REDRAW);
                }
            }
        }
    }
}

/**
 * @brief  Create the T_Ctrl FreeRTOS task.
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_ctrl_create(void)
{
    return xTaskCreate(prvCtrlTask, "CTRL", TASK_CTRL_STACK, NULL, TASK_CTRL_PRIO, &xCtrlTaskHandle);
}
