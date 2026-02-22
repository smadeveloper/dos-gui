#include "gui_utils.h"
#include "gui_state.h"
#include <allegro.h>

ThemeColors theme;

void init_theme(void) {
    theme.bg = makecol(240, 240, 240);
    theme.fg = makecol(30, 30, 30);
    theme.border = makecol(180, 180, 180);
    theme.titlebar_active = makecol(60, 120, 200);
    theme.titlebar_inactive = makecol(150, 150, 150);
    theme.button_face = makecol(225, 225, 225);
    theme.button_highlight = makecol(240, 240, 240);
    theme.button_shadow = makecol(160, 160, 160);
    theme.button_text = makecol(30, 30, 30);
    theme.input_bg = makecol(255, 255, 255);
    theme.input_fg = makecol(0, 0, 0);
    theme.input_border = makecol(180, 180, 180);
    theme.input_focus_border = makecol(60, 120, 200);
    theme.menu_bg = makecol(250, 250, 250);
    theme.menu_fg = makecol(30, 30, 30);
    theme.menu_highlight = makecol(60, 120, 200);
    theme.menu_highlight_fg = makecol(255, 255, 255);
    theme.checkbox_bg = makecol(255, 255, 255);
    theme.checkbox_check = makecol(60, 120, 200);
    theme.scrollbar_bg = makecol(230, 230, 230);
    theme.scrollbar_thumb = makecol(180, 180, 180);
    theme.scrollbar_arrow = makecol(100, 100, 100);
    theme.tab_active = makecol(255, 255, 255);
    theme.tab_inactive = makecol(220, 220, 220);
    theme.tab_border = makecol(180, 180, 180);
    theme.statusbar_bg = makecol(230, 230, 230);
    theme.statusbar_fg = makecol(60, 60, 60);
    theme.tooltip_bg = makecol(255, 255, 225);
    theme.tooltip_fg = makecol(0, 0, 0);
    theme.tooltip_border = makecol(100, 100, 100);
    theme.selection_bg = makecol(60, 120, 200);
    theme.selection_fg = makecol(255, 255, 255);
    theme.disabled_fg = makecol(160, 160, 160);
    theme.desktop = makecol(58, 110, 165);
}

void theme_init_default(void) {
    init_theme();
}

int clamp_int(int val, int lo, int hi) {
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

int point_in_rect(int px, int py, int rx, int ry, int rw, int rh) {
    return px >= rx && py >= ry && px < rx + rw && py < ry + rh;
}

void draw_raised_rect(BITMAP *bmp, int x, int y, int w, int h) {
    rectfill(bmp, x, y, x + w - 1, y + h - 1, theme.button_face);
    hline(bmp, x, y, x + w - 1, theme.button_highlight);
    vline(bmp, x, y, y + h - 1, theme.button_highlight);
    hline(bmp, x, y + h - 1, x + w - 1, theme.button_shadow);
    vline(bmp, x + w - 1, y, y + h - 1, theme.button_shadow);
}

void draw_sunken_rect(BITMAP *bmp, int x, int y, int w, int h) {
    rectfill(bmp, x, y, x + w - 1, y + h - 1, theme.input_bg);
    hline(bmp, x, y, x + w - 1, theme.button_shadow);
    vline(bmp, x, y, y + h - 1, theme.button_shadow);
    hline(bmp, x, y + h - 1, x + w - 1, theme.button_highlight);
    vline(bmp, x + w - 1, y, y + h - 1, theme.button_highlight);
}

void draw_3d_rect(BITMAP *bmp, int x, int y, int w, int h, int pressed) {
    if (pressed) draw_sunken_rect(bmp, x, y, w, h);
    else draw_raised_rect(bmp, x, y, w, h);
}
