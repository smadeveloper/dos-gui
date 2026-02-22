#include "gui_widget.h"
#include "gui_state.h"
#include "gui_utils.h"
#include "gui_box.h"
#include <stdlib.h>
#include <string.h>

void widget_init(Widget *w, WidgetType type, int x, int y, int ww, int hh) {
    memset(w, 0, sizeof(Widget));
    w->type = type;
    w->id = gui_state.next_id++;
    w->x = x;
    w->y = y;
    w->w = ww;
    w->h = hh;
    w->visible = 1;
    w->enabled = 1;
    w->dirty = 1;
    w->needs_layout = 1;
    w->clip_children = 1;
    w->min_w = 0;
    w->min_h = 0;
    w->max_w = 9999;
    w->max_h = 9999;
    w->expand = 0;
    w->expand_weight = 1;
}

Widget *widget_create(WidgetType type, int x, int y, int w, int h) {
    Widget *widget = (Widget *)calloc(1, sizeof(Widget));
    if (!widget) return NULL;
    widget_init(widget, type, x, y, w, h);
    return widget;
}

void widget_on(Widget *w, EventType type, EventCallback cb) {
    EventHandler *h = (EventHandler *)malloc(sizeof(EventHandler));
    h->type = type;
    h->callback = cb;
    h->next = w->handlers;
    w->handlers = h;
}

void widget_emit(Widget *w, Event *event) {
    if (!w) return;
    event->target = w;
    EventHandler *h = w->handlers;
    while (h) {
        if (h->type == event->type && h->callback) {
            h->callback(w, event);
            if (event->handled) return;
        }
        h = h->next;
    }
}

void widget_handle_event(Widget *w, Event *event) {
    widget_emit(w, event);
}

void widget_emit_simple(Widget *w, EventType type) {
    Event e;
    memset(&e, 0, sizeof(e));
    e.type = type;
    widget_emit(w, &e);
}

int widget_add_child(Widget *parent, Widget *child) {
    if (!parent || !child) return -1;
    if (parent->child_count >= MAX_CHILDREN) return -1;
    child->parent = parent;
    parent->children[parent->child_count++] = child;
    parent->needs_layout = 1;
    parent->dirty = 1;
    return 0;
}

void widget_remove_child(Widget *parent, Widget *child) {
    if (!parent || !child) return;
    for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == child) {
            child->parent = NULL;
            for (int j = i; j < parent->child_count - 1; j++)
                parent->children[j] = parent->children[j + 1];
            parent->child_count--;
            parent->needs_layout = 1;
            parent->dirty = 1;
            return;
        }
    }
}

void widget_destroy(Widget *w) {
    if (!w) return;

    for (int i = w->child_count - 1; i >= 0; i--) {
        widget_destroy(w->children[i]);
        w->children[i] = NULL;
    }
    w->child_count = 0;

    if (w->destroy) w->destroy(w);

    EventHandler *h = w->handlers;
    while (h) {
        EventHandler *next = h->next;
        free(h);
        h = next;
    }

    if (w->bitmap) {
        destroy_bitmap(w->bitmap);
        w->bitmap = NULL;
    }
    if (w->extra) {
        free(w->extra);
        w->extra = NULL;
    }

    if (w->parent) {
        widget_remove_child(w->parent, w);
    }

    if (gui_state.focused_widget == w) gui_state.focused_widget = NULL;
    if (gui_state.hovered_widget == w) gui_state.hovered_widget = NULL;
    if (gui_state.captured_widget == w) gui_state.captured_widget = NULL;

    free(w);
}

void widget_set_padding(Widget *w, int top, int right, int bottom, int left) {
    w->padding_top = top;
    w->padding_right = right;
    w->padding_bottom = bottom;
    w->padding_left = left;
    w->needs_layout = 1;
}

void widget_set_margin(Widget *w, int top, int right, int bottom, int left) {
    w->margin_top = top;
    w->margin_right = right;
    w->margin_bottom = bottom;
    w->margin_left = left;
    if (w->parent) w->parent->needs_layout = 1;
}

void widget_set_padding_all(Widget *w, int p) {
    widget_set_padding(w, p, p, p, p);
}

void widget_set_margin_all(Widget *w, int m) {
    widget_set_margin(w, m, m, m, m);
}

void widget_compute_absolute(Widget *w) {
    if (w->parent) {
        w->abs_x = w->parent->abs_x + w->x - w->parent->scroll_x;
        w->abs_y = w->parent->abs_y + w->y - w->parent->scroll_y;
    } else {
        w->abs_x = w->x;
        w->abs_y = w->y;
    }
    for (int i = 0; i < w->child_count; i++) {
        widget_compute_absolute(w->children[i]);
    }
}

