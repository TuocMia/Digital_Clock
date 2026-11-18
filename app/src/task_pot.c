/**
  ******************************************************************************
  * @file    task_pot.c
  * @author  Digital Clock Team (HTNN-N15)
  * @brief   Potentiometer ADC sampling and filtering FreeRTOS task.
  ******************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "app_cfg.h"
#include "app_ipc.h"
#include "hal_adc.h"
#include "task_pot.h"

static TaskHandle_t xPotTaskHandle = NULL;

/**
 * @brief  FreeRTOS task that samples ADC1 every 20 ms (8-sample average) and posts EVT_POT on delta >= 12.
 * @param  pvParameters Unused task argument.
 */
static void prvPotTask(void *pvParameters)
{
    TickType_t last_wake_time;
    const TickType_t period = pdMS_TO_TICKS(TASK_POT_PERIOD_MS);
    uint16_t last_reported_value = 0U;
    uint8_t has_reported_value = 0U;
    uint8_t adc_ready;

    (void)pvParameters;

    adc_ready = (hal_adc_init() == HAL_ADC_OK) ? 1U : 0U;
    last_wake_time = xTaskGetTickCount();

    for (;;) {
        if (adc_ready != 0U) {
            uint32_t sum = 0U;
            uint8_t sample_index;
            uint8_t samples_valid = 1U;

            for (sample_index = 0U;
                 sample_index < POT_AVG_SAMPLES;
                 sample_index++) {
                uint16_t sample;

                if (hal_adc_read(&sample) != HAL_ADC_OK) {
                    samples_valid = 0U;
                    break;
                }

                sum += sample;
            }

            if (samples_valid != 0U) {
                uint16_t average =
                    (uint16_t)((sum + (POT_AVG_SAMPLES / 2U)) /
                               POT_AVG_SAMPLES);
                
                average = (uint16_t)(average & 0xFFF0U);
                
                uint16_t difference =
                    (average >= last_reported_value)
                        ? (average - last_reported_value)
                        : (last_reported_value - average);

                if (has_reported_value == 0U ||
                    difference >= POT_REPORT_DELTA) {
                    Evt event = {
                        .type = EVT_POT,
                        .value = average
                    };

                    if (xQueueSend(
                            xEvtQueue,
                            &event,
                            pdMS_TO_TICKS(Q_EVT_SEND_TIMEOUT_MS)) == pdPASS) {
                        last_reported_value = average;
                        has_reported_value = 1U;
                    }
                }
            }
        }

        vTaskDelayUntil(&last_wake_time, period);
    }
}

/**
 * @brief  Create the T_Pot FreeRTOS task.
 * @return pdPASS on success, errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY on failure.
 */
BaseType_t task_pot_create(void)
{
    return xTaskCreate(prvPotTask,
                       "POT",
                       TASK_POT_STACK,
                       NULL,
                       TASK_POT_PRIO,
                       &xPotTaskHandle);
}
