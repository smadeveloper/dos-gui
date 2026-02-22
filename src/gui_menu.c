#include "gui_menu.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <string.h>

Widget *menu_create(void) {
    Widget *wid = widget_create(WIDGET_MENU, 0, 0, 0, 0);
    MenuData *data = (MenuData *)calloc(1, sizeof(MenuData));
    data->hover_index = -1;
    wid->extra = data;
    wid->visible = 0;
    return wid;
}

Widget *menu_add_item(Widget *menu, const char *text, const char *shortcut, EventCallback on_click) {
    Widget *item = widget_create(WIDGET_MENU_ITEM, 0, 0, 0, 0);
    MenuItemData *data = (MenuItemData *)calloc(1, sizeof(MenuItemData));
    strncpy(data->text, text, MAX_TEXT_LEN - 1);
    if (shortcut) strncpy(data->shortcut, shortcut, 31);
    data->enabled = 1;
    item->extra = data;
    item->enabled = 1;
    if (on_click) widget_on(item, EVENT_CLICK, on_click);
    widget_add_child(menu, item);
    return item;
}

Widget *menu_add_separator(Widget *menu) {
    Widget *item = widget_create(WIDGET_MENU_ITEM, 0, 0, 0, 0);
    MenuItemData *data = (MenuItemData *)calloc(1, sizeof(MenuItemData));
    data->separator = 1;
    data->enabled = 0;
    item->extra = data;
    widget_add_child(menu, item);
    return item;
}

Widget *menu_add_submenu(Widget *menu, const char *text, Widget *submenu) {
    Widget *item = menu_add_item(menu, text, NULL, NULL);
    MenuItemData *data = (MenuItemData *)item->extra;
    data->has_submenu = 1;
    data->submenu = submenu;
    return item;
}

void menu_calc_geometry(Widget *self) {
    MenuData *data = (MenuData *)self->extra;
    int item_h = text_height(font) + 8;
    int sep_h = 6;
    int menu_w = 160;
    int menu_h = 2;
    for (int i = 0; i < self->child_count; i++) {
        MenuItemData *mid = (MenuItemData *)self->children[i]->extra;
        if (mid->separator) { menu_h += sep_h; }
        else {
            int tw = text_length(font, mid->text) + 50;
            if (mid->shortcut[0]) tw += text_length(font, mid->shortcut) + 20;
            if (mid->has_submenu) tw += 16;
            if (tw > menu_w) menu_w = tw;
            menu_h += item_h;
        }
    }
    int mx = data->popup_x, my = data->popup_y;
    if (mx + menu_w > gui_state.screen_w) mx = gui_state.screen_w - menu_w;
    if (my + menu_h > gui_state.screen_h) my = gui_state.screen_h - menu_h;
    if (mx < 0) mx = 0; if (my < 0) my = 0;
    data->popup_x = mx; data->popup_y = my;
    data->popup_w = menu_w; data->popup_h = menu_h;
    int iy = my + 1;
    for (int i = 0; i < self->child_count; i++) {
        MenuItemData *mid = (MenuItemData *)self->children[i]->extra;
        Widget *item = self->children[i];
        if (mid->separator) {
            item->abs_x = mx + 1; item->abs_y = iy; item->w = menu_w - 2; item->h = sep_h;
            iy += sep_h;
        } else {
            item->abs_x = mx + 1; item->abs_y = iy; item->w = menu_w - 2; item->h = item_h;
            iy += item_h;
        }
    }
}

void menu_draw_popup(Widget *self, BITMAP *target) {
    MenuData *data = (MenuData *)self->extra;
    if (!data->open) return;
    int item_h = text_height(font) + 8;
    int sep_h = 6;
    menu_calc_geometry(self);
    int mx = data->popup_x, my = data->popup_y;
    int menu_w = data->popup_w, menu_h = data->popup_h;
    rectfill(target, mx, my, mx + menu_w - 1, my + menu_h - 1, theme.menu_bg);
    rect(target, mx, my, mx + menu_w - 1, my + menu_h - 1, theme.border);
    for (int i = 0; i < self->child_count; i++) {
        MenuItemData *mid = (MenuItemData *)self->children[i]->extra;
        Widget *item = self->children[i];
        if (mid->separator) {
            hline(target, mx + 4, item->abs_y + sep_h / 2, mx + menu_w - 5, theme.border);
        } else {
            if (i == data->hover_index && mid->enabled) {
                rectfill(target, item->abs_x, item->abs_y,
                         item->abs_x + item->w - 1, item->abs_y + item->h - 1, theme.menu_highlight);
                textout_ex(target, font, mid->text, item->abs_x + 23, item->abs_y + 4, theme.menu_highlight_fg, -1);
                if (mid->shortcut[0]) {
                    textout_right_ex(target, font, mid->shortcut, mx + menu_w - 8, item->abs_y + 4, theme.menu_highlight_fg, -1);
                }
            } else {
                int tc = mid->enabled ? theme.menu_fg : theme.disabled_fg;
                textout_ex(target, font, mid->text, item->abs_x + 23, item->abs_y + 4, tc, -1);
                if (mid->shortcut[0]) {
                    textout_right_ex(target, font, mid->shortcut, mx + menu_w - 8, item->abs_y + 4, tc, -1);
                }
            }
            if (mid->checked) {
                int cx_pos = item->abs_x + 7, cy_pos = item->abs_y + item->h / 2;
                line(target, cx_pos, cy_pos, cx_pos + 3, cy_pos + 3, theme.checkbox_check);
                line(target, cx_pos + 3, cy_pos + 3, cx_pos + 9, cy_pos - 3, theme.checkbox_check);
            }
            if (mid->has_submenu) {
                int ax = item->abs_x + item->w - 12, ay = item->abs_y + item->h / 2;
                int ac = (i == data->hover_index) ? theme.menu_highlight_fg : theme.menu_fg;
                triangle(target, ax, ay - 4, ax, ay + 4, ax + 5, ay, ac);
            }
        }
    }
    for (int i = 0; i < self->child_count; i++) {
        MenuItemData *mid = (MenuItemData *)self->children[i]->extra;
        if (mid->has_submenu && mid->submenu) {
            MenuData *sub = (MenuData *)mid->submenu->extra;
            if (sub->open) menu_draw_popup(mid->submenu, target);
        }
    }
}

