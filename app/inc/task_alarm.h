/**
 * @file    task_alarm.h
 * @brief   Alarm comparison, buzzer, and LED alert FreeRTOS task.
 */

#ifndef TASK_ALARM_H
#define TASK_ALARM_H

#include "FreeRTOS.h"

/**
 * @brief  Create the alarm monitoring task (T_Alarm).
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_alarm_create(void);

#endif
