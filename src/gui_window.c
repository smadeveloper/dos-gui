#include "gui_window.h"
#include "gui_button.h"
#include "gui_panel.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <string.h>

/* ========================================================================
 * WINDOW BUTTON ICONS
 * ======================================================================== */

BITMAP *create_close_icon(void) {
    BITMAP *bmp = create_bitmap(16, 16);
    clear_to_color(bmp, makecol(255, 0, 255));
    int c = makecol(220, 60, 60);
    line(bmp, 3, 3, 12, 12, c);
    line(bmp, 4, 3, 12, 11, c);
    line(bmp, 3, 4, 11, 12, c);
    line(bmp, 12, 3, 3, 12, c);
    line(bmp, 11, 3, 3, 11, c);
    line(bmp, 12, 4, 4, 12, c);
    return bmp;
}

BITMAP *create_maximize_icon(void) {
    BITMAP *bmp = create_bitmap(16, 16);
    clear_to_color(bmp, makecol(255, 0, 255));
    int c = makecol(80, 80, 80);
    rect(bmp, 3, 3, 12, 12, c);
    rect(bmp, 4, 4, 11, 11, c);
    hline(bmp, 3, 3, 12, c);
    hline(bmp, 3, 4, 12, c);
    return bmp;
}

BITMAP *create_restore_icon(void) {
    BITMAP *bmp = create_bitmap(16, 16);
    clear_to_color(bmp, makecol(255, 0, 255));
    int c = makecol(80, 80, 80);
    /* Back window */
    rect(bmp, 5, 2, 13, 10, c);
    hline(bmp, 5, 2, 13, c);
    hline(bmp, 5, 3, 13, c);
    /* Front window */
    rect(bmp, 2, 5, 10, 13, c);
    hline(bmp, 2, 5, 10, c);
    hline(bmp, 2, 6, 10, c);
    rectfill(bmp, 3, 7, 9, 12, makecol(255, 0, 255));
    return bmp;
}

/* ========================================================================
 * ACTIVE WINDOW MANAGEMENT
 * ======================================================================== */

void set_active_window(Widget *win) {
    if (gui_state.active_window == win) return;

    Widget *old_active = gui_state.active_window;
    gui_state.active_window = win;

    if (old_active && old_active->extra) {
        WindowData *od = (WindowData *)old_active->extra;
        od->active = 0;
        widget_mark_dirty(old_active);
    }
    if (win && win->extra) {
        WindowData *nd = (WindowData *)win->extra;
        nd->active = 1;
        widget_mark_dirty(win);
    }
}

/* ========================================================================
 * WINDOW INTERNALS
 * ======================================================================== */

static BITMAP *icon_close = NULL;
static BITMAP *icon_maximize = NULL;
static BITMAP *icon_restore = NULL;

static void window_draw(Widget *self, BITMAP *target);
static void window_do_layout(Widget *self);

static void window_btn_close_click(Widget *self, Event *event) {
    (void)event;
    Widget *win = widget_find_parent_window(self);
    if (win) {
        widget_emit_simple(win, EVENT_CLOSE);
    }
}

static void window_btn_maximize_click(Widget *self, Event *event) {
    (void)event;
    Widget *win = widget_find_parent_window(self);
    if (!win) return;
    WindowData *data = (WindowData *)win->extra;
    if (!data) return;

    if (data->maximized) {
        win->x = data->old_x; win->y = data->old_y;
        win->w = data->old_w; win->h = data->old_h;
        data->maximized = 0;
        ButtonData *bd = (ButtonData *)data->btn_maximize->extra;
        if (bd) bd->icon = icon_maximize;
    } else {
        data->old_x = win->x; data->old_y = win->y;
        data->old_w = win->w; data->old_h = win->h;
        win->x = 0; win->y = 0;
        win->w = gui_state.screen_w; win->h = gui_state.screen_h;
        data->maximized = 1;
        ButtonData *bd = (ButtonData *)data->btn_maximize->extra;
        if (bd) bd->icon = icon_restore;
    }
    win->needs_layout = 1;
    widget_mark_dirty(win);
}

static void window_default_close(Widget *self, Event *event) {
    (void)event;
    for (int i = 0; i < gui_state.window_count; i++) {
        if (gui_state.windows[i] == self) {
            for (int j = i; j < gui_state.window_count - 1; j++)
                gui_state.windows[j] = gui_state.windows[j + 1];
            gui_state.window_count--;
            break;
        }
    }

    if (gui_state.active_window == self)
        gui_state.active_window = NULL;
    if (gui_state.dragging_window == self)
        gui_state.dragging_window = NULL;
    if (gui_state.focused_widget && widget_find_parent_window(gui_state.focused_widget) == self)
        gui_state.focused_widget = NULL;
    if (gui_state.hovered_widget && widget_find_parent_window(gui_state.hovered_widget) == self)
        gui_state.hovered_widget = NULL;
    if (gui_state.captured_widget && widget_find_parent_window(gui_state.captured_widget) == self)
        gui_state.captured_widget = NULL;

    widget_destroy(self);
    gui_state.needs_redraw = 1;
}

