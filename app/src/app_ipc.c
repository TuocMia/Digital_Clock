#include "app_ipc.h"
#include "app_cfg.h"

QueueHandle_t      xEvtQueue   = NULL;
QueueHandle_t      xRtcQueue   = NULL;
SemaphoreHandle_t  xDataMutex  = NULL;
EventGroupHandle_t xEventGroup = NULL;
AppData            g_app;

void app_ipc_init(void)
{
    xEvtQueue   = xQueueCreate(Q_EVT_LEN, sizeof(Evt));
    xRtcQueue   = xQueueCreate(Q_RTC_LEN, sizeof(RtcCmd));
    xDataMutex  = xSemaphoreCreateMutex();
    xEventGroup = xEventGroupCreate();
}
