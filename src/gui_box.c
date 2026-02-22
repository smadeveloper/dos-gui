#include "gui_box.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>

void box_calc_content_size(Widget *self, int *out_w, int *out_h) {
    int is_horizontal = (self->type == WIDGET_HBOX);
    int main_total = 0;
    int cross_max = 0;
    int visible_count = 0;

    for (int i = 0; i < self->child_count; i++) {
        Widget *c = self->children[i];
        if (!c->visible) continue;
        int cw = c->w;
        int ch = c->h;
        if (c->type == WIDGET_HBOX || c->type == WIDGET_VBOX) {
            int inner_w, inner_h;
            box_calc_content_size(c, &inner_w, &inner_h);
            if (cw < inner_w) cw = inner_w;
            if (ch < inner_h) ch = inner_h;
        }
        if (cw < c->min_w) cw = c->min_w;
        if (ch < c->min_h) ch = c->min_h;
        int c_main, c_cross;
        int c_main_margin, c_cross_margin;
        if (is_horizontal) {
            c_main = cw; c_cross = ch;
            c_main_margin = c->margin_left + c->margin_right;
            c_cross_margin = c->margin_top + c->margin_bottom;
        } else {
            c_main = ch; c_cross = cw;
            c_main_margin = c->margin_top + c->margin_bottom;
            c_cross_margin = c->margin_left + c->margin_right;
        }
        main_total += c_main + c_main_margin;
        int cross_with_margin = c_cross + c_cross_margin;
        if (cross_with_margin > cross_max) cross_max = cross_with_margin;
        visible_count++;
    }
    if (visible_count > 1) main_total += (visible_count - 1) * self->spacing;
    if (is_horizontal) {
        *out_w = main_total + self->padding_left + self->padding_right;
        *out_h = cross_max + self->padding_top + self->padding_bottom;
    } else {
        *out_w = cross_max + self->padding_left + self->padding_right;
        *out_h = main_total + self->padding_top + self->padding_bottom;
    }
}