static void window_do_layout(Widget *self) {
    WindowData *data = (WindowData *)self->extra;
    if (!data) return;

    int bx = self->w - 4;
    if (data->btn_close) {
        bx -= data->btn_close->w;
        data->btn_close->x = bx;
        data->btn_close->y = 2;
        bx -= 2;
    }
    if (data->btn_maximize) {
        bx -= data->btn_maximize->w;
        data->btn_maximize->x = bx;
        data->btn_maximize->y = 2;
    }
    if (data->client_area) {
        int cy = TITLEBAR_HEIGHT;
        int ch = self->h - TITLEBAR_HEIGHT;
        if (data->menubar) {
            data->menubar->x = 0;
            data->menubar->y = cy;
            data->menubar->w = self->w;
            cy += MENUBAR_HEIGHT;
            ch -= MENUBAR_HEIGHT;
        }
        if (data->statusbar) {
            data->statusbar->x = 0;
            data->statusbar->w = self->w;
            ch -= STATUSBAR_HEIGHT;
            data->statusbar->y = self->h - STATUSBAR_HEIGHT;
        }
        data->client_area->x = 1;
        data->client_area->y = cy;
        data->client_area->w = self->w - 2;
        data->client_area->h = ch;
        data->client_area->needs_layout = 1;
    }
}

static void window_draw(Widget *self, BITMAP *target) {
    WindowData *data = (WindowData *)self->extra;
    if (!data) return;

    int active = data->active;

    /* Shadow */
    rectfill(target, self->abs_x + 3, self->abs_y + 3,
             self->abs_x + self->w + 2, self->abs_y + self->h + 2,
             makecol(0, 0, 0));

    /* Window body */
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.bg);
    rect(target, self->abs_x, self->abs_y,
         self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.border);

    /* Titlebar */
    int tb_color = active ? theme.titlebar_active : theme.titlebar_inactive;
    rectfill(target, self->abs_x + 1, self->abs_y + 1,
             self->abs_x + self->w - 2, self->abs_y + TITLEBAR_HEIGHT - 1, tb_color);
    textout_ex(target, font, data->title,
               self->abs_x + 8, self->abs_y + (TITLEBAR_HEIGHT - text_height(font)) / 2,
               makecol(255, 255, 255), -1);
}

/* ========================================================================
 * PUBLIC API
 * ======================================================================== */

Widget *window_create(const char *title, int x, int y, int w, int h) {
    if (!icon_close) {
        icon_close = create_close_icon();
        icon_maximize = create_maximize_icon();
        icon_restore = create_restore_icon();
    }

    Widget *win = widget_create(WIDGET_WINDOW, x, y, w, h);
    WindowData *data = (WindowData *)calloc(1, sizeof(WindowData));
    strncpy(data->title, title, MAX_TEXT_LEN - 1);
    data->z_index = gui_state.next_z_index++;
    data->closable = 1;
    data->maximizable = 1;
    data->resizable = 1;
    data->active = 0;

    win->extra = data;
    win->draw = window_draw;
    win->do_layout = window_do_layout;
    win->clip_children = 1;

    /* Close button */
    data->btn_close = image_button_create(icon_close, w - 22, 2, 20, 20);
    widget_add_child(win, data->btn_close);
    widget_on(data->btn_close, EVENT_CLICK, window_btn_close_click);

    /* Maximize button */
    data->btn_maximize = image_button_create(icon_maximize, w - 44, 2, 20, 20);
    widget_add_child(win, data->btn_maximize);
    widget_on(data->btn_maximize, EVENT_CLICK, window_btn_maximize_click);

    /* Client area */
    data->client_area = panel_create(1, TITLEBAR_HEIGHT, w - 2, h - TITLEBAR_HEIGHT - 1);
    data->client_area->layout = LAYOUT_VERTICAL;
    data->client_area->spacing = 4;
    widget_set_padding_all(data->client_area, 6);
    widget_add_child(win, data->client_area);

    /* Default close handler */
    widget_on(win, EVENT_CLOSE, window_default_close);

    /* Track window */
    if (gui_state.window_count < MAX_WINDOWS) {
        gui_state.windows[gui_state.window_count++] = win;
    }

    /* Add to root widget */
    if (gui_state.root) {
        widget_add_child(gui_state.root, win);
    }

    /* Set as active */
    set_active_window(win);

    return win;
}

Widget *window_get_client(Widget *win) {
    WindowData *data = (WindowData *)win->extra;
    return data ? data->client_area : NULL;
}

void window_set_menubar(Widget *win, Widget *menubar) {
    WindowData *data = (WindowData *)win->extra;
    if (!data) return;
    data->menubar = menubar;
    widget_add_child(win, menubar);
    win->needs_layout = 1;
}

void window_set_statusbar(Widget *win, Widget *statusbar) {
    WindowData *data = (WindowData *)win->extra;
    if (!data) return;
    data->statusbar = statusbar;
    widget_add_child(win, statusbar);
    win->needs_layout = 1;
}

void window_bring_to_front(Widget *win) {
    int idx = -1;
    for (int i = 0; i < gui_state.window_count; i++) {
        if (gui_state.windows[i] == win) { idx = i; break; }
    }
    if (idx < 0 || idx == gui_state.window_count - 1) return;
    for (int i = idx; i < gui_state.window_count - 1; i++) {
        gui_state.windows[i] = gui_state.windows[i + 1];
    }
    gui_state.windows[gui_state.window_count - 1] = win;
    WindowData *data = (WindowData *)win->extra;
    if (data) data->z_index = gui_state.next_z_index++;
}

Widget *window_at_point(int mx, int my) {
    for (int i = gui_state.window_count - 1; i >= 0; i--) {
        Widget *w = gui_state.windows[i];
        if (w->visible && point_in_rect(mx, my, w->abs_x, w->abs_y, w->w, w->h))
            return w;
    }
    return NULL;
}
