#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "st7789.h"
#include "ui_render.h"
#include "task_button.h"
#include "task_pot.h"
#include "task_ctrl.h"
#include "task_rtc.h"
#include "task_alarm.h"

/* ARM Semihosting SYS_WRITE0 (r0 = 4, r1 = null-terminated string) */
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

static void semihost_put_u32(uint32_t v)
{
    char buf[12];
    int8_t idx = 11;
    buf[idx--] = '\0';
    if (v == 0U) {
        buf[idx--] = '0';
    } else {
        while (v > 0U && idx >= 0) {
            buf[idx--] = (char)('0' + (v % 10U));
            v /= 10U;
        }
    }
    semihost_puts(&buf[idx + 1]);
}

static void print_task_hwm(const char *name, UBaseType_t alloc_words)
{
    TaskHandle_t h = xTaskGetHandle(name);
    if (h != NULL) {
        UBaseType_t hwm = uxTaskGetStackHighWaterMark(h);
        semihost_puts("  ");
        semihost_puts(name);
        semihost_puts(": free ");
        semihost_put_u32((uint32_t)hwm);
        semihost_puts(" / ");
        semihost_put_u32((uint32_t)alloc_words);
        semihost_puts(" words (used ");
        semihost_put_u32((uint32_t)(alloc_words - hwm));
        semihost_puts(" W)\r\n");
    }
}

static void prvStatsTask(void *pvParameters)
{
    (void)pvParameters;
    AppData snap;

    LCD_Init();
    if (app_data_lock() == pdTRUE) {
        snap = g_app;
        app_data_unlock();
        ui_draw_frame(&snap);
        ui_render(&snap);
    }

    /* Cho cac task chay on dinh 2 giay truoc khi in log */
    vTaskDelay(pdMS_TO_TICKS(2000));

    for (;;) {
        size_t free_heap = xPortGetFreeHeapSize();
        size_t min_heap  = xPortGetMinimumEverFreeHeapSize();

        semihost_puts("\r\n=== FreeRTOS Runtime Stats ===\r\n");
        semihost_puts("Free Heap     : ");
        semihost_put_u32((uint32_t)free_heap);
        semihost_puts(" bytes\r\n");
        semihost_puts("Min Ever Heap : ");
        semihost_put_u32((uint32_t)min_heap);
        semihost_puts(" bytes\r\n");
        semihost_puts("Task Stack High Water Mark:\r\n");

        print_task_hwm("BUTTON", TASK_BUTTON_STACK);
        print_task_hwm("POT",    TASK_POT_STACK);
        print_task_hwm("CTRL",   TASK_CTRL_STACK);
        print_task_hwm("RTC",    TASK_RTC_STACK);
        print_task_hwm("ALARM",  TASK_ALARM_STACK);
        print_task_hwm("STATS",  TASK_DISPLAY_STACK);

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

int main(void)
{
    NVIC_SetPriorityGrouping(3U);
    bsp_init();

    if (app_ipc_init() != pdPASS) { for (;;) {} }

    if (task_button_create() != pdPASS) { for (;;) {} }
    if (task_pot_create()    != pdPASS) { for (;;) {} }
    if (task_ctrl_create()   != pdPASS) { for (;;) {} }
    if (task_rtc_create()    != pdPASS) { for (;;) {} }
    if (task_alarm_create()  != pdPASS) { for (;;) {} }

    if (xTaskCreate(prvStatsTask, "STATS", TASK_DISPLAY_STACK, NULL, TASK_DISPLAY_PRIO, NULL) != pdPASS) {
        for (;;) {}
    }

    vTaskStartScheduler();
    for (;;) {}
}
