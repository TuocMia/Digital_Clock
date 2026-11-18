/**
 * @file    task_rtc.h
 * @brief   DS1302 RTC polling and time/alarm write FreeRTOS task.
 */

#ifndef TASK_RTC_H
#define TASK_RTC_H

#include "FreeRTOS.h"

/**
 * @brief  Create the RTC communication task (T_RTC).
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_rtc_create(void);

#endif
