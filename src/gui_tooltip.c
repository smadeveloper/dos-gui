#include "gui_tooltip.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <string.h>

TooltipState gui_tooltip;

/* Store tooltip text in widget user_data as simple string */
void tooltip_set(Widget *w, const char *text) {
    if (!w) return;
    if (w->user_data) free(w->user_data);
    if (text && text[0]) {
        w->user_data = malloc(strlen(text) + 1);
        if (w->user_data) strcpy((char *)w->user_data, text);
    } else {
        w->user_data = NULL;
    }
}

const char *tooltip_get(Widget *w) {
    if (!w || !w->user_data) return NULL;
    return (const char *)w->user_data;
}

void tooltip_reset(void) {
    gui_tooltip.visible = 0;
    gui_tooltip.timer = 0;
    gui_tooltip.current_widget = NULL;
}

void tooltip_update(int mx, int my) {
    Widget *hovered = gui_state.hovered_widget;

    if (hovered != gui_tooltip.current_widget) {
        gui_tooltip.current_widget = hovered;
        gui_tooltip.visible = 0;
        gui_tooltip.timer = 0;
    }

    if (hovered && hovered->user_data) {
        gui_tooltip.timer++;
        if (gui_tooltip.timer >= TOOLTIP_DELAY && !gui_tooltip.visible) {
            const char *tip_text = (const char *)hovered->user_data;
            strncpy(gui_tooltip.text, tip_text, TOOLTIP_MAX_LEN - 1);
            gui_tooltip.text[TOOLTIP_MAX_LEN - 1] = 0;

            gui_tooltip.tip_w = text_length(font, gui_tooltip.text) + 10;
            gui_tooltip.tip_h = text_height(font) + 8;
            gui_tooltip.tip_x = mx + 12;
            gui_tooltip.tip_y = my + 18;

            /* Clamp to screen */
            if (gui_tooltip.tip_x + gui_tooltip.tip_w > gui_state.screen_w) {
                gui_tooltip.tip_x = gui_state.screen_w - gui_tooltip.tip_w;
            }
            if (gui_tooltip.tip_y + gui_tooltip.tip_h > gui_state.screen_h) {
                gui_tooltip.tip_y = my - gui_tooltip.tip_h - 4;
            }

            gui_tooltip.visible = 1;
        }
    } else {
        gui_tooltip.visible = 0;
        gui_tooltip.timer = 0;
    }
}

void tooltip_draw(BITMAP *target) {
    if (!gui_tooltip.visible) return;

    rectfill(target, gui_tooltip.tip_x, gui_tooltip.tip_y,
             gui_tooltip.tip_x + gui_tooltip.tip_w - 1,
             gui_tooltip.tip_y + gui_tooltip.tip_h - 1,
             theme.tooltip_bg);
    rect(target, gui_tooltip.tip_x, gui_tooltip.tip_y,
         gui_tooltip.tip_x + gui_tooltip.tip_w - 1,
         gui_tooltip.tip_y + gui_tooltip.tip_h - 1,
         theme.tooltip_border);
    textout_ex(target, font, gui_tooltip.text,
               gui_tooltip.tip_x + 5,
               gui_tooltip.tip_y + 4,
               theme.tooltip_fg, -1);
}
