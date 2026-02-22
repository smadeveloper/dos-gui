#include "gui_menubar.h"
#include "gui_menu.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <string.h>

static void menubar_draw(Widget *self, BITMAP *target) {
    MenubarData *data = (MenubarData *)self->extra;
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + MENUBAR_HEIGHT - 1, theme.menu_bg);
    hline(target, self->abs_x, self->abs_y + MENUBAR_HEIGHT - 1, self->abs_x + self->w - 1, theme.border);
    int tx = self->abs_x + 4;
    for (int i = 0; i < self->child_count; i++) {
        Widget *menu = self->children[i];
        int item_w = text_length(font, menu->name) + 16;
        menu->abs_x = tx; menu->abs_y = self->abs_y; menu->w = item_w; menu->h = MENUBAR_HEIGHT;
        if (i == data->active_index && data->menu_open) {
            rectfill(target, tx, self->abs_y + 1, tx + item_w - 1, self->abs_y + MENUBAR_HEIGHT - 2, theme.menu_highlight);
            textout_ex(target, font, menu->name, tx + 8, self->abs_y + (MENUBAR_HEIGHT - text_height(font)) / 2, theme.menu_highlight_fg, -1);
        } else {
            int hovered = point_in_rect(mouse_x, mouse_y, tx, self->abs_y, item_w, MENUBAR_HEIGHT);
            if (hovered) rectfill(target, tx, self->abs_y + 1, tx + item_w - 1, self->abs_y + MENUBAR_HEIGHT - 2, theme.button_highlight);
            textout_ex(target, font, menu->name, tx + 8, self->abs_y + (MENUBAR_HEIGHT - text_height(font)) / 2, theme.menu_fg, -1);
        }
        tx += item_w;
    }
}

void menubar_draw_menus(Widget *self, BITMAP *target) {
    MenubarData *data = (MenubarData *)self->extra;
    if (!data->menu_open || data->active_index < 0) return;
    if (data->active_index >= self->child_count) return;
    Widget *menu = self->children[data->active_index];
    MenuData *md = (MenuData *)menu->extra;
    if (md->open) menu_draw_popup(menu, target);
}

int menubar_item_at(Widget *menubar, int mx, int my) {
    if (my < menubar->abs_y || my >= menubar->abs_y + MENUBAR_HEIGHT) return -1;
    int tx = menubar->abs_x + 4;
    for (int i = 0; i < menubar->child_count; i++) {
        Widget *menu = menubar->children[i];
        int item_w = text_length(font, menu->name) + 16;
        if (mx >= tx && mx < tx + item_w && my >= menubar->abs_y && my < menubar->abs_y + MENUBAR_HEIGHT) return i;
        tx += item_w;
    }
    return -1;
}

static void menubar_click(Widget *self, Event *event) {
    MenubarData *data = (MenubarData *)self->extra;
    int idx = menubar_item_at(self, event->mouse_x, event->mouse_y);
    if (idx < 0) return;
    if (data->menu_open && data->active_index == idx) {
        menu_close_all(self->children[idx]);
        data->menu_open = 0; data->active_index = -1;
    } else {
        if (data->active_index >= 0 && data->active_index < self->child_count) menu_close_all(self->children[data->active_index]);
        data->active_index = idx; data->menu_open = 1;
        Widget *menu = self->children[idx];
        MenuData *md = (MenuData *)menu->extra;
        md->open = 1; md->popup_x = menu->abs_x; md->popup_y = self->abs_y + MENUBAR_HEIGHT; md->hover_index = -1;
        menu_calc_geometry(menu);
    }
    self->dirty = 1; event->handled = 1;
}

Widget *menubar_create(void) {
    Widget *wid = widget_create(WIDGET_MENUBAR, 0, 0, 100, MENUBAR_HEIGHT);
    MenubarData *data = (MenubarData *)calloc(1, sizeof(MenubarData));
    data->active_index = -1;
    wid->extra = data;
    wid->draw = menubar_draw;
    wid->fill = FILL_X;
    widget_on(wid, EVENT_CLICK, menubar_click);
    return wid;
}

Widget *menubar_add_menu(Widget *menubar, const char *title) {
    Widget *menu = menu_create();
    strncpy(menu->name, title, 63);
    widget_add_child(menubar, menu);
    return menu;
}
