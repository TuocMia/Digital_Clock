/**
  ******************************************************************************
  * @file    app_ipc.c
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   FreeRTOS IPC object initialization and M_DATA mutex helpers.
  ******************************************************************************
  */

#include <string.h>
#include "app_ipc.h"
#include "app_cfg.h"

QueueHandle_t      xEvtQueue   = NULL;
QueueHandle_t      xRtcQueue   = NULL;
SemaphoreHandle_t  xDataMutex  = NULL;
EventGroupHandle_t xEventGroup = NULL;
AppData            g_app;

/**
 * @brief  Reset g_app and create Q_EVT, Q_RTC, M_DATA mutex, and xEventGroup.
 * @retval pdPASS if all IPC objects were created, pdFAIL otherwise.
 */
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

/**
 * @brief  Acquire M_DATA mutex protecting g_app with timeout DATA_MUTEX_TIMEOUT_MS.
 * @retval pdTRUE if acquired, pdFALSE on timeout.
 */
BaseType_t app_data_lock(void)
{
    return xSemaphoreTake(xDataMutex, pdMS_TO_TICKS(DATA_MUTEX_TIMEOUT_MS));
}

/**
 * @brief  Release M_DATA mutex after reading or updating g_app.
 */
void app_data_unlock(void)
{
    xSemaphoreGive(xDataMutex);
}
