/**
  ******************************************************************************
  * @file    task_rtc.c
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   DS1302 RTC polling and time/alarm write FreeRTOS task.
  ******************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#include "app_cfg.h"
#include "app_ipc.h"
#include "hal_ds1302.h"
#include "task_rtc.h"

static TaskHandle_t xRtcTaskHandle = NULL;

/**
 * @brief  Convert DS1302 read status into the boolean rtc_ok flag.
 * @param  status Return code from hal_ds1302_read_time().
 * @return 1 if HAL_DS1302_OK, 0 otherwise.
 */
static uint8_t rtc_time_status_ok(hal_ds1302_status_t status)
{
    return (status == HAL_DS1302_OK) ? 1U : 0U;
}

/**
 * @brief  Publish DS1302 date/time to g_app under M_DATA and set EV_REDRAW if any field changed.
 * @param  datetime Pointer to the newly read date/time.
 * @param  read_ok  1 if DS1302 clock is valid and running, 0 otherwise.
 */
static void rtc_publish_time(const hal_ds1302_datetime_t *datetime,
                             uint8_t read_ok)
{
    uint8_t redraw = 0U;

    if (app_data_lock() != pdTRUE) {
        return;
    }

    if (g_app.second != datetime->seconds ||
        g_app.minute != datetime->minutes ||
        g_app.hour != datetime->hours ||
        g_app.date != datetime->date ||
        g_app.month != datetime->month ||
        g_app.year != datetime->year ||
        g_app.rtc_ok != read_ok) {
        redraw = 1U;
    }

    g_app.second = datetime->seconds;
    g_app.minute = datetime->minutes;
    g_app.hour = datetime->hours;
    g_app.date = datetime->date;
    g_app.month = datetime->month;
    g_app.year = datetime->year;
    g_app.rtc_ok = read_ok;

    app_data_unlock();

    if (redraw != 0U) {
        xEventGroupSetBits(xEventGroup, EV_REDRAW);
    }
}

/**
 * @brief  Clear g_app.rtc_ok on DS1302 communication/validation error and trigger EV_REDRAW.
 */
static void rtc_publish_error(void)
{
    uint8_t redraw = 0U;

    if (app_data_lock() != pdTRUE) {
        return;
    }

    if (g_app.rtc_ok != 0U) {
        g_app.rtc_ok = 0U;
        redraw = 1U;
    }

    app_data_unlock();

    if (redraw != 0U) {
        xEventGroupSetBits(xEventGroup, EV_REDRAW);
    }
}

/**
 * @brief  Load saved alarm hour, minute, and enable flag from DS1302 RAM at startup.
 */
static void rtc_load_alarm_from_ram(void)
{
    uint8_t magic;
    uint8_t alarm_h;
    uint8_t alarm_m;
    uint8_t alarm_en;
    uint8_t valid = 0U;

    if (hal_ds1302_read_ram(DS_RAM_ADDR_MAGIC, &magic) == HAL_DS1302_OK &&
        magic == DS_RAM_MAGIC &&
        hal_ds1302_read_ram(DS_RAM_ADDR_ALARM_H, &alarm_h) == HAL_DS1302_OK &&
        hal_ds1302_read_ram(DS_RAM_ADDR_ALARM_M, &alarm_m) == HAL_DS1302_OK &&
        hal_ds1302_read_ram(DS_RAM_ADDR_ALARM_EN, &alarm_en) == HAL_DS1302_OK &&
        alarm_h < 24U && alarm_m < 60U && alarm_en <= 1U) {
        valid = 1U;
    }

    if (app_data_lock() != pdTRUE) {
        return;
    }

    if (valid != 0U) {
        g_app.alarm_hour = alarm_h;
        g_app.alarm_minute = alarm_m;
        g_app.alarm_enabled = alarm_en;
    } else {
        g_app.alarm_hour = 0U;
        g_app.alarm_minute = 0U;
        g_app.alarm_enabled = 0U;
    }

    app_data_unlock();
    xEventGroupSetBits(xEventGroup, EV_REDRAW);
}

/**
 * @brief  Persist alarm settings to DS1302 RAM using magic-byte write-last protection.
 * @param  command Pointer to CMD_SET_ALARM RtcCmd payload.
 * @retval HAL_DS1302_OK on success or error status.
 */
