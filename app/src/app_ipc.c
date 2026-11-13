#include <string.h>
#include "app_ipc.h"
#include "app_cfg.h"

QueueHandle_t      xEvtQueue   = NULL;
QueueHandle_t      xRtcQueue   = NULL;
SemaphoreHandle_t  xDataMutex  = NULL;
EventGroupHandle_t xEventGroup = NULL;
AppData            g_app;

BaseType_t app_ipc_init(void)
{
    memset(&g_app, 0, sizeof(g_app));
    g_app.state = ST_VIEW;

    xEvtQueue   = xQueueCreate(Q_EVT_LEN, sizeof(Evt));
    xRtcQueue   = xQueueCreate(Q_RTC_LEN, sizeof(RtcCmd));
    xDataMutex  = xSemaphoreCreateMutex();
    xEventGroup = xEventGroupCreate();

    if (xEvtQueue == NULL || xRtcQueue == NULL ||
        xDataMutex == NULL || xEventGroup == NULL) {
        return pdFAIL;
    }

    /* Ve khung dau tien ngay khi T_Display chay */
    xEventGroupSetBits(xEventGroup, EV_REDRAW);
    return pdPASS;
}

BaseType_t app_data_lock(void)
{
    return xSemaphoreTake(xDataMutex, pdMS_TO_TICKS(DATA_MUTEX_TIMEOUT_MS));
}

void app_data_unlock(void)
{
    xSemaphoreGive(xDataMutex);
}
