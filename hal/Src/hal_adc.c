#include "hal_adc.h"
#include "hal_gpio.h"
#include "stm32f10x_conf.h"

#define HAL_ADC_POLL_TIMEOUT 100000UL

/* Wait for ADC reset calibration or calibration to finish. */
static uint8_t wait_for_calibration(FunctionalState reset)
{
    uint32_t timeout = HAL_ADC_POLL_TIMEOUT;

    if (reset == ENABLE) {
        while (ADC_GetResetCalibrationStatus(ADC1) == SET && timeout > 0U) {
            timeout--;
        }
    } else {
        while (ADC_GetCalibrationStatus(ADC1) == SET && timeout > 0U) {
            timeout--;
        }
    }

    if (reset == ENABLE) {
        return (ADC_GetResetCalibrationStatus(ADC1) == RESET) ? 1U : 0U;
    }
    return (ADC_GetCalibrationStatus(ADC1) == RESET) ? 1U : 0U;
}

/*****************************************************************************
 * @name       :hal_adc_status_t hal_adc_init(void)
 * @function   :Initialize ADC1 to read the potentiometer on PA3
 * @parameters :None
 * @retvalue   :HAL_ADC_OK or HAL_ADC_TIMEOUT
 ******************************************************************************/
hal_adc_status_t hal_adc_init(void)
{
    ADC_InitTypeDef adc;

    /* PA3 is ADC1 channel 3; configure it through the shared GPIO HAL. */
    hal_gpio_init(GPIOA, GPIO_Pin_3, HAL_GPIO_AIN);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    ADC_DeInit(ADC1);
    adc.ADC_Mode = ADC_Mode_Independent;
    adc.ADC_ScanConvMode = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &adc);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    if (wait_for_calibration(ENABLE) == 0U) {
        return HAL_ADC_TIMEOUT;
    }

    ADC_StartCalibration(ADC1);
    if (wait_for_calibration(DISABLE) == 0U) {
        return HAL_ADC_TIMEOUT;
    }

    return HAL_ADC_OK;
}

/*****************************************************************************
 * @name       :hal_adc_status_t hal_adc_read(uint16_t *value)
 * @function   :Perform one ADC conversion and return the 12-bit result
 * @parameters :value: Pointer to the result variable
 * @retvalue   :HAL_ADC_OK, HAL_ADC_INVALID_ARGUMENT, or HAL_ADC_TIMEOUT
 ******************************************************************************/
hal_adc_status_t hal_adc_read(uint16_t *value)
{
    uint32_t timeout = HAL_ADC_POLL_TIMEOUT;

    if (value == 0) {
        return HAL_ADC_INVALID_ARGUMENT;
    }

    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET && timeout > 0U) {
        timeout--;
    }

    if (timeout == 0U) {
        return HAL_ADC_TIMEOUT;
    }

    *value = ADC_GetConversionValue(ADC1);
    return HAL_ADC_OK;
}
