/**
 * @file    task_button.h
 * @brief   Button scanning and 30 ms debounce FreeRTOS task.
 */

#ifndef TASK_BUTTON_H
#define TASK_BUTTON_H

#include "FreeRTOS.h"

/**
 * @brief  Create the button scanning task (T_Button).
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_button_create(void);

#endif
