#include "gui_tab.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

static void tab_draw(Widget *self, BITMAP *target) {
    TabData *data = (TabData *)self->extra;
    if (!data) return;

    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + TAB_HEIGHT - 1, theme.bg);

    int tx = self->abs_x + 2;
    for (int i = 0; i < data->tab_count; i++) {
        int tw = text_length(font, data->tab_titles[i]) + 20;
        int active = (i == data->active_tab);

        if (active) {
            rectfill(target, tx, self->abs_y, tx + tw - 1, self->abs_y + TAB_HEIGHT, theme.tab_active);
            rect(target, tx, self->abs_y, tx + tw - 1, self->abs_y + TAB_HEIGHT - 1, theme.tab_border);
            hline(target, tx + 1, self->abs_y + TAB_HEIGHT, tx + tw - 2, theme.tab_active);
        } else {
            int hovered = point_in_rect(mouse_x, mouse_y, tx, self->abs_y, tw, TAB_HEIGHT);
            rectfill(target, tx, self->abs_y + 2, tx + tw - 1, self->abs_y + TAB_HEIGHT - 1,
                     hovered ? theme.button_highlight : theme.tab_inactive);
            rect(target, tx, self->abs_y + 2, tx + tw - 1, self->abs_y + TAB_HEIGHT - 1, theme.tab_border);
        }

        textout_centre_ex(target, font, data->tab_titles[i],
                          tx + tw / 2, self->abs_y + (TAB_HEIGHT - text_height(font)) / 2 + (active ? 0 : 1),
                          theme.fg, -1);

        tx += tw;
    }

    rect(target, self->abs_x, self->abs_y + TAB_HEIGHT,
         self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.tab_border);
    rectfill(target, self->abs_x + 1, self->abs_y + TAB_HEIGHT + 1,
             self->abs_x + self->w - 2, self->abs_y + self->h - 2, theme.tab_active);
}

static void tab_handle_click(Widget *self, Event *event) {
    TabData *data = (TabData *)self->extra;
    if (!data) return;
    if (event->mouse_y > self->abs_y + TAB_HEIGHT) return;

    int tx = self->abs_x + 2;
    for (int i = 0; i < data->tab_count; i++) {
        int tw = text_length(font, data->tab_titles[i]) + 20;
        if (point_in_rect(event->mouse_x, event->mouse_y, tx, self->abs_y, tw, TAB_HEIGHT)) {
            if (data->active_tab != i) {
                if (data->active_tab >= 0 && data->active_tab < data->tab_count && data->pages[data->active_tab])
                    data->pages[data->active_tab]->visible = 0;
                data->active_tab = i;
                if (data->pages[data->active_tab])
                    data->pages[data->active_tab]->visible = 1;
                widget_emit_simple(self, EVENT_TAB_CHANGE);
                widget_mark_dirty(self);
            }
            event->handled = 1;
            break;
        }
        tx += tw;
    }
}

Widget *tab_create(int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_TAB, x, y, w, h);
    TabData *data = (TabData *)calloc(1, sizeof(TabData));
    wid->extra = data;
    wid->draw = tab_draw;
    /* Use both CLICK and MOUSE_DOWN for tab switching */
    widget_on(wid, EVENT_CLICK, tab_handle_click);
    widget_on(wid, EVENT_MOUSE_DOWN, tab_handle_click);
    return wid;
}

Widget *tab_add_page(Widget *tab, const char *title) {
    TabData *data = (TabData *)tab->extra;
    if (!data || data->tab_count >= MAX_TAB_PAGES) return NULL;

    Widget *page = widget_create(WIDGET_TAB_PAGE, 2, TAB_HEIGHT + 2, tab->w - 4, tab->h - TAB_HEIGHT - 4);
    page->layout = LAYOUT_VERTICAL;
    page->spacing = 4;
    widget_set_padding_all(page, 6);

    strncpy(data->tab_titles[data->tab_count], title, MAX_TEXT_LEN - 1);
    data->pages[data->tab_count] = page;

    if (data->tab_count > 0) page->visible = 0;

    data->tab_count++;
    widget_add_child(tab, page);
    return page;
}

Widget *tab_add(Widget *tab, const char *title) {
    return tab_add_page(tab, title);
}
