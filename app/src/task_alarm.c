/**
  ******************************************************************************
  * @file    task_alarm.c
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   Alarm comparison, buzzer, and LED alert FreeRTOS task.
  ******************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "app_cfg.h"
#include "app_ipc.h"
#include "bsp.h"
#include "task_alarm.h"

/**
  * @brief Single step in the alarm buzzer/LED cadence pattern.
  */
typedef struct {
    uint8_t on;      /**< 1 = buzzer and LED on, 0 = off */
    uint8_t periods; /**< Duration in TASK_ALARM_PERIOD_MS ticks */
} AlarmStep_t;

/* Mỗi period tương ứng TASK_ALARM_PERIOD_MS, hiện tại là 250 ms. */
#define BEEP_STEP  { 1U, 4U }  /**< Beep 4 x 250 ms = 1000 ms */
#define GAP_STEP   { 0U, 1U }  /**< Silent gap 1 x 250 ms = 250 ms between beeps */
#define REST_STEP  { 0U, 10U } /**< Rest 10 x 250 ms = 2500 ms between beep groups */

static const AlarmStep_t alarm_pattern[] = {
    BEEP_STEP, GAP_STEP, BEEP_STEP, GAP_STEP, BEEP_STEP, REST_STEP,
    BEEP_STEP, GAP_STEP, BEEP_STEP, GAP_STEP, BEEP_STEP, REST_STEP,
    BEEP_STEP, GAP_STEP, BEEP_STEP, GAP_STEP, BEEP_STEP, GAP_STEP
};

static TaskHandle_t xAlarmTaskHandle = NULL;

/**
 * @brief  Update g_app.alarm_ringing under M_DATA and trigger EV_REDRAW when changed.
 * @param  ring 1 when alarm is actively ringing, 0 when silent.
 */
static void prvAlarmSetRingState(uint8_t ring)
{
    BaseType_t changed = pdFALSE;

    if (app_data_lock() == pdTRUE) {
        if (g_app.alarm_ringing != ring) {
            g_app.alarm_ringing = ring;
            changed = pdTRUE;
        }

        app_data_unlock();
    }

    if ((changed != pdFALSE) && (xEventGroup != NULL)) {
        xEventGroupSetBits(xEventGroup, EV_REDRAW);
    }
}

/**
 * @brief  FreeRTOS task that compares RTC time with alarm every 250 ms and drives buzzer/LED cadence.
 * @param  pvParameters Unused task argument.
 */
static void prvAlarmTask(void *pvParameters)
{
    TickType_t last_wake_time;
    const TickType_t period =
        pdMS_TO_TICKS(TASK_ALARM_PERIOD_MS);

    const uint8_t pattern_length =
        (uint8_t)(sizeof(alarm_pattern) / sizeof(alarm_pattern[0]));

    uint8_t dismissed = 0U;
    uint8_t pattern_started = 0U;
    uint8_t pattern_done = 0U;
    uint8_t pattern_index = 0U;
    uint8_t periods_left = 0U;

    (void)pvParameters;

    bsp_buzzer_set(0U);
    bsp_led_set(0U);

    last_wake_time = xTaskGetTickCount();

    for (;;) {
        AppData snapshot;
        uint8_t alarm_matches;
        EventBits_t event_bits;

        /* Nhận yêu cầu tắt báo thức. */
        event_bits = xEventGroupWaitBits(xEventGroup,
                                         EV_DISMISS,
                                         pdTRUE,
                                         pdFALSE,
                                         0);

        if ((event_bits & EV_DISMISS) != 0U) {
            dismissed = 1U;
        }

        /* Chụp dữ liệu dùng chung khi đang giữ mutex. */
        if (app_data_lock() != pdTRUE) {
            vTaskDelayUntil(&last_wake_time, period);
            continue;
        }

        snapshot = g_app;
        app_data_unlock();

        alarm_matches =
            (snapshot.rtc_ok != 0U) &&
            (snapshot.alarm_enabled != 0U) &&
            (snapshot.hour == snapshot.alarm_hour) &&
            (snapshot.minute == snapshot.alarm_minute);

        if (alarm_matches == 0U) {
            /* Qua phút báo thức: cho phép kích hoạt ở lần kế tiếp. */
            dismissed = 0U;
            pattern_started = 0U;
            pattern_done = 0U;
            pattern_index = 0U;
            periods_left = 0U;

            bsp_buzzer_set(0U);
            bsp_led_set(0U);
            prvAlarmSetRingState(0U);
        } else if (dismissed != 0U) {
            pattern_started = 0U;
            pattern_done = 1U;

            bsp_buzzer_set(0U);
            bsp_led_set(0U);
            prvAlarmSetRingState(0U);
        } else if (pattern_done == 0U) {
            if (pattern_started == 0U) {
                /* Bắt đầu nhóm tiếng đầu tiên. */
                pattern_started = 1U;
                pattern_index = 0U;
                periods_left = alarm_pattern[0].periods;

                bsp_buzzer_set(1U);
                bsp_led_set(1U);
                prvAlarmSetRingState(1U);
            } else if (periods_left > 1U) {
                periods_left--;
            } else {
                pattern_index++;

                if (pattern_index >= pattern_length) {
                    /* Đã phát đủ 3 nhóm. */
                    pattern_started = 0U;
                    pattern_done = 1U;

                    bsp_buzzer_set(0U);
                    bsp_led_set(0U);
                    prvAlarmSetRingState(0U);
                } else {
                    periods_left =
                        alarm_pattern[pattern_index].periods;

                    if (alarm_pattern[pattern_index].on != 0U) {
                        bsp_buzzer_set(1U);
                        bsp_led_set(1U);
                    } else {
                        bsp_buzzer_set(0U);
                        bsp_led_set(0U);
                    }
                }
            }
        } else {
            bsp_buzzer_set(0U);
            bsp_led_set(0U);
        }

        vTaskDelayUntil(&last_wake_time, period);
    }
}

/**
 * @brief  Create the T_Alarm FreeRTOS task.
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_alarm_create(void)
{
    return xTaskCreate(prvAlarmTask,
                       "ALARM",
                       TASK_ALARM_STACK,
                       NULL,
                       TASK_ALARM_PRIO,
                       &xAlarmTaskHandle);
}