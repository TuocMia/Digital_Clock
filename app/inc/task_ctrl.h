/**
 * @file    task_ctrl.h
 * @brief   System state machine (S0..S5) and input coordinator FreeRTOS task.
 */

#ifndef TASK_CTRL_H
#define TASK_CTRL_H

#include "FreeRTOS.h"

/**
 * @brief  Create the control state-machine task (T_Ctrl).
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_ctrl_create(void);

#endif
