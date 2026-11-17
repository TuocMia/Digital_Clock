#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "app_cfg.h"
#include "app_ipc.h"
#include "hal_gpio.h"
#include "task_alarm.h"

#define ALARM_BUZZER_PORT  GPIOB
#define ALARM_BUZZER_PIN   GPIO_Pin_8

typedef struct {
    uint8_t on;
    uint8_t periods;
} AlarmStep_t;

/* Mỗi period tương ứng TASK_ALARM_PERIOD_MS, hiện tại là 250 ms. */
#define BEEP_STEP  { 1U, 4U }  /* Kêu 200 ms */
#define GAP_STEP   { 0U, 1U }  /* Ngắt 50 ms giữa tiếng */
#define REST_STEP  { 0U, 10U } /* Nghỉ 500 ms giữa nhóm */

static const AlarmStep_t alarm_pattern[] = {
    BEEP_STEP, GAP_STEP, BEEP_STEP, GAP_STEP, BEEP_STEP, REST_STEP,
    BEEP_STEP, GAP_STEP, BEEP_STEP, GAP_STEP, BEEP_STEP, REST_STEP,
    BEEP_STEP, GAP_STEP, BEEP_STEP, GAP_STEP, BEEP_STEP, GAP_STEP
};

static TaskHandle_t xAlarmTaskHandle = NULL;

static void prvAlarmSetRingState(uint8_t ring)
{
    BaseType_t changed = pdFALSE;

    if (app_data_lock() == pdTRUE) {
        if (g_app.ring != ring) {
            g_app.ring = ring;
            changed = pdTRUE;
        }

        app_data_unlock();
    }

    if ((changed != pdFALSE) && (xEventGroup != NULL)) {
        xEventGroupSetBits(xEventGroup, EV_REDRAW);
    }
}

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

    hal_gpio_init(ALARM_BUZZER_PORT,
                  ALARM_BUZZER_PIN,
                  HAL_GPIO_OUT_PP);
    hal_gpio_clear(ALARM_BUZZER_PORT, ALARM_BUZZER_PIN);

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
            (snapshot.aen != 0U) &&
            (snapshot.hour == snapshot.ah) &&
            (snapshot.min == snapshot.am);

        if (alarm_matches == 0U) {
            /* Qua phút báo thức: cho phép kích hoạt ở lần kế tiếp. */
            dismissed = 0U;
            pattern_started = 0U;
            pattern_done = 0U;
            pattern_index = 0U;
            periods_left = 0U;

            hal_gpio_clear(ALARM_BUZZER_PORT, ALARM_BUZZER_PIN);
            prvAlarmSetRingState(0U);
        } else if (dismissed != 0U) {
            pattern_started = 0U;
            pattern_done = 1U;

            hal_gpio_clear(ALARM_BUZZER_PORT, ALARM_BUZZER_PIN);
            prvAlarmSetRingState(0U);
        } else if (pattern_done == 0U) {
            if (pattern_started == 0U) {
                /* Bắt đầu nhóm tiếng đầu tiên. */
                pattern_started = 1U;
                pattern_index = 0U;
                periods_left = alarm_pattern[0].periods;

                hal_gpio_set(ALARM_BUZZER_PORT, ALARM_BUZZER_PIN);
                prvAlarmSetRingState(1U);
            } else if (periods_left > 1U) {
                periods_left--;
            } else {
                pattern_index++;

                if (pattern_index >= pattern_length) {
                    /* Đã phát đủ 3 nhóm. */
                    pattern_started = 0U;
                    pattern_done = 1U;

                    hal_gpio_clear(ALARM_BUZZER_PORT,
                                   ALARM_BUZZER_PIN);
                    prvAlarmSetRingState(0U);
                } else {
                    periods_left =
                        alarm_pattern[pattern_index].periods;

                    if (alarm_pattern[pattern_index].on != 0U) {
                        hal_gpio_set(ALARM_BUZZER_PORT,
                                     ALARM_BUZZER_PIN);
                    } else {
                        hal_gpio_clear(ALARM_BUZZER_PORT,
                                       ALARM_BUZZER_PIN);
                    }
                }
            }
        } else {
            hal_gpio_clear(ALARM_BUZZER_PORT, ALARM_BUZZER_PIN);
        }

        vTaskDelayUntil(&last_wake_time, period);
    }
}

BaseType_t task_alarm_create(void)
{
    return xTaskCreate(prvAlarmTask,
                       "ALARM",
                       TASK_ALARM_STACK,
                       NULL,
                       TASK_ALARM_PRIO,
                       &xAlarmTaskHandle);
}