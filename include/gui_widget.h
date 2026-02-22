#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include "gui_types.h"

/* Widget core */
void widget_init(Widget *w, WidgetType type, int x, int y, int ww, int hh);
Widget *widget_create(WidgetType type, int x, int y, int w, int h);

/* Event system */
void widget_on(Widget *w, EventType type, EventCallback cb);
void widget_emit(Widget *w, Event *event);
void widget_emit_simple(Widget *w, EventType type);
void widget_handle_event(Widget *w, Event *event);

/* Child management */
int  widget_add_child(Widget *parent, Widget *child);
void widget_remove_child(Widget *parent, Widget *child);

/* Destruction */
void widget_destroy(Widget *w);

/* Geometry helpers */
void widget_set_padding(Widget *w, int top, int right, int bottom, int left);
void widget_set_margin(Widget *w, int top, int right, int bottom, int left);
void widget_set_padding_all(Widget *w, int p);
void widget_set_margin_all(Widget *w, int m);

/* Absolute position computation */
void widget_compute_absolute(Widget *w);

/* Layout engine */
void widget_do_layout(Widget *w);

/* Dirty marking */
void widget_mark_dirty(Widget *w);

/* Hit test */
int widget_default_hit_test(Widget *w, int mx, int my);
Widget *widget_hit_test_recursive(Widget *w, int mx, int my);

/* Focus */
void widget_set_focus(Widget *w);

/* Drawing */
void widget_draw_recursive(Widget *w, BITMAP *target);

/* Expand helper */
void widget_set_expand(Widget *w, int expand, int weight);

/* Find parent window */
Widget *widget_find_parent_window(Widget *w);

#endif /* GUI_WIDGET_H */
