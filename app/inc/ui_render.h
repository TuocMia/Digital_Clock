/**
 * @file    ui_render.h
 * @brief   ST7789 240x240 UI frame and partial 7-segment renderer.
 */

#ifndef UI_RENDER_H
#define UI_RENDER_H

#include "app_ipc.h"

/**
 * @brief  Draw static screen frame (title bar, colon, divider, underline).
 * @param  d Pointer to snapshot of shared application data.
 */
void ui_draw_frame(const AppData *d);

/**
 * @brief  Redraw only changed digits, date, and alarm status fields.
 * @param  d Pointer to snapshot of shared application data.
 */
void ui_render(const AppData *d);

#endif
