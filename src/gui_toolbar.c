#include "gui_toolbar.h"
#include "gui_widget.h"
#include "gui_button.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

#define TOOLBAR_HEIGHT   28
#define TOOLBAR_BTN_SIZE 24
#define TOOLBAR_SEP_W    8

static void toolbar_draw(Widget *self, BITMAP *target) {
    int x = self->abs_x, y = self->abs_y, w = self->w, h = self->h;

    /* Background gradient-ish */
    rectfill(target, x, y, x + w - 1, y + h - 1, theme.button_face);
    hline(target, x, y + h - 1, x + w - 1, theme.button_shadow);
}

Widget *toolbar_create(void) {
    Widget *tb = widget_create(WIDGET_BASE, 0, 0, 400, TOOLBAR_HEIGHT);
    tb->fill = FILL_X;
    tb->layout = LAYOUT_HORIZONTAL;
    tb->spacing = 2;
    tb->draw = toolbar_draw;
    widget_set_padding(tb, 2, 4, 2, 4);
    return tb;
}

static void toolbar_separator_draw(Widget *self, BITMAP *target) {
    int x = self->abs_x + TOOLBAR_SEP_W / 2;
    int y1 = self->abs_y + 2;
    int y2 = self->abs_y + self->h - 3;
    vline(target, x, y1, y2, theme.button_shadow);
    vline(target, x + 1, y1, y2, theme.button_highlight);
}

Widget *toolbar_add_button(Widget *tb, BITMAP *icon, const char *tooltip_text, EventCallback on_click) {
    Widget *btn = image_button_create(icon, 0, 0, TOOLBAR_BTN_SIZE, TOOLBAR_BTN_SIZE);
    btn->align_v = ALIGN_CENTER;
    widget_add_child(tb, btn);

    if (on_click) {
        widget_on(btn, EVENT_CLICK, on_click);
    }

    if (tooltip_text && strlen(tooltip_text) > 0) {
        /* Store tooltip text in name field for identification */
        strncpy(btn->name, tooltip_text, sizeof(btn->name) - 1);
    }

    return btn;
}

Widget *toolbar_add_toggle(Widget *tb, BITMAP *icon, const char *tooltip_text, EventCallback on_click) {
    Widget *btn = image_button_create(icon, 0, 0, TOOLBAR_BTN_SIZE, TOOLBAR_BTN_SIZE);
    btn->align_v = ALIGN_CENTER;
    
    ButtonData *bd = (ButtonData *)btn->extra;
    if (bd) bd->is_toggle = 1;
    
    /* Also set as ToolbarItemData for toggled state access from callbacks */
    widget_add_child(tb, btn);

    if (on_click) {
        widget_on(btn, EVENT_CLICK, on_click);
    }

    if (tooltip_text && strlen(tooltip_text) > 0) {
        strncpy(btn->name, tooltip_text, sizeof(btn->name) - 1);
    }

    return btn;
}

void toolbar_add_separator(Widget *tb) {
    Widget *sep = widget_create(WIDGET_SEPARATOR, 0, 0, TOOLBAR_SEP_W, TOOLBAR_BTN_SIZE);
    sep->draw = toolbar_separator_draw;
    sep->align_v = ALIGN_CENTER;
    widget_add_child(tb, sep);
}
