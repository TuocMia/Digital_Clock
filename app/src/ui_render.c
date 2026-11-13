#include <string.h>
#include "ui_render.h"
#include "st7789.h"

/* Bang mau RGB565 */
#define UI_BG           BLACK
#define UI_BAR_NORMAL   0x0210U     /* xanh dam */
#define UI_BAR_ALARM    RED
#define UI_FG_TIME      CYAN
#define UI_FG_EDIT      YELLOW
#define UI_FG_DATE      WHITE
#define UI_FG_ALM_ON    GREEN
#define UI_FG_ALM_OFF   GRAY1
#define UI_DIVIDER      GRAY2

/* Bo cuc man hinh ST7789 240x240 */
#define BAR_H           36U
#define BIG_Y           58U
#define BIG_W           36U
#define BIG_H           64U
#define BIG_T           7U
#define SEC_Y           90U
#define SEC_W           18U
#define SEC_H           32U
#define SEC_T           4U
#define UNDER_Y         128U
#define UNDER_H         3U
#define DATE_Y          150U
#define DIV_Y           180U
#define ALM_Y           196U
#define ALM_UNDER_Y     214U

/* Vi tri X cac chu so */
#define X_H1            14U
#define X_H2            56U
#define X_COLON         98U
#define X_M1            112U
#define X_M2            154U
#define X_S1            194U
#define X_S2            216U

/* Cache trang thai ve lan truoc de chi ve lai phan thay doi */
typedef struct {
    uint8_t valid;
    uint8_t h, m, s;
    uint8_t date, month, year;
    uint8_t ah, am, aen;
    uint8_t state;
    uint8_t ring;
} UiCache;

static UiCache s_cache;

/* Bitmask 7 doan: bit0=a(tren), 1=b(phai tren), 2=c(phai duoi),
 * 3=d(duoi), 4=e(trai duoi), 5=f(trai tren), 6=g(giua) */
static const uint8_t s_seg_lut[10] = {
    0x3FU, /* 0 */
    0x06U, /* 1 */
    0x5BU, /* 2 */
    0x4FU, /* 3 */
    0x66U, /* 4 */
    0x6DU, /* 5 */
    0x7DU, /* 6 */
    0x07U, /* 7 */
    0x7FU, /* 8 */
    0x6FU  /* 9 */
};

/* Font 5x7 toi gian cho cac ky tu can dung (moi cot 1 byte, 5 cot/ky tu) */
typedef struct {
    char ch;
    uint8_t col[5];
} Glyph5x7;

static const Glyph5x7 s_font[] = {
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00}},
    {'!', {0x00, 0x00, 0x5F, 0x00, 0x00}},
    {'/', {0x20, 0x10, 0x08, 0x04, 0x02}},
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
    {'E', {0x7F, 0x49, 0x49, 0x49, 0x41}},
    {'F', {0x7F, 0x09, 0x09, 0x09, 0x01}},
    {'H', {0x7F, 0x08, 0x08, 0x08, 0x7F}},
    {'I', {0x00, 0x41, 0x7F, 0x41, 0x00}},
    {'L', {0x7F, 0x40, 0x40, 0x40, 0x40}},
    {'M', {0x7F, 0x02, 0x0C, 0x02, 0x7F}},
    {'N', {0x7F, 0x04, 0x08, 0x10, 0x7F}},
    {'O', {0x3E, 0x41, 0x41, 0x41, 0x3E}},
    {'R', {0x7F, 0x09, 0x19, 0x29, 0x46}},
    {'S', {0x46, 0x49, 0x49, 0x49, 0x31}},
    {'T', {0x01, 0x01, 0x7F, 0x01, 0x01}},
    {'U', {0x3F, 0x40, 0x40, 0x40, 0x3F}},
    {'V', {0x1F, 0x20, 0x40, 0x20, 0x1F}},
    {'W', {0x3F, 0x40, 0x38, 0x40, 0x3F}}
};

static void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint32_t total;
    if (w == 0U || h == 0U || x >= LCD_W || y >= LCD_H) {
        return;
    }
    if (x + w > LCD_W) { w = LCD_W - x; }
    if (y + h > LCD_H) { h = LCD_H - y; }

    LCD_SetWindows(x, y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U));
    total = (uint32_t)w * (uint32_t)h;
    while (total--) {
        Lcd_WriteData_16Bit(color);
    }
}

static const uint8_t *find_glyph(char c)
{
    uint8_t i;
    for (i = 0U; i < (uint8_t)(sizeof(s_font) / sizeof(s_font[0])); i++) {
        if (s_font[i].ch == c) {
            return s_font[i].col;
        }
    }
    return s_font[0].col;
}

