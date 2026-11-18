/**
  ******************************************************************************
  * @file    app_ipc.h
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   Shared data structures, queues, mutex, and event group handles.
  ******************************************************************************
  */

#ifndef APP_IPC_H
#define APP_IPC_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/**
  * @brief Operating states of the system state machine (managed by T_Ctrl).
  */
typedef enum {
    ST_VIEW = 0,   /**< S0: Normal clock display mode */
    ST_SET_HOUR,   /**< S1: Adjust current hour (0..23) */
    ST_SET_MIN,    /**< S2: Adjust current minute (0..59), writes DS1302 on exit */
    ST_ALARM_HOUR, /**< S3: Adjust alarm hour (0..23) */
    ST_ALARM_MIN,  /**< S4: Adjust alarm minute (0..59) */
    ST_ALARM_EN,   /**< S5: Enable/disable alarm, writes DS1302 RAM on exit */
    ST_COUNT       /**< Total number of states */
} app_state_t;

/**
  * @brief Input event types sent over xEvtQueue (Q_EVT).
  */
typedef enum {
    EVT_BTN = 0, /**< Debounced push-button press event from T_Button */
    EVT_POT      /**< Filtered potentiometer ADC value event from T_Pot */
} evt_type_t;

/**
  * @brief Event message queued from T_Button and T_Pot to T_Ctrl.
  */
typedef struct {
    uint8_t  type;  /**< Event type (evt_type_t) */
    uint16_t value; /**< Averaged 12-bit ADC value (0..4095) for EVT_POT; 0 for EVT_BTN */
} Evt;

/**
  * @brief RTC command types sent over xRtcQueue (Q_RTC).
  */
typedef enum {
    CMD_SET_TIME = 0, /**< Write hours and minutes to DS1302 (seconds reset to 0) */
    CMD_SET_ALARM     /**< Persist alarm hour, minute, and enable flag to DS1302 RAM */
} rtc_cmd_t;

/**
  * @brief Command message queued from T_Ctrl to T_RTC.
  */
typedef struct {
    uint8_t command;       /**< Command type (rtc_cmd_t) */
    uint8_t hour;          /**< Target hour (0..23) */
    uint8_t minute;        /**< Target minute (0..59) */
    uint8_t alarm_enabled; /**< Alarm enable flag for CMD_SET_ALARM (1 = enabled, 0 = disabled) */
} RtcCmd;

/**
  * @brief Shared application state protected by xDataMutex (M_DATA).
  * @note  Each field has a single owning writer task; other tasks only read.
  */
typedef struct {
    uint8_t second;             /**< Current second 0..59 (written by T_RTC) */
    uint8_t minute;             /**< Current minute 0..59 (written by T_RTC) */
    uint8_t hour;               /**< Current hour 0..23 (written by T_RTC) */
    uint8_t date;               /**< Day of month 1..31 (written by T_RTC) */
    uint8_t month;              /**< Month 1..12 (written by T_RTC) */
    uint8_t year;               /**< Year 0..99 representing 2000..2099 (written by T_RTC) */
    uint8_t rtc_ok;             /**< 1 if last DS1302 read succeeded, 0 on error (written by T_RTC) */
    uint8_t state;              /**< Current state machine step app_state_t (written by T_Ctrl) */
    uint8_t edit_hour;          /**< Temporary hour being edited in S1..S4 (written by T_Ctrl) */
    uint8_t edit_minute;        /**< Temporary minute being edited in S1..S4 (written by T_Ctrl) */
    uint8_t edit_alarm_enabled; /**< Temporary alarm enable flag in S5 (written by T_Ctrl) */
    uint8_t alarm_hour;         /**< Active alarm hour 0..23 (written by T_Ctrl; loaded at boot by T_RTC) */
    uint8_t alarm_minute;       /**< Active alarm minute 0..59 (written by T_Ctrl; loaded at boot by T_RTC) */
    uint8_t alarm_enabled;      /**< Active alarm enable flag (written by T_Ctrl; loaded at boot by T_RTC) */
    uint8_t alarm_ringing;      /**< 1 while alarm is actively ringing, 0 otherwise (written by T_Alarm) */
} AppData;

extern QueueHandle_t      xEvtQueue;    /**< Q_EVT: T_Button, T_Pot -> T_Ctrl */
extern QueueHandle_t      xRtcQueue;    /**< Q_RTC: T_Ctrl -> T_RTC */
extern SemaphoreHandle_t  xDataMutex;   /**< M_DATA: Mutex protecting g_app */
extern EventGroupHandle_t xEventGroup;  /**< Event group for EV_REDRAW and EV_DISMISS */
extern AppData            g_app;        /**< Global shared application state */

/**
  * @brief  Create queues, mutex, and event group before starting the scheduler.
  * @retval pdPASS if all IPC objects were created, pdFAIL otherwise.
  */
BaseType_t app_ipc_init(void);

/**
  * @brief  Acquire M_DATA mutex with timeout DATA_MUTEX_TIMEOUT_MS.
  * @retval pdTRUE if mutex was acquired, pdFALSE on timeout.
  */
BaseType_t app_data_lock(void);

/**
  * @brief  Release M_DATA mutex.
  */
void app_data_unlock(void);

#endif