static hal_ds1302_status_t rtc_store_alarm(const RtcCmd *command)
{
    hal_ds1302_status_t status;

    if (command->hour >= 24U || command->minute >= 60U || command->alarm_enabled > 1U) {
        return HAL_DS1302_INVALID_ARGUMENT;
    }

    /* Mark the stored alarm invalid until all fields have been written. */
    status = hal_ds1302_write_ram(DS_RAM_ADDR_MAGIC, 0U);
    if (status != HAL_DS1302_OK) {
        return status;
    }

    status = hal_ds1302_write_ram(DS_RAM_ADDR_ALARM_H, command->hour);
    if (status != HAL_DS1302_OK) {
        return status;
    }

    status = hal_ds1302_write_ram(DS_RAM_ADDR_ALARM_M, command->minute);
    if (status != HAL_DS1302_OK) {
        return status;
    }

    status = hal_ds1302_write_ram(DS_RAM_ADDR_ALARM_EN, command->alarm_enabled);
    if (status != HAL_DS1302_OK) {
        return status;
    }

    /* Write the magic value last, so partial data is not accepted at boot. */
    return hal_ds1302_write_ram(DS_RAM_ADDR_MAGIC, DS_RAM_MAGIC);
}

/**
 * @brief  Execute one RtcCmd (CMD_SET_TIME, CMD_SET_DATE, or CMD_SET_ALARM) against DS1302.
 * @param  command Pointer to dequeued command.
 */
static void rtc_handle_command(const RtcCmd *command)
{
    hal_ds1302_status_t status;

    switch (command->command) {
    case CMD_SET_TIME: {
        hal_ds1302_datetime_t datetime;

        if (command->hour >= 24U || command->minute >= 60U) {
            rtc_publish_error();
            break;
        }

        /* Read first to preserve the current date and weekday; fallback if RTC uninitialized. */
        status = hal_ds1302_read_time(&datetime);
        if (status != HAL_DS1302_OK &&
            status != HAL_DS1302_CLOCK_STOPPED) {
            datetime.date    = 18U;
            datetime.month   = 11U;
            datetime.year    = 26U;
            datetime.weekday = 1U;
        }

        datetime.hours = command->hour;
        datetime.minutes = command->minute;
        datetime.seconds = 0U;

        status = hal_ds1302_write_time(&datetime);
        if (status != HAL_DS1302_OK) {
            rtc_publish_error();
            break;
        }

        xEventGroupSetBits(xEventGroup, EV_REDRAW);
        break;
    }

    case CMD_SET_DATE: {
        hal_ds1302_datetime_t datetime;

        if (command->date < 1U || command->date > 31U ||
            command->month < 1U || command->month > 12U ||
            command->year > 99U) {
            rtc_publish_error();
            break;
        }

        /* Read first to preserve current hours, minutes, and seconds. */
        status = hal_ds1302_read_time(&datetime);
        if (status != HAL_DS1302_OK &&
            status != HAL_DS1302_CLOCK_STOPPED) {
            datetime.hours   = 0U;
            datetime.minutes = 0U;
            datetime.seconds = 0U;
            datetime.weekday = 1U;
        }

        datetime.date  = command->date;
        datetime.month = command->month;
        datetime.year  = command->year;

        status = hal_ds1302_write_time(&datetime);
        if (status != HAL_DS1302_OK) {
            rtc_publish_error();
            break;
        }

        xEventGroupSetBits(xEventGroup, EV_REDRAW);
        break;
    }

    case CMD_SET_ALARM:
        status = rtc_store_alarm(command);
        if (status != HAL_DS1302_OK) {
            rtc_publish_error();
        } else {
            xEventGroupSetBits(xEventGroup, EV_REDRAW);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief  FreeRTOS task that polls DS1302 every 250 ms and drains Q_RTC write commands.
 * @param  pvParameters Unused task argument.
 */
static void prvRtcTask(void *pvParameters)
{
    TickType_t last_wake_time;
    const TickType_t period = pdMS_TO_TICKS(TASK_RTC_PERIOD_MS);

    (void)pvParameters;

    hal_ds1302_init();
    rtc_load_alarm_from_ram();

    last_wake_time = xTaskGetTickCount();

    for (;;) {
        hal_ds1302_datetime_t datetime;
        hal_ds1302_status_t status;
        RtcCmd command;

        status = hal_ds1302_read_time(&datetime);
        if (status == HAL_DS1302_OK ||
            status == HAL_DS1302_CLOCK_STOPPED) {
            rtc_publish_time(&datetime, rtc_time_status_ok(status));
        } else {
            rtc_publish_error();
        }

        /* Process queued commands without blocking the periodic RTC read. */
        while (xQueueReceive(xRtcQueue, &command, 0U) == pdPASS) {
            rtc_handle_command(&command);
        }

        vTaskDelayUntil(&last_wake_time, period);
    }
}

/**
 * @brief  Create the T_RTC FreeRTOS task.
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_rtc_create(void)
{
    return xTaskCreate(prvRtcTask,
                       "RTC",
                       TASK_RTC_STACK,
                       NULL,
                       TASK_RTC_PRIO,
                       &xRtcTaskHandle);
}