void widget_do_layout(Widget *w) {
    if (!w->needs_layout && w->layout == LAYOUT_NONE &&
        w->type != WIDGET_HBOX && w->type != WIDGET_VBOX) {
        for (int i = 0; i < w->child_count; i++) {
            if (w->children[i]->needs_layout)
                widget_do_layout(w->children[i]);
        }
        return;
    }

    if ((w->type == WIDGET_HBOX || w->type == WIDGET_VBOX) && w->do_layout) {
        w->do_layout(w);
        w->needs_layout = 0;
        for (int i = 0; i < w->child_count; i++) {
            widget_do_layout(w->children[i]);
        }
        return;
    }

    int cx = w->padding_left;
    int cy = w->padding_top;
    int avail_w = w->w - w->padding_left - w->padding_right;
    int avail_h = w->h - w->padding_top - w->padding_bottom;

    if (w->layout == LAYOUT_VERTICAL) {
        for (int i = 0; i < w->child_count; i++) {
            Widget *c = w->children[i];
            if (!c->visible) continue;

            if (c->type == WIDGET_HBOX || c->type == WIDGET_VBOX) {
                int cw, ch;
                box_calc_content_size(c, &cw, &ch);
                if (c->h < ch) c->h = ch;
                if (!(c->fill & FILL_X) && c->w < cw) c->w = cw;
            }

            c->x = cx + c->margin_left;
            c->y = cy + c->margin_top;

            if (c->fill & FILL_X) {
                c->w = avail_w - c->margin_left - c->margin_right;
            }

            if (c->align_h == ALIGN_CENTER && !(c->fill & FILL_X)) {
                c->x = cx + (avail_w - c->w) / 2;
            } else if (c->align_h == ALIGN_RIGHT && !(c->fill & FILL_X)) {
                c->x = cx + avail_w - c->w - c->margin_right;
            }

            cy += c->margin_top + c->h + c->margin_bottom + w->spacing;
        }
        w->content_h = cy - w->spacing + w->padding_bottom;
        w->content_w = w->w;

    } else if (w->layout == LAYOUT_HORIZONTAL) {
        for (int i = 0; i < w->child_count; i++) {
            Widget *c = w->children[i];
            if (!c->visible) continue;

            if (c->type == WIDGET_HBOX || c->type == WIDGET_VBOX) {
                int cw, ch;
                box_calc_content_size(c, &cw, &ch);
                if (c->w < cw) c->w = cw;
                if (!(c->fill & FILL_Y) && c->h < ch) c->h = ch;
            }

            c->x = cx + c->margin_left;
            c->y = cy + c->margin_top;

            if (c->fill & FILL_Y) {
                c->h = avail_h - c->margin_top - c->margin_bottom;
            }

            if (c->align_v == ALIGN_CENTER && !(c->fill & FILL_Y)) {
                c->y = cy + (avail_h - c->h) / 2;
            } else if (c->align_v == ALIGN_BOTTOM && !(c->fill & FILL_Y)) {
                c->y = cy + avail_h - c->h - c->margin_bottom;
            }

            cx += c->margin_left + c->w + c->margin_right + w->spacing;
        }
        w->content_w = cx - w->spacing + w->padding_right;
        w->content_h = w->h;
    }

    w->needs_layout = 0;

    if (w->do_layout && w->type != WIDGET_HBOX && w->type != WIDGET_VBOX)
        w->do_layout(w);

    for (int i = 0; i < w->child_count; i++) {
        widget_do_layout(w->children[i]);
    }
}

void widget_mark_dirty(Widget *w) {
    w->dirty = 1;
    for (int i = 0; i < w->child_count; i++) {
        widget_mark_dirty(w->children[i]);
    }
}

int widget_default_hit_test(Widget *w, int mx, int my) {
    return point_in_rect(mx, my, w->abs_x, w->abs_y, w->w, w->h);
}

Widget *widget_hit_test_recursive(Widget *w, int mx, int my) {
    if (!w->visible || !w->enabled) return NULL;
    int hit;
    if (w->hit_test) hit = w->hit_test(w, mx, my);
    else hit = widget_default_hit_test(w, mx, my);
    if (!hit) return NULL;

    for (int i = w->child_count - 1; i >= 0; i--) {
        Widget *result = widget_hit_test_recursive(w->children[i], mx, my);
        if (result) return result;
    }
    return w;
}

void widget_set_focus(Widget *w) {
    if (gui_state.focused_widget == w) return;
    if (gui_state.focused_widget) {
        gui_state.focused_widget->focused = 0;
        widget_emit_simple(gui_state.focused_widget, EVENT_BLUR);
        gui_state.focused_widget->dirty = 1;
    }
    gui_state.focused_widget = w;
    if (w) {
        w->focused = 1;
        widget_emit_simple(w, EVENT_FOCUS);
        w->dirty = 1;
    }
}

void widget_draw_recursive(Widget *w, BITMAP *target) {
    if (!w->visible) return;
    if (w->draw) w->draw(w, target);

    int old_cl, old_ct, old_cr, old_cb;
    if (w->clip_children && w->type != WIDGET_WINDOW) {
        old_cl = target->cl;
        old_ct = target->ct;
        old_cr = target->cr;
        old_cb = target->cb;

        int cl = w->abs_x + w->padding_left;
        int ct = w->abs_y + w->padding_top;
        int cr = w->abs_x + w->w - w->padding_right - 1;
        int cb = w->abs_y + w->h - w->padding_bottom - 1;

        if (cl < old_cl) cl = old_cl;
        if (ct < old_ct) ct = old_ct;
        if (cr > old_cr) cr = old_cr;
        if (cb > old_cb) cb = old_cb;

        set_clip_rect(target, cl, ct, cr, cb);
    }

    for (int i = 0; i < w->child_count; i++) {
        widget_draw_recursive(w->children[i], target);
    }

    if (w->clip_children && w->type != WIDGET_WINDOW) {
        set_clip_rect(target, old_cl, old_ct, old_cr, old_cb);
    }
}

void widget_set_expand(Widget *w, int expand, int weight) {
    w->expand = expand;
    w->expand_weight = weight > 0 ? weight : 1;
    if (w->parent) {
        w->parent->needs_layout = 1;
        w->parent->dirty = 1;
    }
}

Widget *widget_find_parent_window(Widget *w) {
    Widget *p = w;
    while (p) {
        if (p->type == WIDGET_WINDOW) return p;
        p = p->parent;
    }
    return NULL;
}
