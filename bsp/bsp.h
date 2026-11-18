/**
 * @file    bsp.h
 * @brief   Board Support Package interface for Blue Pill peripherals.
 */

#ifndef BSP_H
#define BSP_H

#include <stdint.h>

/**
 * @brief  Initialize clocks and GPIO pins defined in bsp_pins.h.
 */
void bsp_init(void);

/**
 * @brief  Control the active buzzer.
 * @param  on Non-zero to turn buzzer on, 0 to turn off.
 */
void bsp_buzzer_set(uint8_t on);

/**
 * @brief  Control the onboard Blue Pill LED (PC13, active-low).
 * @param  on Non-zero to turn LED on, 0 to turn off.
 */
void bsp_led_set(uint8_t on);

/**
 * @brief  Read the debounced-raw state of the user push button.
 * @return 1 if button is currently pressed, 0 otherwise.
 */
uint8_t bsp_button_pressed(void);

#endif
