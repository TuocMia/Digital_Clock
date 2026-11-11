#include "FreeRTOS.h"
#include "task.h"

/* Can configCHECK_FOR_STACK_OVERFLOW = 2 trong FreeRTOSConfig.h */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask; (void)pcTaskName;
    for (;;);
}
