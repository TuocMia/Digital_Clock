#include "bsp.h"
#include "bsp_pins.h"
#include "hal_gpio.h"

/**
 ******************************************************************************
 * @brief  Khoi tao toan bo phan cung (GPIO) cho project Digital Clock
 *
 * @note   Cau hinh cac chan theo bang bsp_pins.h:
 *
 *   Chan          | Port  | Pin | Che do        | Chuc nang
 *   --------------|-------|-----|---------------|---------------------------
 *   DS1302 CE     | GPIOA |  4  | OUT_PP        | Chip Enable DS1302
 *   DS1302 CLK    | GPIOA |  5  | OUT_PP        | Clock DS1302
 *   DS1302 IO     | GPIOA |  7  | OUT_PP        | Data DS1302 (doi IN/OUT)
 *   Nut bam       | GPIOA |  0  | IN_PU         | Nhan = 0 (noi GND) - PA0
 *   Bien tro      | GPIOA |  3  | AIN           | ADC1 kenh 3 - PA3
 *   Buzzer        | GPIOA |  8  | OUT_PP        | Muc cao = keu
 *   LCD LED (BL)  | GPIOB |  9  | OUT_PP        | Backlight ST7789
 *   LCD RS (DC)   | GPIOB | 10  | OUT_PP        | Data/Command ST7789
 *   LCD RST       | GPIOB | 12  | OUT_PP        | Reset LCD
 *   LED BluePill  | GPIOC | 13  | OUT_PP        | Muc thap = sang
 *
 *   (SPI2 SCK=PB13, MOSI=PB15 duoc cau hinh trong hal_spi.c / SPI2_Init)
 *
 ******************************************************************************
 */
void bsp_init(void)
{
    /* ---- DS1302: 3 chan dieu khien (bit-bang) ---- */
    hal_gpio_init(DS_PORT, DS_CE_PIN,  HAL_GPIO_OUT_PP);   /* PA4 */
    hal_gpio_init(DS_PORT, DS_CLK_PIN, HAL_GPIO_OUT_PP);   /* PA5 */
    hal_gpio_init(DS_PORT, DS_IO_PIN,  HAL_GPIO_OUT_PP);   /* PA7 */

    /* ---- Nut bam: input pull-up, nhan = LOW ---- */
    hal_gpio_init(BTN_PORT, BTN_PIN,   HAL_GPIO_IN_PU);    /* PA0 */

    /* ---- Bien tro: analog input cho ADC ---- */
    hal_gpio_init(POT_PORT, POT_PIN,   HAL_GPIO_AIN);      /* PA3 */

    /* ---- Buzzer: output push-pull, HIGH = keu ---- */
    hal_gpio_init(BUZ_PORT, BUZ_PIN,   HAL_GPIO_OUT_PP);   /* PA8 */
    hal_gpio_clear(BUZ_PORT, BUZ_PIN);                      /* Tat buzzer ban dau */

    /* ---- LCD control pins (Module ST7789 7-pin: CS da noi GND, khong dung CS) ---- */
    hal_gpio_init(LCD_RS_PORT,  LCD_RS_PIN,  HAL_GPIO_OUT_PP);  /* PB10 */
    hal_gpio_init(LCD_RST_PORT, LCD_RST_PIN, HAL_GPIO_OUT_PP);  /* PB12 */

    /* ---- LED tren BluePill: muc thap = sang ---- */
    hal_gpio_init(LED_PORT, LED_PIN, HAL_GPIO_OUT_PP);      /* PC13 */
    hal_gpio_set(LED_PORT, LED_PIN);                         /* LED tat ban dau (HIGH = tat) */
}

/**
 * @brief  Dieu khien buzzer
 * @param  on: 1 = keu (set HIGH), 0 = tat (set LOW)
 *
 * @note   Buzzer chu dong: cap muc cao => keu, muc thap => im
 *         Su dung BSRR/BRR de thao tac atomic, an toan trong FreeRTOS
 */
void bsp_buzzer_set(uint8_t on)
{
    if (on)
        hal_gpio_set(BUZ_PORT, BUZ_PIN);    /* PA8 = HIGH => buzzer keu */
    else
        hal_gpio_clear(BUZ_PORT, BUZ_PIN);  /* PA8 = LOW  => buzzer tat */
}

/**
 * @brief  Dieu khien LED tren board BluePill
 * @param  on: 1 = sang (set LOW), 0 = tat (set HIGH)
 *
 * @note   LED tren BluePill noi ACTIVE LOW:
 *         PC13 = LOW  => LED sang
 *         PC13 = HIGH => LED tat
 */
void bsp_led_set(uint8_t on)
{
    if (on)
        hal_gpio_clear(LED_PORT, LED_PIN);  /* PC13 = LOW  => LED sang */
    else
        hal_gpio_set(LED_PORT, LED_PIN);    /* PC13 = HIGH => LED tat  */
}

/**
 * @brief  Doc trang thai nut bam
 * @retval 1 neu dang nhan, 0 neu tha
 *
 * @note   Nut bam noi GND voi pull-up noi:
 *         - Tha: IDR = 1 (keo len boi pull-up)
 *         - Nhan: IDR = 0 (noi truc tiep GND)
 *         => Dao logic: return !read
 */
uint8_t bsp_button_pressed(void)
{
    return !hal_gpio_read(BTN_PORT, BTN_PIN);  /* Nhan = LOW => !0 = 1 */
}
