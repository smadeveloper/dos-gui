
#include "gui_core.h"
#include "gui_state.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_window.h"
#include "gui_tooltip.h"
#include "gui_combobox.h"
#include "gui_menu.h"
#include "gui_menubar.h"
#include <allegro.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================
 * INITIALIZATION / SHUTDOWN
 * ======================================================================== */
void gui_init(int width, int height) {
    allegro_init();
    install_keyboard();
    install_mouse();
    install_timer();
    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT, width, height, 0, 0) != 0) {
        allegro_message("Video mode failed: %s\n", allegro_error);
        exit(1);
    }
    show_mouse(screen);
    memset(&gui_state, 0, sizeof(gui_state));
    gui_state.screen_w = width;
    gui_state.screen_h = height;
    gui_state.running = 1;
    gui_state.needs_redraw = 1;
    gui_state.root = widget_create(WIDGET_BASE, 0, 0, width, height);
    gui_state.root->layout = LAYOUT_NONE;
    gui_state.root->clip_children = 0;
    theme_init_default();
    tooltip_reset();
    gui_state.buffer = create_bitmap(width, height);
    clear_to_color(gui_state.buffer, theme.desktop);
}

void gui_shutdown(void) {
    if (gui_state.root) {
        widget_destroy(gui_state.root);
        gui_state.root = NULL;
    }
    if (gui_state.buffer) {
        destroy_bitmap(gui_state.buffer);
        gui_state.buffer = NULL;
    }
    allegro_exit();
}

/* ========================================================================
 * FOCUS MANAGEMENT
 * ======================================================================== */
void gui_set_focus(Widget *w) {
    if (gui_state.focused_widget == w) return;
    if (gui_state.focused_widget) {
        gui_state.focused_widget->focused = 0;
        gui_state.focused_widget->dirty = 1;
        Event blur_ev;
        memset(&blur_ev, 0, sizeof(blur_ev));
        blur_ev.type = EVENT_BLUR;
        blur_ev.target = gui_state.focused_widget;
        widget_handle_event(gui_state.focused_widget, &blur_ev);
    }
    gui_state.focused_widget = w;
    if (w) {
        w->focused = 1;
        w->dirty = 1;
        Event focus_ev;
        memset(&focus_ev, 0, sizeof(focus_ev));
        focus_ev.type = EVENT_FOCUS;
        focus_ev.target = w;
        widget_handle_event(w, &focus_ev);
    }
}

/* ========================================================================
 * WINDOW Z-ORDER
 * ======================================================================== */
static void bring_window_to_front(Widget *win) {
    Widget *root = gui_state.root;
    if (!root || !win) return;
    int idx = -1;
    for (int i = 0; i < root->child_count; i++) {
        if (root->children[i] == win) { idx = i; break; }
    }
    if (idx < 0 || idx == root->child_count - 1) return;
    for (int i = idx; i < root->child_count - 1; i++) {
        root->children[i] = root->children[i + 1];
    }
    root->children[root->child_count - 1] = win;
    gui_state.needs_redraw = 1;
}

/* ========================================================================
 * HIT TESTING
 * ======================================================================== */
static Widget *hit_test_recursive(Widget *w, int mx, int my) {
    if (!w || !w->visible) return NULL;
    for (int i = w->child_count - 1; i >= 0; i--) {
        Widget *child = w->children[i];
        if (!child || !child->visible) continue;
        Widget *hit = hit_test_recursive(child, mx, my);
        if (hit) return hit;
    }
    if (point_in_rect(mx, my, w->abs_x, w->abs_y, w->w, w->h)) {
        return w;
    }
    return NULL;
}

static Widget *find_topmost_widget(int mx, int my) {
    Widget *root = gui_state.root;
    if (!root) return NULL;
    for (int i = root->child_count - 1; i >= 0; i--) {
        Widget *child = root->children[i];
        if (!child || !child->visible) continue;
        Widget *hit = hit_test_recursive(child, mx, my);
        if (hit) return hit;
    }
    return root;
}

/* ========================================================================
 * WINDOW TITLEBAR HIT TEST
 * ======================================================================== */
