#ifndef APP_IPC_H
#define APP_IPC_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* ---- Buoc cua may trang thai (T_Ctrl) ---- */
typedef enum {
    ST_VIEW = 0,        /* S0: xem gio                         */
    ST_SET_HOUR,        /* S1: chinh gio: gio      (0..23)     */
    ST_SET_MIN,         /* S2: chinh gio: phut     (0..59)     */
    ST_ALARM_HOUR,      /* S3: hen gio: gio        (0..23)     */
    ST_ALARM_MIN,       /* S4: hen gio: phut       (0..59)     */
    ST_ALARM_EN,        /* S5: bat/tat bao thuc                */
    ST_COUNT
} app_state_t;

/* ---- Q_EVT: T_Button, T_Pot -> T_Ctrl ---- */
typedef enum { EVT_BTN = 0, EVT_POT } evt_type_t;
typedef struct {
    uint8_t  type;      /* evt_type_t */
    uint16_t val;       /* EVT_POT: gia tri ADC trung binh 0..4095; EVT_BTN: 0 */
} Evt;

/* ---- Q_RTC: T_Ctrl -> T_RTC ---- */
typedef enum {
    CMD_SET_TIME = 0,   /* ghi h:m vao DS1302, giay = 0        */
    CMD_SET_ALARM       /* ghi h, m, en vao RAM DS1302         */
} rtc_cmd_t;
typedef struct {
    uint8_t cmd;        /* rtc_cmd_t */
    uint8_t h, m;
    uint8_t en;         /* CMD_SET_ALARM: 1 = bat */
} RtcCmd;

/* ---- Du lieu chung, bao ve boi xDataMutex (M_DATA) ----
 * Chu so huu (task duoc phep ghi) ghi o cot ben phai. Task khac chi doc. */
typedef struct {
    uint8_t sec, min, hour;             /* gio hien tai               (T_RTC)  */
    uint8_t date, month, year;          /* ngay; year 0..99 = 2000+   (T_RTC)  */
    uint8_t rtc_ok;                     /* 1 = doc DS1302 thanh cong  (T_RTC)  */
    uint8_t state;                      /* app_state_t                (T_Ctrl) */
    uint8_t edit_h, edit_m;             /* gia tri dang chinh S1..S4  (T_Ctrl) */
    uint8_t edit_en;                    /* gia tri dang chinh S5      (T_Ctrl) */
    uint8_t ah, am, aen;                /* gio hen dang hieu luc      (T_Ctrl; T_RTC nap tu RAM luc khoi dong) */
    uint8_t ring;                       /* 1 = chuong dang keu        (T_Alarm) */
} AppData;

extern QueueHandle_t      xEvtQueue;    /* Q_EVT  */
extern QueueHandle_t      xRtcQueue;    /* Q_RTC  */
extern SemaphoreHandle_t  xDataMutex;   /* M_DATA */
extern EventGroupHandle_t xEventGroup;  /* EV_REDRAW, EV_DISMISS (app_cfg.h) */
extern AppData            g_app;

/* Tao queue, mutex, event group. Goi trong main truoc vTaskStartScheduler().
 * Tra ve pdPASS neu tao du tat ca doi tuong. */
BaseType_t app_ipc_init(void);

/* Khoa / mo M_DATA voi timeout DATA_MUTEX_TIMEOUT_MS. Tra ve pdTRUE neu lay duoc. */
BaseType_t app_data_lock(void);
void       app_data_unlock(void);

#endif
