/**
 * @file    task_display.h
 * @brief   ST7789 LCD rendering FreeRTOS task.
 */

#ifndef TASK_DISPLAY_H
#define TASK_DISPLAY_H

#include "FreeRTOS.h"

/**
 * @brief  Create the LCD display task (T_Display).
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_display_create(void);

#endif
