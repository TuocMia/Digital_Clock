#ifndef UI_RENDER_H
#define UI_RENDER_H

#include "app_ipc.h"

/* Chi task_display goi. */
void ui_draw_frame(const AppData *d);   /* ve khung khi doi trang thai */
void ui_render(const AppData *d);       /* ve phan thay doi (chu so 7 doan, chu, gach chan) */

#endif