static void draw_char(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg, uint8_t scale)
{
    const uint8_t *col = find_glyph(c);
    uint8_t cx, cy;

    for (cx = 0U; cx < 5U; cx++) {
        uint8_t bits = col[cx];
        for (cy = 0U; cy < 7U; cy++) {
            uint16_t color = (bits & (1U << cy)) ? fg : bg;
            lcd_fill_rect((uint16_t)(x + cx * scale), (uint16_t)(y + cy * scale), scale, scale, color);
        }
    }
    /* Khoang cach 1 cot giua cac ky tu */
    lcd_fill_rect((uint16_t)(x + 5U * scale), y, scale, (uint16_t)(7U * scale), bg);
}

static void draw_text(uint16_t x, uint16_t y, const char *s, uint16_t fg, uint16_t bg, uint8_t scale)
{
    while (*s != '\0') {
        draw_char(x, y, *s, fg, bg, scale);
        x = (uint16_t)(x + 6U * scale);
        s++;
    }
}

static void draw_7seg(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t t,
                      uint8_t digit, uint16_t fg, uint16_t bg)
{
    uint8_t mask = (digit <= 9U) ? s_seg_lut[digit] : 0U;
    uint16_t half_h = (uint16_t)(h / 2U);
    uint16_t v_len  = (uint16_t)(half_h - t);
    uint16_t h_len  = (uint16_t)(w - 2U * t);

    lcd_fill_rect((uint16_t)(x + t), y, h_len, t, (mask & 0x01U) ? fg : bg);                                   /* a */
    lcd_fill_rect((uint16_t)(x + w - t), (uint16_t)(y + t), t, v_len, (mask & 0x02U) ? fg : bg);                /* b */
    lcd_fill_rect((uint16_t)(x + w - t), (uint16_t)(y + half_h + t / 2U), t, v_len, (mask & 0x04U) ? fg : bg);  /* c */
    lcd_fill_rect((uint16_t)(x + t), (uint16_t)(y + h - t), h_len, t, (mask & 0x08U) ? fg : bg);                /* d */
    lcd_fill_rect(x, (uint16_t)(y + half_h + t / 2U), t, v_len, (mask & 0x10U) ? fg : bg);                      /* e */
    lcd_fill_rect(x, (uint16_t)(y + t), t, v_len, (mask & 0x20U) ? fg : bg);                                    /* f */
    lcd_fill_rect((uint16_t)(x + t), (uint16_t)(y + half_h - t / 2U), h_len, t, (mask & 0x40U) ? fg : bg);      /* g */
}

static void put_2d(char *buf, uint8_t v)
{
    buf[0] = (char)('0' + (v / 10U) % 10U);
    buf[1] = (char)('0' + (v % 10U));
}

static const char *title_for_state(const AppData *d)
{
    if (d->ring) {
        return "ALARM!";
    }
    switch (d->state) {
        case ST_VIEW:       return "DIGITAL CLOCK";
        case ST_SET_HOUR:   return "SET HOUR";
        case ST_SET_MIN:    return "SET MIN";
        case ST_ALARM_HOUR: return "ALARM HOUR";
        case ST_ALARM_MIN:  return "ALARM MIN";
        case ST_ALARM_EN:   return "ALARM ON/OFF";
        default:            return "CLOCK";
    }
}

void ui_draw_frame(const AppData *d)
{
    const char *title = title_for_state(d);
    uint16_t bar_bg = d->ring ? UI_BAR_ALARM : UI_BAR_NORMAL;
    uint16_t len = (uint16_t)strlen(title);
    uint16_t tx = (len * 12U < LCD_W) ? (uint16_t)((LCD_W - len * 12U) / 2U) : 4U;

    memset(&s_cache, 0, sizeof(s_cache));

    LCD_Clear(UI_BG);
    lcd_fill_rect(0U, 0U, LCD_W, BAR_H, bar_bg);
    draw_text(tx, 11U, title, WHITE, bar_bg, 2U);

    /* Dau hai cham giua HH va MM */
    lcd_fill_rect(X_COLON, (uint16_t)(BIG_Y + 18U), 7U, 7U, UI_FG_TIME);
    lcd_fill_rect(X_COLON, (uint16_t)(BIG_Y + 40U), 7U, 7U, UI_FG_TIME);

    /* Duong ke ngan cach phan bao thuc */
    lcd_fill_rect(16U, DIV_Y, (uint16_t)(LCD_W - 32U), 1U, UI_DIVIDER);

    /* Gach chan muc dang chinh */
    if (d->state == ST_SET_HOUR || d->state == ST_ALARM_HOUR) {
        lcd_fill_rect(X_H1, UNDER_Y, (uint16_t)(X_H2 + BIG_W - X_H1), UNDER_H, UI_FG_EDIT);
    } else if (d->state == ST_SET_MIN || d->state == ST_ALARM_MIN) {
        lcd_fill_rect(X_M1, UNDER_Y, (uint16_t)(X_M2 + BIG_W - X_M1), UNDER_H, UI_FG_EDIT);
    } else if (d->state == ST_ALARM_EN) {
        lcd_fill_rect(162U, ALM_UNDER_Y, 34U, UNDER_H, UI_FG_EDIT);
    }
}