static int is_in_titlebar(Widget *win, int mx, int my) {
    if (!win || win->type != WIDGET_WINDOW) return 0;
    return (mx >= win->abs_x && mx < win->abs_x + win->w &&
            my >= win->abs_y && my < win->abs_y + TITLEBAR_HEIGHT);
}

/* ========================================================================
 * COMBOBOX DROPDOWN HANDLING
 * ======================================================================== */
static void close_all_combobox_dropdowns(void) {
    Widget *open_cb = find_open_combobox_recursive(gui_state.root);
    if (open_cb) {
        ComboboxData *cd = (ComboboxData *)open_cb->extra;
        cd->open = 0;
        open_cb->dirty = 1;
        gui_state.needs_redraw = 1;
    }
}

static int handle_combobox_dropdown_click(int mx, int my) {
    Widget *open_cb = find_open_combobox_recursive(gui_state.root);
    if (!open_cb) return 0;
    if (combobox_hit_dropdown(open_cb, mx, my)) {
        int idx = combobox_dropdown_item_at(open_cb, mx, my);
        ComboboxData *cd = (ComboboxData *)open_cb->extra;
        if (idx >= 0 && idx < cd->item_count) {
            cd->selected_index = idx;
            widget_emit_simple(open_cb, EVENT_CHANGE);
        }
        cd->open = 0;
        open_cb->dirty = 1;
        gui_state.needs_redraw = 1;
        return 1;
    }
    close_all_combobox_dropdowns();
    return 0;
}

static void handle_combobox_dropdown_hover(int mx, int my) {
    Widget *open_cb = find_open_combobox_recursive(gui_state.root);
    if (!open_cb) return;
    if (combobox_hit_dropdown(open_cb, mx, my)) {
        ComboboxData *cd = (ComboboxData *)open_cb->extra;
        int old_hover = cd->hover_index;
        cd->hover_index = combobox_dropdown_item_at(open_cb, mx, my);
        if (cd->hover_index != old_hover) {
            open_cb->dirty = 1;
            gui_state.needs_redraw = 1;
        }
    }
}

/* ========================================================================
 * MENUBAR / MENU HELPERS
 * ======================================================================== */

/* Find the menubar widget inside a window */
static Widget *find_menubar_in_window(Widget *win) {
    if (!win || win->type != WIDGET_WINDOW) return NULL;
    WindowData *wd = (WindowData *)win->extra;
    if (wd && wd->menubar) return wd->menubar;
    return NULL;
}

/* Find any open menubar across all windows */
static Widget *find_open_menubar(void) {
    for (int i = 0; i < gui_state.window_count; i++) {
        Widget *win = gui_state.windows[i];
        Widget *mb = find_menubar_in_window(win);
        if (mb) {
            MenubarData *mbd = (MenubarData *)mb->extra;
            if (mbd && mbd->menu_open) return mb;
        }
    }
    return NULL;
}

/* Close all menus on a menubar */
static void menubar_close_all(Widget *menubar) {
    if (!menubar) return;
    MenubarData *mbd = (MenubarData *)menubar->extra;
    if (!mbd) return;
    if (mbd->active_index >= 0 && mbd->active_index < menubar->child_count) {
        menu_close_all(menubar->children[mbd->active_index]);
    }
    mbd->menu_open = 0;
    mbd->active_index = -1;
    menubar->dirty = 1;
    gui_state.needs_redraw = 1;
}

/* Check if a point is inside any open menu popup (recursive) */
static int point_in_open_menus(Widget *menubar, int mx, int my) {
    MenubarData *mbd = (MenubarData *)menubar->extra;
    if (!mbd || !mbd->menu_open || mbd->active_index < 0) return 0;
    if (mbd->active_index >= menubar->child_count) return 0;
    Widget *menu = menubar->children[mbd->active_index];
    return menu_is_point_in_any(menu, mx, my);
}