void menu_close_all(Widget *menu) {
    MenuData *data = (MenuData *)menu->extra;
    data->open = 0;
    data->hover_index = -1;
    for (int i = 0; i < menu->child_count; i++) {
        MenuItemData *mid = (MenuItemData *)menu->children[i]->extra;
        if (mid->has_submenu && mid->submenu) menu_close_all(mid->submenu);
    }
}

int menu_hit_test_deep(Widget *menu, int mx, int my, Widget **out_menu, int *out_index) {
    MenuData *data = (MenuData *)menu->extra;
    if (!data->open) return 0;
    for (int i = 0; i < menu->child_count; i++) {
        MenuItemData *mid = (MenuItemData *)menu->children[i]->extra;
        if (mid->has_submenu && mid->submenu) {
            MenuData *sub = (MenuData *)mid->submenu->extra;
            if (sub->open) {
                if (menu_hit_test_deep(mid->submenu, mx, my, out_menu, out_index)) return 1;
            }
        }
    }
    if (point_in_rect(mx, my, data->popup_x, data->popup_y, data->popup_w, data->popup_h)) {
        for (int i = 0; i < menu->child_count; i++) {
            MenuItemData *mid = (MenuItemData *)menu->children[i]->extra;
            if (mid->separator) continue;
            Widget *item = menu->children[i];
            if (point_in_rect(mx, my, item->abs_x, item->abs_y, item->w, item->h)) {
                *out_menu = menu; *out_index = i; return 1;
            }
        }
        *out_menu = menu; *out_index = -1; return 1;
    }
    return 0;
}

int menu_is_point_in_any(Widget *menu, int mx, int my) {
    Widget *dummy_menu; int dummy_idx;
    return menu_hit_test_deep(menu, mx, my, &dummy_menu, &dummy_idx);
}

void menu_update_hover_recursive(Widget *menu, int mx, int my) {
    MenuData *data = (MenuData *)menu->extra;
    if (!data->open) return;
    menu_calc_geometry(menu);
    if (point_in_rect(mx, my, data->popup_x, data->popup_y, data->popup_w, data->popup_h)) {
        int new_hover = -1;
        for (int i = 0; i < menu->child_count; i++) {
            MenuItemData *mid = (MenuItemData *)menu->children[i]->extra;
            if (mid->separator) continue;
            Widget *item = menu->children[i];
            if (point_in_rect(mx, my, item->abs_x, item->abs_y, item->w, item->h)) {
                new_hover = i; break;
            }
        }
        if (new_hover != data->hover_index) {
            if (data->hover_index >= 0 && data->hover_index < menu->child_count) {
                MenuItemData *old_mid = (MenuItemData *)menu->children[data->hover_index]->extra;
                if (old_mid->has_submenu && old_mid->submenu) menu_close_all(old_mid->submenu);
            }
            data->hover_index = new_hover;
            if (new_hover >= 0) {
                MenuItemData *mid = (MenuItemData *)menu->children[new_hover]->extra;
                if (mid->has_submenu && mid->submenu) {
                    MenuData *sub = (MenuData *)mid->submenu->extra;
                    Widget *item = menu->children[new_hover];
                    sub->open = 1;
                    sub->popup_x = item->abs_x + item->w;
                    sub->popup_y = item->abs_y;
                    sub->hover_index = -1;
                    menu_calc_geometry(mid->submenu);
                }
            }
        }
    } else {
        for (int i = 0; i < menu->child_count; i++) {
            MenuItemData *mid = (MenuItemData *)menu->children[i]->extra;
            if (mid->has_submenu && mid->submenu) {
                MenuData *sub = (MenuData *)mid->submenu->extra;
                if (sub->open) {
                    if (menu_is_point_in_any(mid->submenu, mx, my)) {
                        menu_update_hover_recursive(mid->submenu, mx, my);
                        return;
                    }
                }
            }
        }
    }
}