void box_do_layout(Widget *self) {
    int is_horizontal = (self->type == WIDGET_HBOX);

    {
        int content_w, content_h;
        box_calc_content_size(self, &content_w, &content_h);
        if (is_horizontal) {
            if (!(self->fill & FILL_X) && !self->expand && self->w < content_w) self->w = content_w;
            if (!(self->fill & FILL_Y) && self->h < content_h) self->h = content_h;
        } else {
            if (!(self->fill & FILL_Y) && !self->expand && self->h < content_h) self->h = content_h;
            if (!(self->fill & FILL_X) && self->w < content_w) self->w = content_w;
        }
        if (is_horizontal) { if (self->h <= 0) self->h = content_h; }
        else { if (self->w <= 0) self->w = content_w; }
        if (is_horizontal) { if (self->w <= 0) self->w = content_w; }
        else { if (self->h <= 0) self->h = content_h; }
    }

    int avail_main, avail_cross;
    int pad_cross_start, pad_cross_end;
    if (is_horizontal) {
        avail_main = self->w - self->padding_left - self->padding_right;
        avail_cross = self->h - self->padding_top - self->padding_bottom;
        pad_cross_start = self->padding_top;
        pad_cross_end = self->padding_bottom;
    } else {
        avail_main = self->h - self->padding_top - self->padding_bottom;
        avail_cross = self->w - self->padding_left - self->padding_right;
        pad_cross_start = self->padding_left;
        pad_cross_end = self->padding_right;
    }

    int fixed_total = 0;
    int expand_weight_total = 0;
    int visible_count = 0;
    for (int i = 0; i < self->child_count; i++) {
        Widget *c = self->children[i];
        if (!c->visible) continue;
        if ((c->type == WIDGET_HBOX || c->type == WIDGET_VBOX)) {
            int cw, ch;
            box_calc_content_size(c, &cw, &ch);
            if (c->w < cw) c->w = cw;
            if (c->h < ch) c->h = ch;
        }
        int c_main_margin, c_main_size;
        if (is_horizontal) {
            c_main_margin = c->margin_left + c->margin_right;
            c_main_size = c->w;
        } else {
            c_main_margin = c->margin_top + c->margin_bottom;
            c_main_size = c->h;
        }
        if (c->expand) {
            int weight = c->expand_weight > 0 ? c->expand_weight : 1;
            expand_weight_total += weight;
            fixed_total += c_main_margin;
        } else {
            fixed_total += c_main_size + c_main_margin;
        }
        visible_count++;
    }

    int spacing_total = 0;
    if (visible_count > 1) spacing_total = (visible_count - 1) * self->spacing;
    int remaining = avail_main - fixed_total - spacing_total;
    if (remaining < 0) remaining = 0;

    int pos = is_horizontal ? self->padding_left : self->padding_top;
    for (int i = 0; i < self->child_count; i++) {
        Widget *c = self->children[i];
        if (!c->visible) continue;
        int c_main_size, c_cross_size;
        int c_main_margin_before, c_main_margin_after;
        int c_cross_margin_before, c_cross_margin_after;
        if (is_horizontal) {
            c_main_size = c->w; c_cross_size = c->h;
            c_main_margin_before = c->margin_left; c_main_margin_after = c->margin_right;
            c_cross_margin_before = c->margin_top; c_cross_margin_after = c->margin_bottom;
        } else {
            c_main_size = c->h; c_cross_size = c->w;
            c_main_margin_before = c->margin_top; c_main_margin_after = c->margin_bottom;
            c_cross_margin_before = c->margin_left; c_cross_margin_after = c->margin_right;
        }
        if (c->expand && expand_weight_total > 0) {
            int weight = c->expand_weight > 0 ? c->expand_weight : 1;
            c_main_size = remaining * weight / expand_weight_total;
        }
        if (is_horizontal) {
            if (c->fill & FILL_Y) {
                c_cross_size = avail_cross - c_cross_margin_before - c_cross_margin_after;
                if (c_cross_size < 0) c_cross_size = 0;
            }
        } else {
            if (c->fill & FILL_X) {
                c_cross_size = avail_cross - c_cross_margin_before - c_cross_margin_after;
                if (c_cross_size < 0) c_cross_size = 0;
            }
        }
        int cross_pos = pad_cross_start + c_cross_margin_before;
        Alignment cross_align = is_horizontal ? c->align_v : c->align_h;
        if (cross_align == ALIGN_CENTER) {
            cross_pos = pad_cross_start + (avail_cross - c_cross_size) / 2;
        } else if ((is_horizontal && cross_align == ALIGN_BOTTOM) ||
                   (!is_horizontal && cross_align == ALIGN_RIGHT)) {
            cross_pos = pad_cross_start + avail_cross - c_cross_size - c_cross_margin_after;
        }
        if (is_horizontal) {
            c->x = pos + c_main_margin_before; c->y = cross_pos;
            c->w = c_main_size; c->h = c_cross_size;
        } else {
            c->y = pos + c_main_margin_before; c->x = cross_pos;
            c->h = c_main_size; c->w = c_cross_size;
        }
        if (c->w < c->min_w) c->w = c->min_w;
        if (c->h < c->min_h) c->h = c->min_h;
        if (c->max_w < 9999 && c->w > c->max_w) c->w = c->max_w;
        if (c->max_h < 9999 && c->h > c->max_h) c->h = c->max_h;
        pos += c_main_margin_before + c_main_size + c_main_margin_after + self->spacing;
        c->needs_layout = 1;
    }
    if (is_horizontal) {
        self->content_w = pos - self->spacing + self->padding_right;
        self->content_h = self->h;
    } else {
        self->content_h = pos - self->spacing + self->padding_bottom;
        self->content_w = self->w;
    }
}

void box_draw(Widget *self, BITMAP *target) {
    (void)self; (void)target;
}

void box_draw_bg(Widget *self, BITMAP *target) {
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.bg);
}

Widget *hbox_create(int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_HBOX, x, y, w, h);
    wid->layout = LAYOUT_HORIZONTAL;
    wid->spacing = 4;
    wid->draw = box_draw;
    wid->do_layout = box_do_layout;
    wid->clip_children = 1;
    return wid;
}

Widget *vbox_create(int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_VBOX, x, y, w, h);
    wid->layout = LAYOUT_VERTICAL;
    wid->spacing = 4;
    wid->draw = box_draw;
    wid->do_layout = box_do_layout;
    wid->clip_children = 1;
    return wid;
}

Widget *hbox_create_simple(int spacing) {
    Widget *box = hbox_create(0, 0, 0, 0);
    box->spacing = spacing;
    box->fill = FILL_X;
    return box;
}

Widget *vbox_create_simple(int spacing) {
    Widget *box = vbox_create(0, 0, 0, 0);
    box->spacing = spacing;
    box->fill = FILL_X;
    return box;
}