/* Handle click on open menu popups, returns 1 if handled */
static int handle_menu_popup_click(int mx, int my) {
    Widget *menubar = find_open_menubar();
    if (!menubar) return 0;
    MenubarData *mbd = (MenubarData *)menubar->extra;
    if (!mbd || !mbd->menu_open || mbd->active_index < 0) return 0;
    if (mbd->active_index >= menubar->child_count) return 0;

    Widget *active_menu = menubar->children[mbd->active_index];
    Widget *hit_menu = NULL;
    int hit_index = -1;

    if (menu_hit_test_deep(active_menu, mx, my, &hit_menu, &hit_index)) {
        if (hit_menu && hit_index >= 0 && hit_index < hit_menu->child_count) {
            MenuItemData *mid = (MenuItemData *)hit_menu->children[hit_index]->extra;
            if (mid && mid->enabled && !mid->separator && !mid->has_submenu) {
                /* Fire the click event on the menu item */
                Event ev;
                memset(&ev, 0, sizeof(ev));
                ev.type = EVENT_CLICK;
                ev.target = hit_menu->children[hit_index];
                ev.mouse_x = mx;
                ev.mouse_y = my;
                widget_handle_event(hit_menu->children[hit_index], &ev);
                /* Close all menus */
                menubar_close_all(menubar);
                return 1;
            }
            /* If it has submenu, don't close, just return handled */
            if (mid && mid->has_submenu) return 1;
        }
        /* Clicked inside menu area but not on a valid item */
        return 1;
    }

    return 0;
}

/* Handle hover over open menu popups */
static void handle_menu_popup_hover(int mx, int my) {
    Widget *menubar = find_open_menubar();
    if (!menubar) return;
    MenubarData *mbd = (MenubarData *)menubar->extra;
    if (!mbd || !mbd->menu_open || mbd->active_index < 0) return;
    if (mbd->active_index >= menubar->child_count) return;

    Widget *active_menu = menubar->children[mbd->active_index];
    menu_update_hover_recursive(active_menu, mx, my);

    /* Also check if hovering over a different menubar item */
    int mb_idx = menubar_item_at(menubar, mx, my);
    if (mb_idx >= 0 && mb_idx != mbd->active_index) {
        /* Switch to different menu */
        menu_close_all(menubar->children[mbd->active_index]);
        mbd->active_index = mb_idx;
        Widget *new_menu = menubar->children[mb_idx];
        MenuData *md = (MenuData *)new_menu->extra;
        md->open = 1;
        md->popup_x = new_menu->abs_x;
        md->popup_y = menubar->abs_y + MENUBAR_HEIGHT;
        md->hover_index = -1;
        menu_calc_geometry(new_menu);
        menubar->dirty = 1;
        gui_state.needs_redraw = 1;
    }

    gui_state.needs_redraw = 1;
}

/* ========================================================================
 * EVENT PROCESSING
 * ======================================================================== */
static int prev_mouse_x = 0, prev_mouse_y = 0;
static int prev_mouse_b = 0;
static int prev_mouse_z = 0;

static void send_event(Widget *target, EventType type, int mx, int my, int mb) {
    if (!target) return;
    Event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.target = target;
    ev.mouse_x = mx;
    ev.mouse_y = my;
    ev.mouse_button = mb;
    widget_handle_event(target, &ev);
}

