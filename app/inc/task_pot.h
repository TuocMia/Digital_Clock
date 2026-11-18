/**
 * @file    task_pot.h
 * @brief   Potentiometer ADC sampling and filtering FreeRTOS task.
 */

#ifndef TASK_POT_H
#define TASK_POT_H

#include "FreeRTOS.h"

/**
 * @brief  Create the potentiometer ADC task (T_Pot).
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_pot_create(void);

#endif
