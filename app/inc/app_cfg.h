#ifndef APP_CFG_H
#define APP_CFG_H

/*
 * BANG THONG SO TAI NGUYEN RTOS (Giai doan 2 - Buoc 1)
 * Moi thay doi trong file nay phai duoc ca nhom thong nhat va merge qua PR.
 *
 *  Task       | Kich hoat          | Prio | Stack (words) | Gui                    | Nhan
 *  -----------+--------------------+------+---------------+------------------------+------------------------
 *  T_Button   | 10 ms              |  3   | 128           | Q_EVT (EVT_BTN)        | -
 *  T_Pot      | 20 ms              |  2   | 128           | Q_EVT (EVT_POT)        | -
 *  T_Ctrl     | khi co Q_EVT       |  3   | 192           | Q_RTC, EV_REDRAW,      | Q_EVT
 *             |                    |      |               | EV_DISMISS             |
 *  T_RTC      | 250 ms             |  2   | 160           | EV_REDRAW              | Q_RTC
 *  T_Alarm    | 250 ms             |  3   | 128           | EV_REDRAW              | EV_DISMISS
 *  T_Display  | EV_REDRAW / 500 ms |  1   | 256           | -                      | EV_REDRAW
 *
 *  M_DATA (xDataMutex) bao ve g_app (xem app_ipc.h).
 *  Uu tien cao nhat dung trong app = 3 < configMAX_PRIORITIES (5).
 *  Tong stack task = 992 words (~3.9 KB) + Idle 128 words -> heap 10 KB du dung.
 */

#include "FreeRTOS.h"
#include "task.h"

/* ------------------------------------------------------------------ */
/* Uu tien (so lon = uu tien cao)                                      */
/* ------------------------------------------------------------------ */
#define TASK_BUTTON_PRIO        (tskIDLE_PRIORITY + 3)
#define TASK_CTRL_PRIO          (tskIDLE_PRIORITY + 3)
#define TASK_ALARM_PRIO         (tskIDLE_PRIORITY + 3)
#define TASK_POT_PRIO           (tskIDLE_PRIORITY + 2)
#define TASK_RTC_PRIO           (tskIDLE_PRIORITY + 2)
#define TASK_DISPLAY_PRIO       (tskIDLE_PRIORITY + 1)

/* ------------------------------------------------------------------ */
/* Stack (words, 1 word = 4 byte)                                      */
/* ------------------------------------------------------------------ */
#define TASK_BUTTON_STACK       128
#define TASK_POT_STACK          128
#define TASK_CTRL_STACK         192
#define TASK_RTC_STACK          160
#define TASK_ALARM_STACK        128
#define TASK_DISPLAY_STACK      256

/* ------------------------------------------------------------------ */
/* Chu ky / timeout (ms)                                               */
/* ------------------------------------------------------------------ */
#define TASK_BUTTON_PERIOD_MS   10
#define TASK_POT_PERIOD_MS      20
#define TASK_RTC_PERIOD_MS      250
#define TASK_ALARM_PERIOD_MS    250     /* cung la chu ky dao buzzer/LED khi keu */
#define TASK_DISPLAY_TIMEOUT_MS 500     /* ve lai dinh ky ke ca khi khong co EV_REDRAW */

#define DATA_MUTEX_TIMEOUT_MS   20      /* cho M_DATA toi da, khong block vinh vien */
#define Q_EVT_SEND_TIMEOUT_MS   0       /* T_Button/T_Pot: queue day thi bo su kien */
#define Q_RTC_SEND_TIMEOUT_MS   10      /* T_Ctrl -> T_RTC */

/* ------------------------------------------------------------------ */
/* Queue (do dai; kieu phan tu dinh nghia trong app_ipc.h)             */
/* ------------------------------------------------------------------ */
#define Q_EVT_LEN               8       /* phan tu: Evt    */
#define Q_RTC_LEN               4       /* phan tu: RtcCmd */

/* ------------------------------------------------------------------ */
/* Event group bits (xEventGroup)                                      */
/* ------------------------------------------------------------------ */
#define EV_REDRAW               (1U << 0)   /* -> T_Display: ve lai          */
#define EV_DISMISS              (1U << 1)   /* -> T_Alarm  : tat chuong      */

/* ------------------------------------------------------------------ */
/* Nguong nhap lieu                                                    */
/* ------------------------------------------------------------------ */
#define BTN_DEBOUNCE_SAMPLES    3       /* 3 mau x 10 ms = 30 ms */
#define POT_AVG_SAMPLES         8       /* T_Pot lay trung binh 8 mau ADC */
#define POT_ADC_MAX             4095U   /* ADC 12 bit */
#define POT_REPORT_DELTA        12      /* T_Pot chi gui khi ADC lech >= 12 so voi lan gui truoc */
#define POT_ARM_DELTA           40      /* T_Ctrl bo qua bien tro sau khi vao buoc moi cho den khi nup lech >= 40 */
#define POT_ONOFF_THRESHOLD     2048U   /* S5: < nguong = TAT, >= nguong = BAT */

/* ------------------------------------------------------------------ */
/* Bo tri RAM DS1302 (31 byte) luu gio hen                             */
/* ------------------------------------------------------------------ */
#define DS_RAM_ADDR_MAGIC       0       /* = DS_RAM_MAGIC neu du lieu hop le */
#define DS_RAM_ADDR_ALARM_H     1       /* 0..23 */
#define DS_RAM_ADDR_ALARM_M     2       /* 0..59 */
#define DS_RAM_ADDR_ALARM_EN    3       /* 0 = tat, 1 = bat */
#define DS_RAM_MAGIC            0xA5

#endif
