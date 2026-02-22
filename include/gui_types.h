#ifndef GUI_TYPES_H
#define GUI_TYPES_H

#include <allegro.h>

/* ========================================================================
 * CONSTANTS
 * ======================================================================== */
#define MAX_CHILDREN    128
#define MAX_WINDOWS     64
#define MAX_TEXT_LEN    256
#define MAX_MENU_ITEMS  32
#define MAX_TAB_PAGES   16
#define MAX_COMBO_ITEMS 64

#define TITLEBAR_HEIGHT   24
#define SCROLLBAR_SIZE    16
#define MENUBAR_HEIGHT    22
#define STATUSBAR_HEIGHT  22
#define TAB_HEIGHT        26
#define BUTTON_HEIGHT     26
#define INPUT_HEIGHT      24
#define CHECKBOX_SIZE     16
#define RADIO_SIZE        16
#define SPINBOX_BTN_W     18

/* ========================================================================
 * THEME COLORS
 * ======================================================================== */
typedef struct {
    int bg, fg, border, titlebar_active, titlebar_inactive;
    int button_face, button_highlight, button_shadow, button_text;
    int input_bg, input_fg, input_border, input_focus_border;
    int menu_bg, menu_fg, menu_highlight, menu_highlight_fg;
    int checkbox_bg, checkbox_check;
    int scrollbar_bg, scrollbar_thumb, scrollbar_arrow;
    int tab_active, tab_inactive, tab_border;
    int statusbar_bg, statusbar_fg;
    int tooltip_bg, tooltip_fg, tooltip_border;
    int selection_bg, selection_fg;
    int disabled_fg;
    int desktop;
} ThemeColors;

extern ThemeColors theme;

/* ========================================================================
 * WIDGET TYPE ENUM
 * ======================================================================== */
typedef enum {
    WIDGET_BASE = 0,
    WIDGET_WINDOW,
    WIDGET_BUTTON,
    WIDGET_IMAGE_BUTTON,
    WIDGET_LABEL,
    WIDGET_INPUT,
    WIDGET_CHECKBOX,
    WIDGET_RADIO,
    WIDGET_COMBOBOX,
    WIDGET_MENUBAR,
    WIDGET_MENU,
    WIDGET_MENU_ITEM,
    WIDGET_STATUSBAR,
    WIDGET_TAB,
    WIDGET_TAB_PAGE,
    WIDGET_SCROLLBAR,
    WIDGET_PANEL,
    WIDGET_SEPARATOR,
    WIDGET_SLIDER,
    WIDGET_SPINBOX,
    WIDGET_TYPE_COUNT,
    WIDGET_HBOX,
    WIDGET_VBOX
} WidgetType;

/* ========================================================================
 * ALIGNMENT, FILL, LAYOUT, EVENT ENUMS
 * ======================================================================== */
typedef enum {
    ALIGN_NONE = 0,
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
    ALIGN_TOP,
    ALIGN_BOTTOM
} Alignment;

typedef enum {
    FILL_NONE = 0,
    FILL_X    = 1,
    FILL_Y    = 2,
    FILL_BOTH = 3
} FillMode;

typedef enum {
    LAYOUT_NONE = 0,
    LAYOUT_VERTICAL,
    LAYOUT_HORIZONTAL
} LayoutMode;

typedef enum {
    EVENT_NONE = 0,
    EVENT_CLICK,
    EVENT_DOUBLE_CLICK,
    EVENT_MOUSE_DOWN,
    EVENT_MOUSE_UP,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_ENTER,
    EVENT_MOUSE_LEAVE,
    EVENT_KEY_DOWN,
    EVENT_KEY_UP,
    EVENT_FOCUS,
    EVENT_BLUR,
    EVENT_CHANGE,
    EVENT_SCROLL,
    EVENT_RESIZE,
    EVENT_CLOSE,
    EVENT_TAB_CHANGE,
    EVENT_MENU_SELECT,
    EVENT_TIMER,
    EVENT_TYPE_COUNT
} EventType;

/* ========================================================================
 * FORWARD DECLARATIONS
 * ======================================================================== */
typedef struct Widget Widget;

/* ========================================================================
 * EVENT STRUCTURE
 * ======================================================================== */
typedef struct {
    EventType type;
    Widget *target;
    int mouse_x, mouse_y;
    int mouse_button;
    int key;
    int key_char;
    int key_modifiers;
    int delta;
    void *data;
    int handled;
} Event;

/* ========================================================================
 * CALLBACK TYPES
 * ======================================================================== */
typedef void (*EventCallback)(Widget *self, Event *event);
typedef void (*DrawFunc)(Widget *self, BITMAP *target);
typedef void (*UpdateFunc)(Widget *self);
typedef void (*DestroyFunc)(Widget *self);
typedef void (*LayoutFunc)(Widget *self);
typedef int  (*HitTestFunc)(Widget *self, int mx, int my);

/* ========================================================================
 * EVENT HANDLER LIST
 * ======================================================================== */
typedef struct EventHandler {
    EventType type;
    EventCallback callback;
    struct EventHandler *next;
} EventHandler;

