#include "hal_ds1302.h"
#include "hal_gpio.h"
#include "stm32f10x_conf.h"

#define DS1302_CMD_BURST_READ   0xBFU
#define DS1302_CMD_BURST_WRITE  0xBEU
#define DS1302_CMD_RAM_WRITE(a) (0xC0U | (uint8_t)(((a) & 0x1FU) << 1U))
#define DS1302_CMD_RAM_READ(a)  (0xC1U | (uint8_t)(((a) & 0x1FU) << 1U))
#define DS1302_RAM_SIZE         31U
#define DS1302_CMD_WRITE_PROT   0x8EU
#define DS1302_WRITE_PROT_ON    0x80U
#define DS1302_SECONDS_CH       0x80U
#define DS1302_HOUR_12_FORMAT   0x80U
#define DS1302_HOUR_PM          0x20U
#define DS1302_DELAY_LOOPS      18U

/*****************************************************************************
 * @name       :static void ds1302_delay(void)
 * @function   :Provide a short setup/hold delay for bit-banged communication
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void ds1302_delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < DS1302_DELAY_LOOPS; i++) {
        /* Keep a short setup/hold time between GPIO edges. */
    }
}

/*****************************************************************************
 * @name       :static void ds1302_write_mode(void)
 * @function   :Configure the shared DS1302 IO pin as output
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void ds1302_write_mode(void)
{
    hal_gpio_init(HAL_DS1302_IO_PORT, HAL_DS1302_IO_PIN, HAL_GPIO_OUT_PP);
}

/*****************************************************************************
 * @name       :static void ds1302_read_mode(void)
 * @function   :Configure the shared DS1302 IO pin as pulled-up input
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void ds1302_read_mode(void)
{
    hal_gpio_init(HAL_DS1302_IO_PORT, HAL_DS1302_IO_PIN, HAL_GPIO_IN_PU);
}

/*****************************************************************************
 * @name       :static void ds1302_write_byte(uint8_t value)
 * @function   :Send one byte, least-significant bit first
 * @parameters :value: Byte to send
 * @retvalue   :None
 ******************************************************************************/
static void ds1302_write_byte(uint8_t value)
{
    uint8_t bit;

    for (bit = 0; bit < 8U; bit++) {
        hal_gpio_clear(HAL_DS1302_SCLK_PORT, HAL_DS1302_SCLK_PIN);
        if ((value & 0x01U) != 0U) {
            hal_gpio_set(HAL_DS1302_IO_PORT, HAL_DS1302_IO_PIN);
        } else {
            hal_gpio_clear(HAL_DS1302_IO_PORT, HAL_DS1302_IO_PIN);
        }
        ds1302_delay();
        hal_gpio_set(HAL_DS1302_SCLK_PORT, HAL_DS1302_SCLK_PIN);
        ds1302_delay();
        value >>= 1;
    }
}

/*****************************************************************************
 * @name       :static void ds1302_begin(void)
 * @function   :Start a DS1302 transaction by asserting CE
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void ds1302_begin(void)
{
    hal_gpio_clear(HAL_DS1302_SCLK_PORT, HAL_DS1302_SCLK_PIN);
    hal_gpio_set(HAL_DS1302_CE_PORT, HAL_DS1302_CE_PIN);
    ds1302_delay();
}

/*****************************************************************************
 * @name       :static void ds1302_end(void)
 * @function   :End a DS1302 transaction by deasserting CE
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
static void ds1302_end(void)
{
    hal_gpio_clear(HAL_DS1302_SCLK_PORT, HAL_DS1302_SCLK_PIN);
    ds1302_delay();
    hal_gpio_clear(HAL_DS1302_CE_PORT, HAL_DS1302_CE_PIN);
}

/*****************************************************************************
 * @name       :static uint8_t ds1302_read_byte(void)
 * @function   :Read one byte, least-significant bit first
 * @parameters :None
 * @retvalue   :Byte received from DS1302
 ******************************************************************************/
static uint8_t ds1302_read_byte(void)
{
    uint8_t bit;
    uint8_t value = 0U;

    for (bit = 0; bit < 8U; bit++) {
        hal_gpio_clear(HAL_DS1302_SCLK_PORT, HAL_DS1302_SCLK_PIN);
        ds1302_delay();
        if (hal_gpio_read(HAL_DS1302_IO_PORT, HAL_DS1302_IO_PIN) != 0U) {
            value |= (uint8_t)(1U << bit);
        }
        hal_gpio_set(HAL_DS1302_SCLK_PORT, HAL_DS1302_SCLK_PIN);
        ds1302_delay();
    }

    return value;
}

/*****************************************************************************
 * @name       :static uint8_t bcd_to_binary(uint8_t value)
 * @function   :Convert a packed BCD value to a binary integer
 * @parameters :value: Packed BCD value
 * @retvalue   :Converted integer
 ******************************************************************************/
