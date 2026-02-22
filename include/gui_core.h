#ifndef GUI_CORE_H
#define GUI_CORE_H

#include "gui_types.h"

void gui_init(int w, int h);
void gui_shutdown(void);
void gui_set_focus(Widget *w);
void gui_main_loop(void);

#endif /* GUI_CORE_H */