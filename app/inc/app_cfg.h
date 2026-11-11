#ifndef APP_CFG_H
#define APP_CFG_H

#include "FreeRTOS.h"
#include "task.h"

/* Uu tien (so lon = uu tien cao) */
#define TASK_BUTTON_PRIO       (tskIDLE_PRIORITY + 3)
#define TASK_CTRL_PRIO         (tskIDLE_PRIORITY + 3)
#define TASK_ALARM_PRIO        (tskIDLE_PRIORITY + 3)
#define TASK_POT_PRIO          (tskIDLE_PRIORITY + 2)
#define TASK_RTC_PRIO          (tskIDLE_PRIORITY + 2)
#define TASK_DISPLAY_PRIO      (tskIDLE_PRIORITY + 1)

/* Stack (words) */
#define TASK_BUTTON_STACK      128
#define TASK_POT_STACK         128
#define TASK_CTRL_STACK        192
#define TASK_RTC_STACK         160
#define TASK_ALARM_STACK       128
#define TASK_DISPLAY_STACK     256

/* Chu ky (ms) */
#define TASK_BUTTON_PERIOD_MS  10
#define TASK_POT_PERIOD_MS     20
#define TASK_RTC_PERIOD_MS     250
#define TASK_ALARM_PERIOD_MS   250
#define TASK_DISPLAY_TIMEOUT_MS 500

/* Queue */
#define Q_EVT_LEN              8
#define Q_RTC_LEN              4

/* Event group bits */
#define EV_REDRAW              (1U << 0)
#define EV_DISMISS             (1U << 1)

/* Nguong */
#define BTN_DEBOUNCE_SAMPLES   3     /* 3 mau x 10 ms = 30 ms */
#define POT_REPORT_DELTA       12    /* task_pot chi gui khi ADC lech >= 12 */
#define POT_ARM_DELTA          40    /* task_ctrl bo qua bien tro cho den khi nup lech >= 40 */

#endif