static uint8_t bcd_to_binary(uint8_t value)
{
    return (uint8_t)(((value >> 4) * 10U) + (value & 0x0FU));
}

/*****************************************************************************
 * @name       :static uint8_t binary_to_bcd(uint8_t value)
 * @function   :Convert a binary integer to packed BCD
 * @parameters :value: Binary integer
 * @retvalue   :Packed BCD value
 ******************************************************************************/
static uint8_t binary_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10U) << 4) | (value % 10U));
}

/*****************************************************************************
 * @name       :static uint8_t valid_bcd(uint8_t value, uint8_t max_value)
 * @function   :Check BCD digits and the permitted maximum value
 * @parameters :value: Packed BCD value; max_value: Maximum decoded value
 * @retvalue   :1 when valid, otherwise 0
 ******************************************************************************/
static uint8_t valid_bcd(uint8_t value, uint8_t max_value)
{
    uint8_t low = value & 0x0FU;
    uint8_t high = (value >> 4) & 0x0FU;
    return (low <= 9U && high <= 9U && bcd_to_binary(value) <= max_value) ? 1U : 0U;
}

/*****************************************************************************
 * @name       :static uint8_t valid_datetime(...)
 * @function   :Check that each date/time field is within its basic range
 * @parameters :datetime: Date/time structure to check
 * @retvalue   :1 when valid, otherwise 0
 ******************************************************************************/
static uint8_t valid_datetime(const hal_ds1302_datetime_t *datetime)
{
    return (datetime != 0 && datetime->seconds < 60U && datetime->minutes < 60U &&
            datetime->hours < 24U && datetime->date >= 1U && datetime->date <= 31U &&
            datetime->month >= 1U && datetime->month <= 12U &&
            datetime->weekday >= 1U && datetime->weekday <= 7U && datetime->year <= 99U);
}

/*****************************************************************************
 * @name       :void hal_ds1302_init(void)
 * @function   :Initialize DS1302 control and bidirectional data pins
 * @parameters :None
 * @retvalue   :None
 ******************************************************************************/
void hal_ds1302_init(void)
{
    hal_gpio_init(HAL_DS1302_CE_PORT, HAL_DS1302_CE_PIN, HAL_GPIO_OUT_PP);
    hal_gpio_init(HAL_DS1302_SCLK_PORT, HAL_DS1302_SCLK_PIN, HAL_GPIO_OUT_PP);

    ds1302_read_mode();
    hal_gpio_clear(HAL_DS1302_CE_PORT, HAL_DS1302_CE_PIN);
    hal_gpio_clear(HAL_DS1302_SCLK_PORT, HAL_DS1302_SCLK_PIN);
}

/*****************************************************************************
 * @name       :hal_ds1302_status_t hal_ds1302_read_time(...)
 * @function   :Read, validate, and convert DS1302 date/time registers
 * @parameters :datetime: Pointer to the output date/time structure
 * @retvalue   :RTC status code
 ******************************************************************************/
hal_ds1302_status_t hal_ds1302_read_time(hal_ds1302_datetime_t *datetime)
{
    uint8_t raw[8];
    uint8_t i;
    uint8_t stopped;
    uint8_t hour_reg;
    uint8_t hour_value;

    if (datetime == 0) {
        return HAL_DS1302_INVALID_ARGUMENT;
    }

    ds1302_read_mode();
    ds1302_begin();
    ds1302_write_mode();
    ds1302_write_byte(DS1302_CMD_BURST_READ);
    ds1302_read_mode();

    for (i = 0; i < 8U; i++) {
        raw[i] = ds1302_read_byte();
    }

    ds1302_end();

    stopped = (raw[0] & DS1302_SECONDS_CH) != 0U;
    raw[0] &= (uint8_t)~DS1302_SECONDS_CH;

    if (!valid_bcd(raw[0], 59U) || !valid_bcd(raw[1], 59U) ||
        !valid_bcd(raw[3], 31U) || !valid_bcd(raw[4], 12U) ||
        !valid_bcd(raw[5], 7U) || !valid_bcd(raw[6], 99U)) {
        return HAL_DS1302_INVALID_DATA;
    }
    if (bcd_to_binary(raw[3]) == 0U || bcd_to_binary(raw[4]) == 0U ||
        bcd_to_binary(raw[5]) == 0U) {
        return HAL_DS1302_INVALID_DATA;
    }

    hour_reg = raw[2];
    if ((hour_reg & DS1302_HOUR_12_FORMAT) != 0U) {
        hour_value = hour_reg & 0x1FU;
        if (!valid_bcd(hour_value, 12U) || bcd_to_binary(hour_value) == 0U) {
            return HAL_DS1302_INVALID_DATA;
        }
        datetime->hours = (uint8_t)(bcd_to_binary(hour_value) % 12U);
        if ((hour_reg & DS1302_HOUR_PM) != 0U) {
            datetime->hours += 12U;
        }
    } else {
        hour_value = hour_reg & 0x3FU;
        if (!valid_bcd(hour_value, 23U)) {
            return HAL_DS1302_INVALID_DATA;
        }
        datetime->hours = bcd_to_binary(hour_value);
    }

    datetime->seconds = bcd_to_binary(raw[0]);
    datetime->minutes = bcd_to_binary(raw[1]);
    datetime->date = bcd_to_binary(raw[3]);
    datetime->month = bcd_to_binary(raw[4]);
    datetime->weekday = bcd_to_binary(raw[5]);
    datetime->year = bcd_to_binary(raw[6]);

    return stopped ? HAL_DS1302_CLOCK_STOPPED : HAL_DS1302_OK;
}

