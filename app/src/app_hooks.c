/**
  ******************************************************************************
  * @file    app_hooks.c
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   FreeRTOS stack overflow and malloc failure hooks.
  ******************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief  FreeRTOS hook called when a task overflows its stack (configCHECK_FOR_STACK_OVERFLOW == 2).
 * @param  xTask      Handle of the offending task.
 * @param  pcTaskName Name of the offending task.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask; (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for (;;);
}

/**
 * @brief  FreeRTOS hook called when pvPortMalloc() fails due to insufficient heap (configUSE_MALLOC_FAILED_HOOK == 1).
 */
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;);
}