/* ========================================================================
 * WIDGET STRUCTURE (BASE)
 * ======================================================================== */
struct Widget {
    WidgetType type;
    char name[64];
    unsigned int id;

    int x, y, w, h;
    int abs_x, abs_y;
    int min_w, min_h;
    int max_w, max_h;

    int margin_top, margin_right, margin_bottom, margin_left;
    int padding_top, padding_right, padding_bottom, padding_left;

    Alignment align_h, align_v;
    FillMode fill;
    int expand;
    int expand_weight;

    LayoutMode layout;
    int spacing;

    int visible;
    int enabled;
    int focused;
    int hovered;
    int pressed;
    int dirty;
    int needs_layout;

    BITMAP *bitmap;
    int clip_children;

    Widget *parent;
    Widget *children[MAX_CHILDREN];
    int child_count;

    int z_order;
    int scroll_x, scroll_y;
    int content_w, content_h;

    EventHandler *handlers;

    DrawFunc    draw;
    UpdateFunc  update;
    DestroyFunc destroy;
    LayoutFunc  do_layout;
    HitTestFunc hit_test;

    void *extra;
    void *user_data;
};

/* ========================================================================
 * TYPE-SPECIFIC DATA STRUCTURES
 * ======================================================================== */

/* Window */
typedef struct {
    char title[MAX_TEXT_LEN];
    int z_index;
    int dragging;
    int drag_offset_x, drag_offset_y;
    int maximized;
    int old_x, old_y, old_w, old_h;
    int resizable;
    int closable;
    int maximizable;
    int active;
    Widget *btn_close;
    Widget *btn_maximize;
    Widget *titlebar_area;
    Widget *client_area;
    Widget *menubar;
    Widget *statusbar;
} WindowData;

/* Button */
typedef struct {
    char text[MAX_TEXT_LEN];
    BITMAP *icon;
    BITMAP *icon_hover;
    BITMAP *icon_pressed;
    int is_toggle;
    int toggled;
} ButtonData;

/* Label */
typedef struct {
    char text[MAX_TEXT_LEN];
    int text_color;
    Alignment text_align;
    int bold;
} LabelData;

/* Input */
typedef struct {
    char text[MAX_TEXT_LEN];
    char placeholder[MAX_TEXT_LEN];
    int cursor_pos;
    int selection_start, selection_end;
    int scroll_offset;
    int max_length;
    int password_mode;
    int readonly;
    int cursor_visible;
    int cursor_timer;
} InputData;

/* Checkbox */
typedef struct {
    char text[MAX_TEXT_LEN];
    int checked;
    int tristate;
} CheckboxData;

/* Radio */
typedef struct {
    char text[MAX_TEXT_LEN];
    int selected;
    int group_id;
} RadioData;

/* Combobox */
typedef struct {
    char items[MAX_COMBO_ITEMS][MAX_TEXT_LEN];
    int item_count;
    int selected_index;
    int open;
    int dropdown_height;
    int dropdown_y;
    int hover_index;
    int scroll_offset;
    BITMAP *dropdown_bitmap;
} ComboboxData;

/* Menu Item */
typedef struct {
    char text[MAX_TEXT_LEN];
    char shortcut[32];
    int separator;
    int has_submenu;
    Widget *submenu;
    BITMAP *icon;
    int checked;
    int enabled;
} MenuItemData;

/* Menu (popup/dropdown) */
typedef struct {
    int open;
    int hover_index;
    int popup_x, popup_y;
    int popup_w, popup_h;
    BITMAP *popup_bitmap;
} MenuData;

/* Menubar */
typedef struct {
    int active_index;
    int menu_open;
} MenubarData;

/* Statusbar */
typedef struct {
    char text[MAX_TEXT_LEN];
    char sections[8][MAX_TEXT_LEN];
    int section_widths[8];
    int section_count;
} StatusbarData;

/* Tab */
typedef struct {
    int active_tab;
    char tab_titles[MAX_TAB_PAGES][MAX_TEXT_LEN];
    Widget *pages[MAX_TAB_PAGES];
    int tab_count;
} TabData;

/* Scrollbar */
typedef struct {
    int horizontal;
    int value;
    int min_value, max_value;
    int page_size;
    int thumb_size;
    int thumb_pos;
    int dragging_thumb;
    int drag_offset;
} ScrollbarData;

/* Slider */
typedef struct {
    int horizontal;
    int value;
    int min_value;
    int max_value;
    int step;
    int dragging;
    int show_value;
    int tick_interval;
    int track_color;
    int fill_color;
    int thumb_color;
} SliderData;

/* Spinbox */
typedef struct {
    int value;
    int min_value;
    int max_value;
    int step;
    int decimal_places;
    float float_value;
    float float_min;
    float float_max;
    float float_step;
    int use_float;
    int btn_up_pressed;
    int btn_down_pressed;
    int btn_up_hovered;
    int btn_down_hovered;
    int editing;
    char edit_buf[32];
    int cursor_pos;
    int cursor_visible;
    int cursor_timer;
    int repeat_timer;
    int repeat_active;
    int repeat_dir;
} SpinboxData;

#endif /* GUI_TYPES_H */
