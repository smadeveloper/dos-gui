#ifndef GUI_STATE_H
#define GUI_STATE_H

#include "gui_types.h"

/* ========================================================================
 * GLOBAL STATE
 * ======================================================================== */
typedef struct {
    /* Screen / double buffer */
    BITMAP *screen_buffer;
    BITMAP *buffer;
    int screen_w, screen_h;

    /* Root widget (contains all windows as children) */
    Widget *root;

    /* Window management */
    Widget *windows[MAX_WINDOWS];
    int window_count;
    int next_z_index;

    /* Active window */
    Widget *active_window;

    /* Input state */
    Widget *focused_widget;
    Widget *hovered_widget;
    Widget *captured_widget;
    Widget *dragging_window;

    /* Overlays */
    struct {
        DrawFunc draw;
        Widget *widget;
    } overlays[16];
    int overlay_count;

    /* Mouse previous state */
    int mouse_prev_x, mouse_prev_y;
    int mouse_prev_b;

    /* Double-click detection */
    int double_click_time;
    int last_click_time;
    int last_click_x, last_click_y;

    /* Flags */
    int running;
    int needs_redraw;

    unsigned int next_id;
    volatile int tick_counter;
} GuiState;

extern GuiState gui_state;
extern volatile int gui_tick;

void gui_timer_handler(void);

#endif /* GUI_STATE_H */