static void process_mouse_events(void) {
    int mx = mouse_x, my = mouse_y, mb = mouse_b, mz = mouse_z;

    /* Scroll wheel */
    if (mz != prev_mouse_z) {
        int delta = mz - prev_mouse_z;
        prev_mouse_z = mz;
        Widget *target = gui_state.captured_widget;
        if (!target) target = find_topmost_widget(mx, my);
        if (target) {
            Event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = EVENT_SCROLL;
            ev.target = target;
            ev.mouse_x = mx;
            ev.mouse_y = my;
            ev.delta = delta;
            widget_handle_event(target, &ev);
            gui_state.needs_redraw = 1;
        }
    }

    /* Mouse move */
    if (mx != prev_mouse_x || my != prev_mouse_y) {
        /* Window dragging */
        if (gui_state.dragging_window) {
            WindowData *wd = (WindowData *)gui_state.dragging_window->extra;
            if (wd) {
                gui_state.dragging_window->x = mx - wd->drag_offset_x;
                gui_state.dragging_window->y = my - wd->drag_offset_y;
                gui_state.dragging_window->needs_layout = 1;
                widget_mark_dirty(gui_state.dragging_window);
                gui_state.needs_redraw = 1;
            }
        }
        else if (gui_state.captured_widget) {
            send_event(gui_state.captured_widget, EVENT_MOUSE_MOVE, mx, my, mb);
            gui_state.needs_redraw = 1;
        }
        else {
            /* Handle menu popup hover */
            Widget *open_mb = find_open_menubar();
            if (open_mb) {
                handle_menu_popup_hover(mx, my);
            }

            Widget *new_hover = find_topmost_widget(mx, my);
            if (new_hover != gui_state.hovered_widget) {
                if (gui_state.hovered_widget) {
                    gui_state.hovered_widget->hovered = 0;
                    gui_state.hovered_widget->dirty = 1;
                    send_event(gui_state.hovered_widget, EVENT_MOUSE_LEAVE, mx, my, 0);
                }
                gui_state.hovered_widget = new_hover;
                if (new_hover) {
                    new_hover->hovered = 1;
                    new_hover->dirty = 1;
                    send_event(new_hover, EVENT_MOUSE_ENTER, mx, my, 0);
                }
            }
            if (new_hover) {
                send_event(new_hover, EVENT_MOUSE_MOVE, mx, my, mb);
            }
            handle_combobox_dropdown_hover(mx, my);
            gui_state.needs_redraw = 1;
        }
        prev_mouse_x = mx;
        prev_mouse_y = my;
    }

    /* Left mouse button press */
    if ((mb & 1) && !(prev_mouse_b & 1)) {
        /* First check if clicking on open menu popups */
        Widget *open_mb = find_open_menubar();
        if (open_mb) {
            /* Check if clicking inside menu popup */
            if (handle_menu_popup_click(mx, my)) {
                prev_mouse_b = mb;
                return;
            }
            /* Check if clicking on menubar itself */
            int mb_idx = menubar_item_at(open_mb, mx, my);
            if (mb_idx >= 0) {
                /* Let the menubar click handler deal with it */
                Event ev;
                memset(&ev, 0, sizeof(ev));
                ev.type = EVENT_CLICK;
                ev.target = open_mb;
                ev.mouse_x = mx;
                ev.mouse_y = my;
                ev.mouse_button = 1;
                widget_handle_event(open_mb, &ev);
                prev_mouse_b = mb;
                gui_state.needs_redraw = 1;
                return;
            }
            /* Clicking outside - close menus */
            menubar_close_all(open_mb);
        }

        if (handle_combobox_dropdown_click(mx, my)) {
            prev_mouse_b = mb;
            return;
        }

        Widget *target = find_topmost_widget(mx, my);
        if (target) {
            Widget *win = widget_find_parent_window(target);
            if (!win && target->type == WIDGET_WINDOW) win = target;
            if (win) {
                bring_window_to_front(win);
                set_active_window(win);
            }

            if (win && is_in_titlebar(win, mx, my) && target == win) {
                WindowData *wd = (WindowData *)win->extra;
                if (wd && !wd->maximized) {
                    gui_state.dragging_window = win;
                    wd->dragging = 1;
                    wd->drag_offset_x = mx - win->x;
                    wd->drag_offset_y = my - win->y;
                }
            }
            else if (target->type == WIDGET_MENUBAR) {
                /* Menubar click */
                Event ev;
                memset(&ev, 0, sizeof(ev));
                ev.type = EVENT_CLICK;
                ev.target = target;
                ev.mouse_x = mx;
                ev.mouse_y = my;
                ev.mouse_button = 1;
                widget_handle_event(target, &ev);
            }
            else if (target->type == WIDGET_COMBOBOX) {
                ComboboxData *cd = (ComboboxData *)target->extra;
                cd->open = !cd->open;
                cd->hover_index = cd->selected_index;
                target->dirty = 1;
                gui_state.needs_redraw = 1;
                gui_set_focus(target);
            }
            else {
                gui_set_focus(target);
                gui_state.captured_widget = target;
                target->pressed = 1;
                target->dirty = 1;
                send_event(target, EVENT_MOUSE_DOWN, mx, my, 1);
            }
        }
        gui_state.needs_redraw = 1;
    }

    /* Right mouse button press */
    if ((mb & 2) && !(prev_mouse_b & 2)) {
        /* Close menus on right click */
        Widget *open_mb = find_open_menubar();
        if (open_mb) menubar_close_all(open_mb);

        close_all_combobox_dropdowns();
        Widget *target = find_topmost_widget(mx, my);
        if (target) {
            send_event(target, EVENT_MOUSE_DOWN, mx, my, 2);
        }
        gui_state.needs_redraw = 1;
    }

    /* Left mouse button release */
    if (!(mb & 1) && (prev_mouse_b & 1)) {
        if (gui_state.dragging_window) {
            WindowData *wd = (WindowData *)gui_state.dragging_window->extra;
            if (wd) wd->dragging = 0;
            gui_state.dragging_window = NULL;
            gui_state.needs_redraw = 1;
        }
        else if (gui_state.captured_widget) {
            Widget *captured = gui_state.captured_widget;
            gui_state.captured_widget = NULL;
            captured->pressed = 0;
            captured->dirty = 1;
            send_event(captured, EVENT_MOUSE_UP, mx, my, 1);

            if (point_in_rect(mx, my, captured->abs_x, captured->abs_y, captured->w, captured->h)) {
                if (captured->type == WIDGET_CHECKBOX) {
                    CheckboxData *cd = (CheckboxData *)captured->extra;
                    if (cd) {
                        cd->checked = !cd->checked;
                        captured->dirty = 1;
                        widget_emit_simple(captured, EVENT_CHANGE);
                    }
                }
                else if (captured->type == WIDGET_RADIO) {
                    RadioData *rd = (RadioData *)captured->extra;
                    if (rd && !rd->selected) {
                        Widget *parent = captured->parent;
                        if (parent) {
                            for (int i = 0; i < parent->child_count; i++) {
                                Widget *sib = parent->children[i];
                                if (sib && sib->type == WIDGET_RADIO && sib != captured) {
                                    RadioData *srd = (RadioData *)sib->extra;
                                    if (srd && srd->group_id == rd->group_id && srd->selected) {
                                        srd->selected = 0;
                                        sib->dirty = 1;
                                    }
                                }
                            }
                        }
                        rd->selected = 1;
                        captured->dirty = 1;
                        widget_emit_simple(captured, EVENT_CHANGE);
                    }
                }
                else if (captured->type == WIDGET_BUTTON || captured->type == WIDGET_IMAGE_BUTTON) {
                    ButtonData *bd = (ButtonData *)captured->extra;
                    if (bd && bd->is_toggle) {
                        bd->toggled = !bd->toggled;
                    }
                    send_event(captured, EVENT_CLICK, mx, my, 1);
                }
                else {
                    send_event(captured, EVENT_CLICK, mx, my, 1);
                }
            }
            gui_state.needs_redraw = 1;
        }
        else {
            Widget *target = find_topmost_widget(mx, my);
            if (target) {
                send_event(target, EVENT_MOUSE_UP, mx, my, 1);
            }
            gui_state.needs_redraw = 1;
        }
    }

    /* Right mouse release */
    if (!(mb & 2) && (prev_mouse_b & 2)) {
        Widget *target = find_topmost_widget(mx, my);
        if (target) {
            send_event(target, EVENT_MOUSE_UP, mx, my, 2);
        }
        gui_state.needs_redraw = 1;
    }

    prev_mouse_b = mb;
}

