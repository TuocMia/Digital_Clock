/**
 * @file    app_cfg.h
 * @brief   FreeRTOS task priorities, stack sizes, periods, IPC sizes, and thresholds.
 */

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
#define TASK_BUTTON_PRIO        (tskIDLE_PRIORITY + 3) /**< Priority of T_Button */
#define TASK_CTRL_PRIO          (tskIDLE_PRIORITY + 3) /**< Priority of T_Ctrl */
#define TASK_ALARM_PRIO         (tskIDLE_PRIORITY + 3) /**< Priority of T_Alarm */
#define TASK_POT_PRIO           (tskIDLE_PRIORITY + 2) /**< Priority of T_Pot */
#define TASK_RTC_PRIO           (tskIDLE_PRIORITY + 2) /**< Priority of T_RTC */
#define TASK_DISPLAY_PRIO       (tskIDLE_PRIORITY + 1) /**< Priority of T_Display */

/* ------------------------------------------------------------------ */
/* Stack (words, 1 word = 4 byte)                                      */
/* ------------------------------------------------------------------ */
#define TASK_BUTTON_STACK       128 /**< Stack size of T_Button in 32-bit words */
#define TASK_POT_STACK          128 /**< Stack size of T_Pot in 32-bit words */
#define TASK_CTRL_STACK         192 /**< Stack size of T_Ctrl in 32-bit words */
#define TASK_RTC_STACK          160 /**< Stack size of T_RTC in 32-bit words */
#define TASK_ALARM_STACK        128 /**< Stack size of T_Alarm in 32-bit words */
#define TASK_DISPLAY_STACK      256 /**< Stack size of T_Display in 32-bit words */

/* ------------------------------------------------------------------ */
/* Chu ky / timeout (ms)                                               */
/* ------------------------------------------------------------------ */
#define TASK_BUTTON_PERIOD_MS   10      /**< Button scan period in ms */
#define TASK_POT_PERIOD_MS      20      /**< Potentiometer ADC sampling period in ms */
#define TASK_RTC_PERIOD_MS      250     /**< DS1302 RTC polling period in ms */
#define TASK_ALARM_PERIOD_MS    250     /**< Alarm check and buzzer/LED cadence tick in ms */
#define TASK_DISPLAY_TIMEOUT_MS 500     /**< Max wait for EV_REDRAW before periodic LCD refresh in ms */

#define DATA_MUTEX_TIMEOUT_MS   20      /**< Max wait for M_DATA mutex in ms */
#define Q_EVT_SEND_TIMEOUT_MS   0       /**< T_Button/T_Pot drop event if Q_EVT is full */
#define Q_RTC_SEND_TIMEOUT_MS   10      /**< Max wait when T_Ctrl queues a command to T_RTC */

/* ------------------------------------------------------------------ */
/* Queue (do dai; kieu phan tu dinh nghia trong app_ipc.h)             */
/* ------------------------------------------------------------------ */
#define Q_EVT_LEN               8       /**< Capacity of xEvtQueue (elements of Evt) */
#define Q_RTC_LEN               4       /**< Capacity of xRtcQueue (elements of RtcCmd) */

/* ------------------------------------------------------------------ */
/* Event group bits (xEventGroup)                                      */
/* ------------------------------------------------------------------ */
#define EV_REDRAW               (1U << 0)   /**< Event bit -> T_Display: redraw screen */
#define EV_DISMISS              (1U << 1)   /**< Event bit -> T_Alarm: dismiss ringing alarm */

/* ------------------------------------------------------------------ */
/* Nguong nhap lieu                                                    */
/* ------------------------------------------------------------------ */
#define BTN_DEBOUNCE_SAMPLES    3       /**< Consecutive samples for debounce (3 x 10 ms = 30 ms) */
#define POT_AVG_SAMPLES         8       /**< Number of ADC samples averaged per T_Pot cycle */
#define POT_ADC_MAX             4095U   /**< 12-bit ADC full-scale value */
#define POT_REPORT_DELTA        12      /**< Min ADC delta for T_Pot to emit EVT_POT */
#define POT_ARM_DELTA           24      /**< Min ADC delta to arm potentiometer after state change */
#define POT_ONOFF_THRESHOLD     2048U   /**< S8 (ST_ALARM_EN): < threshold = OFF, >= threshold = ON */

/* ------------------------------------------------------------------ */
/* Bo tri RAM DS1302 (31 byte) luu gio hen                             */
/* ------------------------------------------------------------------ */
#define DS_RAM_ADDR_MAGIC       0       /**< DS1302 RAM index 0: magic byte (DS_RAM_MAGIC if valid) */
#define DS_RAM_ADDR_ALARM_H     1       /**< DS1302 RAM index 1: alarm hour (0..23) */
#define DS_RAM_ADDR_ALARM_M     2       /**< DS1302 RAM index 2: alarm minute (0..59) */
#define DS_RAM_ADDR_ALARM_EN    3       /**< DS1302 RAM index 3: alarm enable flag (0 or 1) */
#define DS_RAM_MAGIC            0xA5    /**< Magic signature validating stored alarm data */

#endif
