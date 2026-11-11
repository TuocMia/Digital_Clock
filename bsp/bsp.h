#ifndef BSP_H
#define BSP_H

#include <stdint.h>

void    bsp_init(void);                 /* bat clock cac cong, cau hinh che do chan theo bsp_pins.h */
void    bsp_buzzer_set(uint8_t on);     /* 1 = keu */
void    bsp_led_set(uint8_t on);        /* 1 = sang */
uint8_t bsp_button_pressed(void);       /* 1 = dang nhan */

#endif
