/**
 * @file    main.c
 * @brief   Entry point for the Digital Clock FreeRTOS application.
 */

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_button.h"
#include "task_pot.h"
#include "task_ctrl.h"
#include "task_rtc.h"
#include "task_alarm.h"
#include "task_display.h"

#ifdef ENABLE_RTOS_STATS
/**
 * @brief  Write a null-terminated string to OpenOCD via ARM Semihosting (SYS_WRITE0).
 * @param  s Null-terminated string to output.
 */
static void semihost_puts(const char *s)
{
    __asm volatile (
        "mov r0, #4\n"
        "mov r1, %0\n"
        "bkpt #0xAB\n"
        :
        : "r" (s)
        : "r0", "r1", "memory"
    );
}

/**
 * @brief  Append a null-terminated string to a bounded output buffer.
 * @param  dst Destination cursor pointer (updated in place).
 * @param  end Exclusive end pointer of the buffer.
 * @param  s   Null-terminated source string.
 */
static void buf_append_str(char **dst, const char *end, const char *s)
{
    while (*s != '\0' && (*dst + 1) < end) {
        *(*dst)++ = *s++;
    }
    **dst = '\0';
}

/**
 * @brief  Append an unsigned 32-bit decimal integer to a bounded output buffer.
 * @param  dst Destination cursor pointer (updated in place).
 * @param  end Exclusive end pointer of the buffer.
 * @param  v   Value to format.
 */
static void buf_append_u32(char **dst, const char *end, uint32_t v)
{
    char tmp[12];
    int8_t idx = 11;
    tmp[idx--] = '\0';
    if (v == 0U) {
        tmp[idx--] = '0';
    } else {
        while (v > 0U && idx >= 0) {
            tmp[idx--] = (char)('0' + (v % 10U));
            v /= 10U;
        }
    }
    buf_append_str(dst, end, &tmp[idx + 1]);
}

/**
 * @brief  Format stack high-water mark for a named FreeRTOS task into the buffer.
 * @param  dst         Destination cursor pointer.
 * @param  end         Exclusive end pointer of the buffer.
 * @param  name        Task name passed to xTaskCreate().
 * @param  alloc_words Allocated stack size in 32-bit words.
 */
static void buf_append_hwm(char **dst, const char *end, const char *name, UBaseType_t alloc_words)
{
    TaskHandle_t h = xTaskGetHandle(name);
    if (h != NULL) {
        UBaseType_t hwm = uxTaskGetStackHighWaterMark(h);
        buf_append_str(dst, end, "  ");
        buf_append_str(dst, end, name);
        buf_append_str(dst, end, ": free ");
        buf_append_u32(dst, end, (uint32_t)hwm);
        buf_append_str(dst, end, " / ");
        buf_append_u32(dst, end, (uint32_t)alloc_words);
        buf_append_str(dst, end, " W (used ");
        buf_append_u32(dst, end, (uint32_t)(alloc_words - hwm));
        buf_append_str(dst, end, " W)\r\n");
    }
}

/**
 * @brief  Periodic stats logger task running alongside all 6 main tasks when ENABLE_RTOS_STATS is defined.
 * @note   Formats the entire report in RAM and triggers a single semihosting bkpt per 5 s to minimize CPU stall.
 * @param  pvParameters Unused task argument.
 */
static void prvMainStatsTask(void *pvParameters)
{
    (void)pvParameters;
    char log_buf[384];

    vTaskDelay(pdMS_TO_TICKS(5000));

    for (;;) {
        char *p = log_buf;
        const char *end = log_buf + sizeof(log_buf);
        size_t free_heap = xPortGetFreeHeapSize();
        size_t min_heap  = xPortGetMinimumEverFreeHeapSize();

        p[0] = '\0';
        buf_append_str(&p, end, "\r\n=== FreeRTOS Runtime Stats (main) ===\r\n");
        buf_append_str(&p, end, "Free Heap     : ");
        buf_append_u32(&p, end, (uint32_t)free_heap);
        buf_append_str(&p, end, " bytes\r\nMin Ever Heap : ");
        buf_append_u32(&p, end, (uint32_t)min_heap);
        buf_append_str(&p, end, " bytes\r\nTask Stack High Water Mark:\r\n");

        buf_append_hwm(&p, end, "BUTTON",  TASK_BUTTON_STACK);
        buf_append_hwm(&p, end, "POT",     TASK_POT_STACK);
        buf_append_hwm(&p, end, "CTRL",    TASK_CTRL_STACK);
        buf_append_hwm(&p, end, "RTC",     TASK_RTC_STACK);
        buf_append_hwm(&p, end, "ALARM",   TASK_ALARM_STACK);
        buf_append_hwm(&p, end, "DISPLAY", TASK_DISPLAY_STACK);

        /* Single semihosting trap for the whole block */
        semihost_puts(log_buf);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
#endif

/**
 * @brief  Initialize hardware, FreeRTOS IPC objects, create all 6 tasks, and start the scheduler.
 * @return Never returns under normal operation.
 */
int main(void)
{
    /* 1. Hardware & BSP init */
    NVIC_SetPriorityGrouping(3U);
    bsp_init();

    /* 2. FreeRTOS IPC Object Creation */
    if (app_ipc_init() != pdPASS) {
        for (;;) {}
    }

    /* 3. Create all 6 FreeRTOS Tasks */
    if (task_button_create()  != pdPASS) { for (;;) {} }
    if (task_pot_create()     != pdPASS) { for (;;) {} }
    if (task_ctrl_create()    != pdPASS) { for (;;) {} }
    if (task_rtc_create()     != pdPASS) { for (;;) {} }
    if (task_alarm_create()   != pdPASS) { for (;;) {} }
    if (task_display_create() != pdPASS) { for (;;) {} }

#ifdef ENABLE_RTOS_STATS
    if (xTaskCreate(prvMainStatsTask, "STATS", 256, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        for (;;) {}
    }
#endif

    /* 4. Start FreeRTOS Scheduler */
    vTaskStartScheduler();

    /* Chuong trinh khong bao gio chay den day neu RTOS du heap */
    for (;;) {}
}