static void process_keyboard_events(void) {
    while (keypressed()) {
        int raw = readkey();
        int scancode = raw >> 8;
        int ascii = raw & 0xFF;

        if (scancode == KEY_ESC && (key_shifts & KB_SHIFT_FLAG)) {
            gui_state.running = 0;
            return;
        }

        /* Close menus on Escape */
        if (scancode == KEY_ESC) {
            Widget *open_mb = find_open_menubar();
            if (open_mb) {
                menubar_close_all(open_mb);
                gui_state.needs_redraw = 1;
                continue;
            }
        }

        if (gui_state.focused_widget) {
            Event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = EVENT_KEY_DOWN;
            ev.target = gui_state.focused_widget;
            ev.key = scancode;
            ev.key_char = ascii;
            ev.key_modifiers = key_shifts;
            widget_handle_event(gui_state.focused_widget, &ev);
            gui_state.needs_redraw = 1;
        }
    }
}

/* ========================================================================
 * UPDATE (TICK)
 * ======================================================================== */
static void gui_update_recursive(Widget *w) {
    if (!w || !w->visible) return;
    if (w->update) w->update(w);
    for (int i = 0; i < w->child_count; i++) {
        gui_update_recursive(w->children[i]);
    }
}

static void gui_update(void) {
    gui_update_recursive(gui_state.root);
    tooltip_update(mouse_x, mouse_y);
}