void ui_render(const AppData *d)
{
    uint8_t disp_h, disp_m, disp_s;
    uint8_t disp_ah, disp_am, disp_aen;
    uint16_t time_fg;
    uint8_t force = (s_cache.valid == 0U);

    /* Chon du lieu hien thi theo buoc */
    if (d->state == ST_SET_HOUR || d->state == ST_SET_MIN) {
        disp_h = d->edit_h;
        disp_m = d->edit_m;
        disp_s = 0U;
        time_fg = UI_FG_EDIT;
    } else if (d->state == ST_ALARM_HOUR || d->state == ST_ALARM_MIN) {
        disp_h = d->edit_h;
        disp_m = d->edit_m;
        disp_s = d->sec;
        time_fg = UI_FG_EDIT;
    } else {
        disp_h = d->hour;
        disp_m = d->min;
        disp_s = d->sec;
        time_fg = UI_FG_TIME;
    }

    if (d->state == ST_ALARM_EN) {
        disp_ah  = d->ah;
        disp_am  = d->am;
        disp_aen = d->edit_en;
    } else {
        disp_ah  = d->ah;
        disp_am  = d->am;
        disp_aen = d->aen;
    }

    /* Cap nhat chu so 7 doan neu thay doi */
    if (force || (disp_h / 10U) != (s_cache.h / 10U)) {
        draw_7seg(X_H1, BIG_Y, BIG_W, BIG_H, BIG_T, (uint8_t)(disp_h / 10U), time_fg, UI_BG);
    }
    if (force || (disp_h % 10U) != (s_cache.h % 10U)) {
        draw_7seg(X_H2, BIG_Y, BIG_W, BIG_H, BIG_T, (uint8_t)(disp_h % 10U), time_fg, UI_BG);
    }
    if (force || (disp_m / 10U) != (s_cache.m / 10U)) {
        draw_7seg(X_M1, BIG_Y, BIG_W, BIG_H, BIG_T, (uint8_t)(disp_m / 10U), time_fg, UI_BG);
    }
    if (force || (disp_m % 10U) != (s_cache.m % 10U)) {
        draw_7seg(X_M2, BIG_Y, BIG_W, BIG_H, BIG_T, (uint8_t)(disp_m % 10U), time_fg, UI_BG);
    }
    if (force || (disp_s / 10U) != (s_cache.s / 10U)) {
        draw_7seg(X_S1, SEC_Y, SEC_W, SEC_H, SEC_T, (uint8_t)(disp_s / 10U), UI_FG_TIME, UI_BG);
    }
    if (force || (disp_s % 10U) != (s_cache.s % 10U)) {
        draw_7seg(X_S2, SEC_Y, SEC_W, SEC_H, SEC_T, (uint8_t)(disp_s % 10U), UI_FG_TIME, UI_BG);
    }

    /* Ngay DD/MM/20YY */
    if (force || d->date != s_cache.date || d->month != s_cache.month || d->year != s_cache.year) {
        char dbuf[11] = "00/00/2000";
        put_2d(&dbuf[0], d->date);
        put_2d(&dbuf[3], d->month);
        put_2d(&dbuf[8], d->year);
        draw_text(60U, DATE_Y, dbuf, UI_FG_DATE, UI_BG, 2U);
    }

    /* Trang thai bao thuc: ALM HH:MM ON / OFF */
    if (force || disp_ah != s_cache.ah || disp_am != s_cache.am || disp_aen != s_cache.aen) {
        char abuf[14] = "ALM 00:00 OFF";
        uint16_t afg = disp_aen ? UI_FG_ALM_ON : UI_FG_ALM_OFF;
        put_2d(&abuf[4], disp_ah);
        put_2d(&abuf[7], disp_am);
        if (disp_aen) {
            abuf[10] = 'O';
            abuf[11] = 'N';
            abuf[12] = ' ';
        }
        draw_text(42U, ALM_Y, abuf, afg, UI_BG, 2U);
    }

    s_cache.valid = 1U;
    s_cache.h     = disp_h;
    s_cache.m     = disp_m;
    s_cache.s     = disp_s;
    s_cache.date  = d->date;
    s_cache.month = d->month;
    s_cache.year  = d->year;
    s_cache.ah    = disp_ah;
    s_cache.am    = disp_am;
    s_cache.aen   = disp_aen;
    s_cache.state = d->state;
    s_cache.ring  = d->ring;
}
