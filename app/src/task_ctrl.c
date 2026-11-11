#include "FreeRTOS.h"
#include "task.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "task_ctrl.h"

static TaskHandle_t xCtrlTaskHandle = NULL;

static void prvCtrlTask(void *pvParameters)
{
    (void)pvParameters;
    Evt e;

    for (;;) {
        if (xQueueReceive(xEvtQueue, &e, portMAX_DELAY) == pdPASS) {
            /* TODO: may trang thai S0..S5.
             * EVT_BTN: chuyen buoc (neu g_app.ring thi chi dat EV_DISMISS)
             * EVT_POT: cap nhat gio/phut dang chinh; gui RtcCmd qua xRtcQueue khi roi S2, S5 */
        }
    }
}

BaseType_t task_ctrl_create(void)
{
    return xTaskCreate(prvCtrlTask, "CTRL", TASK_CTRL_STACK, NULL, TASK_CTRL_PRIO, &xCtrlTaskHandle);
}