/* ========================================================================
 * LAYOUT
 * ======================================================================== */
static void gui_layout_recursive(Widget *w) {
    if (!w || !w->visible) return;
    if (w->parent) {
        w->abs_x = w->parent->abs_x + w->x;
        w->abs_y = w->parent->abs_y + w->y;
    } else {
        w->abs_x = w->x;
        w->abs_y = w->y;
    }
    if (w->do_layout) {
        w->do_layout(w);
    }
    if (w->layout != LAYOUT_NONE && w->child_count > 0) {
        widget_do_layout(w);
    }
    w->needs_layout = 0;
    for (int i = 0; i < w->child_count; i++) {
        gui_layout_recursive(w->children[i]);
    }
}

/* ========================================================================
 * RENDERING
 * ======================================================================== */
static void gui_render_recursive(Widget *w, BITMAP *buf) {
    if (!w || !w->visible) return;
    int old_cl = 0, old_ct = 0, old_cr = buf->w - 1, old_cb = buf->h - 1;
    if (w->clip_children) {
        old_cl = buf->cl; old_ct = buf->ct;
        old_cr = buf->cr; old_cb = buf->cb;
        set_clip_rect(buf, w->abs_x, w->abs_y,
                      w->abs_x + w->w - 1, w->abs_y + w->h - 1);
    }
    if (w->draw) {
        w->draw(w, buf);
    }
    for (int i = 0; i < w->child_count; i++) {
        gui_render_recursive(w->children[i], buf);
    }
    if (w->clip_children) {
        set_clip_rect(buf, old_cl, old_ct, old_cr, old_cb);
    }
}

static void gui_render(void) {
    BITMAP *buf = gui_state.buffer;
    clear_to_color(buf, theme.desktop);
    gui_render_recursive(gui_state.root, buf);

    /* Draw combobox dropdowns */
    Widget *open_cb = find_open_combobox_recursive(gui_state.root);
    if (open_cb) {
        combobox_draw_dropdown(open_cb, buf);
    }

    /* Draw menu popups (overlay - drawn after everything else) */
    for (int i = 0; i < gui_state.window_count; i++) {
        Widget *win = gui_state.windows[i];
        Widget *mb = find_menubar_in_window(win);
        if (mb) {
            menubar_draw_menus(mb, buf);
        }
    }

    tooltip_draw(buf);
    scare_mouse();
    blit(buf, screen, 0, 0, 0, 0, buf->w, buf->h);
    unscare_mouse();
}

/* ========================================================================
 * MAIN LOOP
 * ======================================================================== */
void gui_main_loop(void) {
    prev_mouse_x = mouse_x;
    prev_mouse_y = mouse_y;
    prev_mouse_b = mouse_b;
    prev_mouse_z = mouse_z;
    while (gui_state.running) {
        process_mouse_events();
        process_keyboard_events();
        gui_update();
        gui_layout_recursive(gui_state.root);
        gui_render();
        rest(16);
    }
}