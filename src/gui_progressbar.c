#include "gui_progressbar.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void progressbar_draw(Widget *self, BITMAP *target) {
    ProgressBarData *data = (ProgressBarData *)self->extra;
    int range = data->max_value - data->min_value;
    if (range <= 0) range = 1;

    int bg_col = data->bar_bg_color ? data->bar_bg_color : theme.scrollbar_bg;
    int bar_col = data->bar_color ? data->bar_color : theme.titlebar_active;
    int txt_col = data->text_color ? data->text_color : makecol(255, 255, 255);

    if (!self->enabled) {
        bar_col = theme.disabled_fg;
    }

    /* Background */
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, bg_col);
    rect(target, self->abs_x, self->abs_y,
         self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.border);

    int val = data->value - data->min_value;
    if (val < 0) val = 0;
    if (val > range) val = range;

    if (data->horizontal) {
        int fill_w = val * (self->w - 2) / range;
        if (fill_w > 0) {
            rectfill(target, self->abs_x + 1, self->abs_y + 1,
                     self->abs_x + fill_w, self->abs_y + self->h - 2, bar_col);

            /* Striped style */
            if (data->style == 1 && fill_w > 4) {
                int stripe_offset = data->animated ? data->anim_offset : 0;
                for (int sx = self->abs_x + 1 + stripe_offset; sx < self->abs_x + 1 + fill_w; sx += 8) {
                    int x1 = sx;
                    int x2 = sx + 3;
                    if (x2 > self->abs_x + fill_w) x2 = self->abs_x + fill_w;
                    if (x1 >= self->abs_x + 1 && x1 < self->abs_x + 1 + fill_w) {
                        rectfill(target, x1, self->abs_y + 1, x2, self->abs_y + self->h - 2,
                                 makecol(MIN(getr(bar_col) + 30, 255),
                                         MIN(getg(bar_col) + 30, 255),
                                         MIN(getb(bar_col) + 30, 255)));
                    }
                }
            }
        }

        /* Text */
        if (data->show_text) {
            char buf[16];
            int pct = val * 100 / range;
            sprintf(buf, "%d%%", pct);
            int tw = text_length(font, buf);
            int tx = self->abs_x + (self->w - tw) / 2;
            int ty = self->abs_y + (self->h - text_height(font)) / 2;

            /* Draw text with contrast */
            if (fill_w > tx - self->abs_x + tw / 2) {
                textout_ex(target, font, buf, tx, ty, txt_col, -1);
            } else {
                textout_ex(target, font, buf, tx, ty, theme.fg, -1);
            }
        }
    } else {
        /* Vertical */
        int fill_h = val * (self->h - 2) / range;
        if (fill_h > 0) {
            rectfill(target, self->abs_x + 1, self->abs_y + self->h - 1 - fill_h,
                     self->abs_x + self->w - 2, self->abs_y + self->h - 2, bar_col);
        }

        if (data->show_text) {
            char buf[16];
            int pct = val * 100 / range;
            sprintf(buf, "%d%%", pct);
            textout_centre_ex(target, font, buf,
                              self->abs_x + self->w / 2,
                              self->abs_y + (self->h - text_height(font)) / 2,
                              theme.fg, -1);
        }
    }
}

static void progressbar_update(Widget *self) {
    ProgressBarData *data = (ProgressBarData *)self->extra;
    if (data->animated && data->style == 1) {
        data->anim_offset = (data->anim_offset + 1) % 8;
        self->dirty = 1;
    }
}

Widget *progressbar_create(int horizontal, int x, int y, int length) {
    int w, h;
    if (horizontal) {
        w = length; h = PROGRESSBAR_HEIGHT;
    } else {
        w = PROGRESSBAR_HEIGHT; h = length;
    }
    Widget *wid = widget_create(WIDGET_BASE, x, y, w, h);
    ProgressBarData *data = (ProgressBarData *)calloc(1, sizeof(ProgressBarData));
    data->horizontal = horizontal;
    data->min_value = 0;
    data->max_value = 100;
    data->value = 0;
    data->show_text = 1;
    wid->extra = data;
    wid->draw = progressbar_draw;
    wid->update = progressbar_update;
    return wid;
}

void progressbar_set_value(Widget *w, int value) {
    ProgressBarData *d = (ProgressBarData *)w->extra;
    d->value = clamp_int(value, d->min_value, d->max_value);
    w->dirty = 1;
}

int progressbar_get_value(Widget *w) {
    return ((ProgressBarData *)w->extra)->value;
}

void progressbar_set_range(Widget *w, int min_val, int max_val) {
    ProgressBarData *d = (ProgressBarData *)w->extra;
    d->min_value = min_val;
    d->max_value = max_val;
    if (d->value < min_val) d->value = min_val;
    if (d->value > max_val) d->value = max_val;
    w->dirty = 1;
}

void progressbar_set_colors(Widget *w, int bar_col, int bg_col, int text_col) {
    ProgressBarData *d = (ProgressBarData *)w->extra;
    d->bar_color = bar_col;
    d->bar_bg_color = bg_col;
    d->text_color = text_col;
    w->dirty = 1;
}

void progressbar_set_style(Widget *w, int style) {
    ProgressBarData *d = (ProgressBarData *)w->extra;
    d->style = style;
    d->animated = (style == 1);
    w->dirty = 1;
}

void progressbar_set_show_text(Widget *w, int show) {
    ((ProgressBarData *)w->extra)->show_text = show;
    w->dirty = 1;
}
