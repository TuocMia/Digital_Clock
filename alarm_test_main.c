#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_alarm.h"

#define DISMISS_TEST_STACK  128
#define DISMISS_TEST_PRIO   (tskIDLE_PRIORITY + 1)

static void prvDismissTestTask(void *pvParameters)
{
    (void)pvParameters;

    /* Để còi kêu khoảng 3 giây rồi gửi sự kiện tắt. */
    vTaskDelay(pdMS_TO_TICKS(3000));
    xEventGroupSetBits(xEventGroup, EV_DISMISS);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    if (app_ipc_init() != pdPASS) {
        for (;;) {
        }
    }

    /* Giả lập RTC đang ở đúng thời điểm báo thức. */
    g_app.rtc_ok = 1U;
    g_app.hour = 7U;
    g_app.min = 30U;
    g_app.ah = 7U;
    g_app.am = 30U;
    g_app.aen = 1U;

    if (task_alarm_create() != pdPASS) {
        for (;;) {
        }
    }

    if (xTaskCreate(prvDismissTestTask,
                    "DISMISS",
                    DISMISS_TEST_STACK,
                    NULL,
                    DISMISS_TEST_PRIO,
                    NULL) != pdPASS) {
        for (;;) {
        }
    }

    vTaskStartScheduler();

    for (;;) {
    }
}