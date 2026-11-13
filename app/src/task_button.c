#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bsp.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_button.h"

static TaskHandle_t xButtonTaskHandle = NULL;

static void prvButtonTask(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(TASK_BUTTON_PERIOD_MS);

    uint8_t stable_state = 0U;
    uint8_t debounce_cnt = 0U;

    for (;;) {
        uint8_t raw = bsp_button_pressed() ? 1U : 0U;

        if (raw == stable_state) {
            debounce_cnt = 0U;
        } else {
            debounce_cnt++;
            if (debounce_cnt >= BTN_DEBOUNCE_SAMPLES) {
                stable_state = raw;
                debounce_cnt = 0U;

                /* Chi gui su kien khi phat hien canh nhan (0 -> 1) */
                if (stable_state == 1U) {
                    Evt e = { .type = EVT_BTN, .val = 0U };
                    (void)xQueueSend(xEvtQueue, &e, pdMS_TO_TICKS(Q_EVT_SEND_TIMEOUT_MS));
                }
            }
        }

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

BaseType_t task_button_create(void)
{
    return xTaskCreate(prvButtonTask, "BUTTON", TASK_BUTTON_STACK, NULL, TASK_BUTTON_PRIO, &xButtonTaskHandle);
}
