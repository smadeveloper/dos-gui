#ifndef GUI_TOOLTIP_H
#define GUI_TOOLTIP_H

#include "gui_types.h"

#define TOOLTIP_DELAY 40
#define TOOLTIP_MAX_LEN 256

typedef struct {
    char text[TOOLTIP_MAX_LEN];
    int visible;
    int timer;
    int tip_x, tip_y;
    int tip_w, tip_h;
    Widget *current_widget;
} TooltipState;

extern TooltipState gui_tooltip;

void tooltip_set(Widget *w, const char *text);
const char *tooltip_get(Widget *w);
void tooltip_update(int mx, int my);
void tooltip_draw(BITMAP *target);
void tooltip_reset(void);

#endif /* GUI_TOOLTIP_H */
