#ifndef APP_IPC_H
#define APP_IPC_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* ---- Q_EVT: task_button, task_pot -> task_ctrl ---- */
typedef enum { EVT_BTN = 0, EVT_POT } evt_type_t;
typedef struct {
    uint8_t  type;     /* evt_type_t */
    uint16_t val;      /* EVT_POT: gia tri ADC 0..4095 */
} Evt;

/* ---- Q_RTC: task_ctrl -> task_rtc ---- */
typedef enum { CMD_SET_TIME = 0, CMD_SET_ALARM } rtc_cmd_t;
typedef struct {
    uint8_t cmd;       /* rtc_cmd_t */
    uint8_t h, m;
    uint8_t en;        /* CMD_SET_ALARM: 1 = bat */
} RtcCmd;

/* ---- Du lieu chung, bao ve boi xDataMutex ---- */
typedef struct {
    uint8_t sec, min, hour, date, month, year;   /* gio hien tai (task_rtc ghi) */
    uint8_t state;                               /* 0..5 = S0..S5 (task_ctrl ghi) */
    uint8_t edit_h, edit_m;                      /* gia tri dang chinh (task_ctrl ghi) */
    uint8_t ah, am, aen;                         /* gio hen va co bat/tat */
    uint8_t ring;                                /* 1 = chuong dang keu (task_alarm ghi) */
} AppData;

extern QueueHandle_t      xEvtQueue;    /* Q_EVT */
extern QueueHandle_t      xRtcQueue;    /* Q_RTC */
extern SemaphoreHandle_t  xDataMutex;   /* M_DATA */
extern EventGroupHandle_t xEventGroup;  /* EV_REDRAW, EV_DISMISS (app_cfg.h) */
extern AppData            g_app;

/* Tao queue, mutex, event group. Goi trong main truoc vTaskStartScheduler(). */
void app_ipc_init(void);

#endif