/*****************************************************************************
 * @name       :hal_ds1302_status_t hal_ds1302_write_time(...)
 * @function   :Validate and write date/time registers in 24-hour format
 * @parameters :datetime: Pointer to the input date/time structure
 * @retvalue   :RTC status code
 ******************************************************************************/
hal_ds1302_status_t hal_ds1302_write_time(const hal_ds1302_datetime_t *datetime)
{
    uint8_t values[8];
    uint8_t i;

    if (!valid_datetime(datetime)) {
        return HAL_DS1302_INVALID_ARGUMENT;
    }

    values[0] = binary_to_bcd(datetime->seconds) & (uint8_t)~DS1302_SECONDS_CH;
    values[1] = binary_to_bcd(datetime->minutes);
    values[2] = binary_to_bcd(datetime->hours); /* 24-hour format */
    values[3] = binary_to_bcd(datetime->date);
    values[4] = binary_to_bcd(datetime->month);
    values[5] = binary_to_bcd(datetime->weekday);
    values[6] = binary_to_bcd(datetime->year);
    values[7] = 0x00U; /* Write protection disabled in control register */

    ds1302_write_mode();
    ds1302_begin();
    ds1302_write_byte(DS1302_CMD_WRITE_PROT);
    ds1302_write_byte(0x00U);
    ds1302_end();

    ds1302_begin();
    ds1302_write_byte(DS1302_CMD_BURST_WRITE);
    for (i = 0; i < 8U; i++) {
        ds1302_write_byte(values[i]);
    }
    ds1302_end();

    ds1302_begin();
    ds1302_write_byte(DS1302_CMD_WRITE_PROT);
    ds1302_write_byte(DS1302_WRITE_PROT_ON);
    ds1302_end();

    return HAL_DS1302_OK;
}

/*****************************************************************************
 * @name       :hal_ds1302_status_t hal_ds1302_read_ram(...)
 * @function   :Read one byte from DS1302 31-byte battery-backed RAM
 * @parameters :addr: RAM byte index (0..30); value: Output byte pointer
 * @retvalue   :RTC status code
 ******************************************************************************/
hal_ds1302_status_t hal_ds1302_read_ram(uint8_t addr, uint8_t *value)
{
    if (value == 0 || addr >= DS1302_RAM_SIZE) {
        return HAL_DS1302_INVALID_ARGUMENT;
    }

    ds1302_write_mode();
    ds1302_begin();
    ds1302_write_byte(DS1302_CMD_RAM_READ(addr));
    ds1302_read_mode();
    *value = ds1302_read_byte();
    ds1302_end();

    return HAL_DS1302_OK;
}

/*****************************************************************************
 * @name       :hal_ds1302_status_t hal_ds1302_write_ram(...)
 * @function   :Write one byte to DS1302 31-byte battery-backed RAM
 * @parameters :addr: RAM byte index (0..30); value: Byte to write
 * @retvalue   :RTC status code
 ******************************************************************************/
hal_ds1302_status_t hal_ds1302_write_ram(uint8_t addr, uint8_t value)
{
    if (addr >= DS1302_RAM_SIZE) {
        return HAL_DS1302_INVALID_ARGUMENT;
    }

    ds1302_write_mode();
    ds1302_begin();
    ds1302_write_byte(DS1302_CMD_WRITE_PROT);
    ds1302_write_byte(0x00U);
    ds1302_end();

    ds1302_begin();
    ds1302_write_byte(DS1302_CMD_RAM_WRITE(addr));
    ds1302_write_byte(value);
    ds1302_end();

    ds1302_begin();
    ds1302_write_byte(DS1302_CMD_WRITE_PROT);
    ds1302_write_byte(DS1302_WRITE_PROT_ON);
    ds1302_end();

    return HAL_DS1302_OK;
}
