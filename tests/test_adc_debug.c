#include "stm32f10x.h"
#include "bsp.h"
#include "hal_adc.h"
#include "st7789.h"

/* Minimal 5x7 font for digits and status text */
typedef struct { char ch; uint8_t col[5]; } Glyph;
static const Glyph s_font[] = {
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00}},
    {':', {0x00, 0x36, 0x36, 0x00, 0x00}},
    {'0', {0x3E, 0x51, 0x49, 0x45, 0x3E}},
    {'1', {0x00, 0x42, 0x7F, 0x40, 0x00}},
    {'2', {0x42, 0x61, 0x51, 0x49, 0x46}},
    {'3', {0x21, 0x41, 0x45, 0x4B, 0x31}},
    {'4', {0x18, 0x14, 0x12, 0x7F, 0x10}},
    {'5', {0x27, 0x45, 0x45, 0x45, 0x39}},
    {'6', {0x3C, 0x4A, 0x49, 0x49, 0x30}},
    {'7', {0x01, 0x71, 0x09, 0x05, 0x03}},
    {'8', {0x36, 0x49, 0x49, 0x49, 0x36}},
    {'9', {0x06, 0x49, 0x49, 0x29, 0x1E}},
    {'A', {0x7E, 0x11, 0x11, 0x11, 0x7E}},
    {'C', {0x3E, 0x41, 0x41, 0x41, 0x22}},
    {'D', {0x7F, 0x41, 0x41, 0x22, 0x1C}},
    {'E', {0x7F, 0x49, 0x49, 0x49, 0x41}},
    {'I', {0x00, 0x41, 0x7F, 0x41, 0x00}},
    {'K', {0x7F, 0x08, 0x14, 0x22, 0x41}},
    {'M', {0x7F, 0x02, 0x0C, 0x02, 0x7F}},
    {'N', {0x7F, 0x04, 0x08, 0x10, 0x7F}},
    {'O', {0x3E, 0x41, 0x41, 0x41, 0x3E}},
    {'R', {0x7F, 0x09, 0x19, 0x29, 0x46}},
    {'T', {0x01, 0x01, 0x7F, 0x01, 0x01}},
    {'U', {0x3F, 0x40, 0x40, 0x40, 0x3F}}
};

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint32_t n = (uint32_t)w * (uint32_t)h;
    LCD_SetWindows(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));
    while (n--) {
        Lcd_WriteData_16Bit(color);
    }
}

static void draw_text(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg, uint8_t sc)
{
    while (*s) {
        const uint8_t *col = s_font[0].col;
        uint8_t i, cx, cy;
        for (i = 0U; i < (uint8_t)(sizeof(s_font) / sizeof(s_font[0])); i++) {
            if (s_font[i].ch == *s) { col = s_font[i].col; break; }
        }
        for (cx = 0U; cx < 5U; cx++) {
            for (cy = 0U; cy < 7U; cy++) {
                fill_rect((uint16_t)(x + cx * sc), (uint16_t)(y + cy * sc), sc, sc,
                          (col[cx] & (1U << cy)) ? fg : bg);
            }
        }
        fill_rect((uint16_t)(x + 5U * sc), y, sc, (uint16_t)(7U * sc), bg);
        x = (uint16_t)(x + 6U * sc);
        s++;
    }
}

static void delay_loop(volatile uint32_t c)
{
    while (c--) { __NOP(); }
}

int main(void)
{
    hal_adc_status_t init_st;
    hal_adc_status_t read_st;
    uint16_t val = 0U;
    char buf[5] = "0000";

    bsp_init();
    LCD_Init();
    LCD_Clear(BLACK);

    init_st = hal_adc_init();
    draw_text(20U, 30U, "ADC PA3 TEST", CYAN, BLACK, 2U);
    draw_text(20U, 70U, (init_st == HAL_ADC_OK) ? "INIT: OK" : "INIT: ERR",
              (init_st == HAL_ADC_OK) ? GREEN : RED, BLACK, 2U);

    for (;;) {
        read_st = hal_adc_read(&val);
        if (read_st == HAL_ADC_OK) {
            buf[0] = (char)('0' + (val / 1000U) % 10U);
            buf[1] = (char)('0' + (val / 100U) % 10U);
            buf[2] = (char)('0' + (val / 10U) % 10U);
            buf[3] = (char)('0' + (val % 10U));
            draw_text(20U, 110U, "READ: OK ", GREEN, BLACK, 2U);
            draw_text(36U, 155U, buf, YELLOW, BLACK, 5U);
        } else {
            draw_text(20U, 110U, "READ: ERR", RED, BLACK, 2U);
        }
        delay_loop(600000U);
    }
}
