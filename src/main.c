
/* ============================================================================
 * ALLEGRO 4 GUI TOOLKIT - SINGLE WINDOW TABBED DEMO
 * All widgets demonstrated in categorized tabs within one window
 * ============================================================================ */

#include "gui_types.h"
#include "gui_state.h"
#include "gui_utils.h"
#include "gui_widget.h"
#include "gui_core.h"
#include "gui_window.h"
#include "gui_button.h"
#include "gui_label.h"
#include "gui_input.h"
#include "gui_checkbox.h"
#include "gui_radio.h"
#include "gui_combobox.h"
#include "gui_menu.h"
#include "gui_menubar.h"
#include "gui_statusbar.h"
#include "gui_tab.h"
#include "gui_panel.h"
#include "gui_scrollbar.h"
#include "gui_box.h"
#include "gui_slider.h"
#include "gui_spinbox.h"
#include "gui_progressbar.h"
#include "gui_listview.h"
#include "gui_tooltip.h"
#include "gui_groupbox.h"
#include "gui_textarea.h"
#include "gui_colorpicker.h"
#include "gui_toolbar.h"
#include "gui_splitter.h"

#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ============================================================================
 * GLOBALS
 * ============================================================================ */

static Widget *g_statusbar = NULL;
static Widget *g_main_win = NULL;

/* Tab 1: Form */
static Widget *g_name_input = NULL;
static Widget *g_email_input = NULL;
static Widget *g_pass_input = NULL;
static Widget *g_age_spin = NULL;
static Widget *g_country_combo = NULL;
static Widget *g_gender_male = NULL;
static Widget *g_gender_female = NULL;
static Widget *g_gender_other = NULL;
static Widget *g_agree_check = NULL;
static Widget *g_newsletter_check = NULL;
static Widget *g_form_result_lbl = NULL;

/* Tab 2: Sliders & Progress */
static Widget *g_progress_h1 = NULL;
static Widget *g_progress_h2 = NULL;
static Widget *g_progress_h3 = NULL;
static Widget *g_slider_h = NULL;
static Widget *g_slider_val_lbl = NULL;
static Widget *g_progress_val_lbl = NULL;

/* Tab 3: ListView */
static Widget *g_listview = NULL;
static Widget *g_list_info_lbl = NULL;
static Widget *g_list_count_lbl = NULL;

/* Tab 4: TextArea */
static Widget *g_textarea = NULL;
static Widget *g_ta_line_lbl = NULL;
static Widget *g_ta_readonly_check = NULL;
static Widget *g_ta_linenums_check = NULL;

/* Tab 5: ColorPicker */
static Widget *g_colorpicker = NULL;
static Widget *g_color_r_lbl = NULL;
static Widget *g_color_g_lbl = NULL;
static Widget *g_color_b_lbl = NULL;
static Widget *g_color_hex_lbl = NULL;

/* Tab 6: Splitter */
static Widget *g_splitter_h = NULL;

/* Icons */
static BITMAP *g_icon_new = NULL;
static BITMAP *g_icon_open = NULL;
static BITMAP *g_icon_save = NULL;
static BITMAP *g_icon_cut = NULL;
static BITMAP *g_icon_copy = NULL;
static BITMAP *g_icon_paste = NULL;
static BITMAP *g_icon_undo = NULL;
static BITMAP *g_icon_redo = NULL;
static BITMAP *g_icon_info = NULL;
static BITMAP *g_icon_bold = NULL;
static BITMAP *g_icon_italic = NULL;

/* ============================================================================
 * HELPERS
 * ============================================================================ */

static void status_set(const char *fmt, ...) {
    if (!g_statusbar) return;
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    statusbar_set_text(g_statusbar, buf);
}

static Widget *make_hsep(void) {
    Widget *sep = widget_create(WIDGET_BASE, 0, 0, 1, 6);
    sep->fill = FILL_X;
    return sep;
}

/* ============================================================================
 * ICON CREATION
 * ============================================================================ */

static BITMAP *make_icon(int shape, int r, int g, int b) {
    BITMAP *bmp = create_bitmap(16, 16);
    clear_to_color(bmp, makecol(255, 0, 255));
    int col = makecol(r, g, b);
    int dark = makecol(r / 2, g / 2, b / 2);

    switch (shape) {
        case 0: rectfill(bmp, 2, 2, 13, 13, col); rect(bmp, 2, 2, 13, 13, dark); break;
        case 1: rectfill(bmp, 1, 4, 14, 13, col); rectfill(bmp, 1, 2, 7, 4, col);
                rect(bmp, 1, 2, 7, 4, dark); rect(bmp, 1, 4, 14, 13, dark); break;
        case 2: rectfill(bmp, 2, 1, 13, 14, col); rect(bmp, 2, 1, 13, 14, dark);
                rectfill(bmp, 4, 1, 11, 5, makecol(200, 200, 200));
                rectfill(bmp, 5, 9, 10, 13, makecol(180, 180, 180)); break;
        case 3: line(bmp, 2, 2, 13, 13, col); line(bmp, 13, 2, 2, 13, col);
                circlefill(bmp, 3, 3, 2, col); circlefill(bmp, 12, 3, 2, col); break;
        case 4: rectfill(bmp, 4, 4, 13, 13, col); rect(bmp, 4, 4, 13, 13, dark);
                rectfill(bmp, 1, 1, 10, 10, makecol(MIN(r+60,255), MIN(g+60,255), MIN(b+60,255)));
                rect(bmp, 1, 1, 10, 10, dark); break;
        case 5: rectfill(bmp, 2, 3, 13, 14, col); rect(bmp, 2, 3, 13, 14, dark);
                rectfill(bmp, 5, 1, 10, 4, makecol(180, 150, 100));
                rect(bmp, 5, 1, 10, 4, dark); break;
        case 6: arc(bmp, 8, 8, itofix(32), itofix(192), 5, col);
                line(bmp, 3, 5, 3, 9, col); line(bmp, 3, 5, 7, 5, col); break;
        case 7: arc(bmp, 8, 8, itofix(224), itofix(128), 5, col);
                line(bmp, 12, 5, 12, 9, col); line(bmp, 8, 5, 12, 5, col); break;
        case 8: circlefill(bmp, 8, 8, 6, col); circle(bmp, 8, 8, 6, dark);
                textout_centre_ex(bmp, font, "i", 8, 5, makecol(255,255,255), -1); break;
        case 9: rectfill(bmp, 3, 2, 12, 13, col); rect(bmp, 3, 2, 12, 13, dark);
                textout_centre_ex(bmp, font, "B", 8, 4, makecol(255,255,255), -1); break;
        case 10: rectfill(bmp, 3, 2, 12, 13, col); rect(bmp, 3, 2, 12, 13, dark);
                 textout_centre_ex(bmp, font, "I", 8, 4, makecol(255,255,255), -1); break;
    }
    return bmp;
}

static void create_icons(void) {
    g_icon_new    = make_icon(0, 220, 220, 220);
    g_icon_open   = make_icon(1, 255, 210, 80);
    g_icon_save   = make_icon(2, 100, 150, 220);
    g_icon_cut    = make_icon(3, 200, 80, 80);
    g_icon_copy   = make_icon(4, 80, 160, 80);
    g_icon_paste  = make_icon(5, 200, 170, 100);
    g_icon_undo   = make_icon(6, 80, 80, 200);
    g_icon_redo   = make_icon(7, 80, 80, 200);
    g_icon_info   = make_icon(8, 60, 120, 220);
    g_icon_bold   = make_icon(9, 80, 80, 80);
    g_icon_italic = make_icon(10, 80, 80, 80);
}

static void destroy_icons(void) {
    BITMAP **icons[] = {
        &g_icon_new, &g_icon_open, &g_icon_save, &g_icon_cut, &g_icon_copy,
        &g_icon_paste, &g_icon_undo, &g_icon_redo, &g_icon_info,
        &g_icon_bold, &g_icon_italic
    };
    for (int i = 0; i < 11; i++) {
        if (*icons[i]) { destroy_bitmap(*icons[i]); *icons[i] = NULL; }
    }
}

/* ============================================================================
 * MENU CALLBACKS
 * ============================================================================ */

static void on_file_new(Widget *s, Event *e) {
    (void)s; (void)e;
    if (g_textarea) textarea_set_text(g_textarea, "");
    status_set("New file");
}

static void on_file_open(Widget *s, Event *e) {
    (void)s; (void)e;
    if (g_textarea) {
        textarea_set_text(g_textarea,
            "#include <stdio.h>\n"
            "#include <stdlib.h>\n"
            "\n"
            "typedef struct {\n"
            "    char name[64];\n"
            "    int age;\n"
            "    float score;\n"
            "} Student;\n"
            "\n"
            "void print_student(const Student *s) {\n"
            "    printf(\"Name: %s\\n\", s->name);\n"
            "    printf(\"Age:  %d\\n\", s->age);\n"
            "    printf(\"Score: %.1f\\n\", s->score);\n"
            "}\n"
            "\n"
            "int main(void) {\n"
            "    Student st = {\"Alice\", 20, 95.5f};\n"
            "    print_student(&st);\n"
            "    return 0;\n"
            "}\n");
    }
    status_set("File opened (simulated)");
}

static void on_file_save(Widget *s, Event *e) {
    (void)s; (void)e; status_set("File saved (simulated)");
}

static void on_file_exit(Widget *s, Event *e) {
    (void)s; (void)e; gui_state.running = 0;
}

static void on_edit_action(Widget *s, Event *e) {
    (void)s; (void)e; status_set("Edit action (not implemented)");
}

static void on_help_about(Widget *self, Event *event) {
    (void)self; (void)event;
    Widget *win = window_create("About", 350, 250, 340, 200);
    Widget *cl = window_get_client(win);

    Widget *t = label_create("Allegro 4 GUI Toolkit Demo", 0, 0);
    ((LabelData *)t->extra)->text_align = ALIGN_CENTER;
    ((LabelData *)t->extra)->text_color = theme.titlebar_active;
    t->fill = FILL_X;
    widget_add_child(cl, t);

    widget_add_child(cl, make_hsep());

    const char *lines[] = {
        "Single-window tabbed demo",
        "All 22 widget types shown",
        "in categorized tabs.",
        "",
        "Press Shift+ESC to quit"
    };
    for (int i = 0; i < 5; i++) {
        Widget *l = label_create(lines[i], 0, 0);
        ((LabelData *)l->extra)->text_align = ALIGN_CENTER;
        l->fill = FILL_X;
        widget_add_child(cl, l);
    }

    widget_add_child(cl, make_hsep());

    Widget *ok = button_create("OK", 0, 0, 80, 0);
    ok->align_h = ALIGN_CENTER;
    widget_on(ok, EVENT_CLICK, (EventCallback)on_file_exit); /* just close - reuse */
    widget_add_child(cl, ok);
    /* Override close to just destroy this dialog, not exit app */
}

/* ============================================================================
 * FORM CALLBACKS (Tab 1)
 * ============================================================================ */

static void on_form_submit(Widget *self, Event *event) {
    (void)self; (void)event;
    if (!g_form_result_lbl) return;

    const char *name = g_name_input ? input_get_text(g_name_input) : "";
    const char *email = g_email_input ? input_get_text(g_email_input) : "";
    int age = g_age_spin ? spinbox_get_value(g_age_spin) : 0;
    int agreed = 0;

    if (g_agree_check) {
        CheckboxData *cd = (CheckboxData *)g_agree_check->extra;
        agreed = cd->checked;
    }

    LabelData *ld = (LabelData *)g_form_result_lbl->extra;

    if (strlen(name) == 0) {
        sprintf(ld->text, "Error: Name is required!");
        ld->text_color = makecol(200, 50, 50);
        g_form_result_lbl->dirty = 1;
        status_set("Validation failed: name required");
        return;
    }
    if (!agreed) {
        sprintf(ld->text, "Error: Must agree to terms!");
        ld->text_color = makecol(200, 50, 50);
        g_form_result_lbl->dirty = 1;
        status_set("Validation failed: terms not agreed");
        return;
    }

    char gender[16] = "N/A";
    if (g_gender_male && ((RadioData *)g_gender_male->extra)->selected) strcpy(gender, "Male");
    if (g_gender_female && ((RadioData *)g_gender_female->extra)->selected) strcpy(gender, "Female");
    if (g_gender_other && ((RadioData *)g_gender_other->extra)->selected) strcpy(gender, "Other");

    sprintf(ld->text, "OK! %s, %d, %s", name, age, gender);
    ld->text_color = makecol(50, 150, 50);
    g_form_result_lbl->dirty = 1;
    status_set("Form submitted: %s (%s), age %d, %s", name, email, age, gender);
}

static void on_form_clear(Widget *self, Event *event) {
    (void)self; (void)event;
    if (g_name_input) input_set_text(g_name_input, "");
    if (g_email_input) input_set_text(g_email_input, "");
    if (g_pass_input) input_set_text(g_pass_input, "");
    if (g_age_spin) spinbox_set_value(g_age_spin, 25);
    if (g_form_result_lbl) {
        LabelData *ld = (LabelData *)g_form_result_lbl->extra;
        strcpy(ld->text, " ");
        g_form_result_lbl->dirty = 1;
    }
    status_set("Form cleared");
}

/* ============================================================================
 * SLIDER / PROGRESS CALLBACKS (Tab 2)
 * ============================================================================ */

static void on_slider_change(Widget *self, Event *event) {
    (void)event;
    int val = slider_get_value(self);
    if (g_progress_h1) progressbar_set_value(g_progress_h1, val);
    if (g_progress_h2) progressbar_set_value(g_progress_h2, val);
    if (g_progress_h3) progressbar_set_value(g_progress_h3, val);
    if (g_slider_val_lbl) {
        LabelData *ld = (LabelData *)g_slider_val_lbl->extra;
        sprintf(ld->text, "Value: %d", val);
        g_slider_val_lbl->dirty = 1;
    }
    if (g_progress_val_lbl) {
        LabelData *ld = (LabelData *)g_progress_val_lbl->extra;
        sprintf(ld->text, "Progress: %d%%", val);
        g_progress_val_lbl->dirty = 1;
    }
}

static void on_inc10(Widget *s, Event *e) {
    (void)s; (void)e;
    if (g_slider_h) {
        slider_set_value(g_slider_h, slider_get_value(g_slider_h) + 10);
        Event f; memset(&f, 0, sizeof(f)); on_slider_change(g_slider_h, &f);
    }
}

static void on_dec10(Widget *s, Event *e) {
    (void)s; (void)e;
    if (g_slider_h) {
        slider_set_value(g_slider_h, slider_get_value(g_slider_h) - 10);
        Event f; memset(&f, 0, sizeof(f)); on_slider_change(g_slider_h, &f);
    }
}

static void on_reset_prog(Widget *s, Event *e) {
    (void)s; (void)e;
    if (g_slider_h) slider_set_value(g_slider_h, 0);
    Event f; memset(&f, 0, sizeof(f)); on_slider_change(g_slider_h, &f);
    status_set("Progress reset");
}

static void on_fill_prog(Widget *s, Event *e) {
    (void)s; (void)e;
    if (g_slider_h) slider_set_value(g_slider_h, 100);
    Event f; memset(&f, 0, sizeof(f)); on_slider_change(g_slider_h, &f);
    status_set("Progress filled");
}

/* ============================================================================
 * LISTVIEW CALLBACKS (Tab 3)
 * ============================================================================ */

static const char *fruits[] = {
    "Apple","Apricot","Avocado","Banana","Blackberry","Blueberry","Cherry",
    "Coconut","Cranberry","Date","Dragonfruit","Elderberry","Fig","Grape",
    "Grapefruit","Guava","Honeydew","Jackfruit","Kiwi","Kumquat","Lemon",
    "Lime","Lychee","Mango","Mangosteen","Melon","Nectarine","Orange",
    "Papaya","Passionfruit","Peach","Pear","Persimmon","Pineapple","Plum",
    "Pomegranate","Quince","Raspberry","Starfruit","Strawberry","Tangerine",
    "Watermelon"
};
static const int fruit_count = sizeof(fruits) / sizeof(fruits[0]);

static void update_list_count(void) {
    if (!g_list_count_lbl || !g_listview) return;
    LabelData *ld = (LabelData *)g_list_count_lbl->extra;
    sprintf(ld->text, "Items: %d", listview_get_item_count(g_listview));
    g_list_count_lbl->dirty = 1;
}

static void on_list_select(Widget *self, Event *e) {
    (void)e;
    int sel = listview_get_selected(self);
    if (sel >= 0 && g_list_info_lbl) {
        LabelData *ld = (LabelData *)g_list_info_lbl->extra;
        sprintf(ld->text, "[%d] %s", sel, listview_get_item_text(self, sel));
        g_list_info_lbl->dirty = 1;
    }
    status_set("Selected index %d", sel);
}

static void on_list_add(Widget *s, Event *e) {
    (void)s; (void)e;
    static int c = 1;
    char buf[64]; sprintf(buf, "Custom #%d", c++);
    listview_add_item(g_listview, buf);
    update_list_count();
    status_set("Item added");
}

static void on_list_remove(Widget *s, Event *e) {
    (void)s; (void)e;
    int sel = listview_get_selected(g_listview);
    if (sel >= 0) { listview_remove_item(g_listview, sel); update_list_count(); status_set("Item removed"); }
    else status_set("No selection");
}

static void on_list_clear(Widget *s, Event *e) {
    (void)s; (void)e;
    listview_clear(g_listview); update_list_count(); status_set("List cleared");
}

static void on_list_populate(Widget *s, Event *e) {
    (void)s; (void)e;
    listview_clear(g_listview);
    for (int i = 0; i < fruit_count; i++) listview_add_item(g_listview, fruits[i]);
    update_list_count(); status_set("Populated %d items", fruit_count);
}

/* ============================================================================
 * TEXTAREA CALLBACKS (Tab 4)
 * ============================================================================ */

static void on_ta_change(Widget *self, Event *e) {
    (void)e;
    if (g_ta_line_lbl) {
        LabelData *ld = (LabelData *)g_ta_line_lbl->extra;
        sprintf(ld->text, "Lines: %d", textarea_get_line_count(self));
        g_ta_line_lbl->dirty = 1;
    }
}

static void on_ta_readonly(Widget *self, Event *e) {
    (void)e;
    CheckboxData *cd = (CheckboxData *)self->extra;
    if (g_textarea) textarea_set_readonly(g_textarea, cd->checked);
    status_set("Read-only: %s", cd->checked ? "ON" : "OFF");
}

static void on_ta_linenums(Widget *self, Event *e) {
    (void)e;
    CheckboxData *cd = (CheckboxData *)self->extra;
    if (g_textarea) textarea_set_line_numbers(g_textarea, cd->checked);
    status_set("Line numbers: %s", cd->checked ? "ON" : "OFF");
}

static void on_ta_insert(Widget *s, Event *e) {
    (void)s; (void)e;
    if (g_textarea) {
        textarea_set_text(g_textarea,
            "Lorem ipsum dolor sit amet, consectetur\n"
            "adipiscing elit. Sed do eiusmod tempor\n"
            "incididunt ut labore et dolore magna aliqua.\n"
            "\n"
            "Ut enim ad minim veniam, quis nostrud\n"
            "exercitation ullamco laboris nisi ut\n"
            "aliquip ex ea commodo consequat.\n"
            "\n"
            "Duis aute irure dolor in reprehenderit\n"
            "in voluptate velit esse cillum dolore\n"
            "eu fugiat nulla pariatur.\n");
        Event f; memset(&f, 0, sizeof(f)); on_ta_change(g_textarea, &f);
    }
    status_set("Lorem ipsum inserted");
}

/* Toolbar callbacks */
static void on_tb_new(Widget *s, Event *e) { on_file_new(s, e); }
static void on_tb_open(Widget *s, Event *e) { on_file_open(s, e); }
static void on_tb_save(Widget *s, Event *e) { on_file_save(s, e); }
static void on_tb_cut(Widget *s, Event *e)  { on_edit_action(s, e); }
static void on_tb_copy(Widget *s, Event *e) { on_edit_action(s, e); }
static void on_tb_paste(Widget *s, Event *e){ on_edit_action(s, e); }
static void on_tb_undo(Widget *s, Event *e) { on_edit_action(s, e); }
static void on_tb_redo(Widget *s, Event *e) { on_edit_action(s, e); }
static void on_tb_info(Widget *s, Event *e) { on_help_about(s, e); }

static void on_tb_bold(Widget *self, Event *e) {
    (void)e;
    ButtonData *bd = (ButtonData *)self->extra;
    if (bd) status_set("Bold %s", bd->toggled ? "ON" : "OFF");
}

static void on_tb_italic(Widget *self, Event *e) {
    (void)e;
    ButtonData *bd = (ButtonData *)self->extra;
    if (bd) status_set("Italic %s", bd->toggled ? "ON" : "OFF");
}

/* ============================================================================
 * COLORPICKER CALLBACKS (Tab 5)
 * ============================================================================ */

static void on_color_change(Widget *self, Event *e) {
    (void)e;
    int r, g, b;
    colorpicker_get_rgb(self, &r, &g, &b);
    if (g_color_r_lbl) { LabelData *ld=(LabelData*)g_color_r_lbl->extra; sprintf(ld->text,"R: %3d",r); g_color_r_lbl->dirty=1; }
    if (g_color_g_lbl) { LabelData *ld=(LabelData*)g_color_g_lbl->extra; sprintf(ld->text,"G: %3d",g); g_color_g_lbl->dirty=1; }
    if (g_color_b_lbl) { LabelData *ld=(LabelData*)g_color_b_lbl->extra; sprintf(ld->text,"B: %3d",b); g_color_b_lbl->dirty=1; }
    if (g_color_hex_lbl) { LabelData *ld=(LabelData*)g_color_hex_lbl->extra; sprintf(ld->text,"#%02X%02X%02X",r,g,b); g_color_hex_lbl->dirty=1; }
    status_set("Color: R=%d G=%d B=%d", r, g, b);
}

static void set_preset(Widget *s, Event *e, int r, int g, int b) {
    (void)s; (void)e;
    if (g_colorpicker) {
        colorpicker_set_color(g_colorpicker, r, g, b);
        Event f; memset(&f, 0, sizeof(f)); on_color_change(g_colorpicker, &f);
    }
}

static void on_preset_red(Widget *s, Event *e) { set_preset(s, e, 220, 50, 50); }
static void on_preset_green(Widget *s, Event *e) { set_preset(s, e, 50, 180, 50); }
static void on_preset_blue(Widget *s, Event *e) { set_preset(s, e, 50, 80, 220); }
static void on_preset_gold(Widget *s, Event *e) { set_preset(s, e, 255, 200, 50); }
static void on_preset_purple(Widget *s, Event *e) { set_preset(s, e, 150, 50, 200); }
static void on_preset_white(Widget *s, Event *e) { set_preset(s, e, 255, 255, 255); }
static void on_preset_black(Widget *s, Event *e) { set_preset(s, e, 0, 0, 0); }
static void on_preset_orange(Widget *s, Event *e) { set_preset(s, e, 255, 140, 0); }

/* ============================================================================
 * TAB BUILDERS
 * ============================================================================ */

static void build_tab_form(Widget *page) {
    page->layout = LAYOUT_VERTICAL;
    page->spacing = 6;
    widget_set_padding_all(page, 8);

    /* Title */
    Widget *title = label_create("Registration Form", 0, 0);
    ((LabelData *)title->extra)->text_align = ALIGN_CENTER;
    ((LabelData *)title->extra)->text_color = theme.titlebar_active;
    ((LabelData *)title->extra)->bold = 1;
    title->fill = FILL_X;
    widget_add_child(page, title);

    widget_add_child(page, make_hsep());

    /* --- Personal Info --- */
    Widget *grp = groupbox_create("Personal Information", 0, 0, 10, 160);
    grp->fill = FILL_X;
    widget_add_child(page, grp);

    /* Name row */
    Widget *r1 = hbox_create_simple(6); r1->fill = FILL_X; r1->h = INPUT_HEIGHT;
    widget_add_child(grp, r1);
    Widget *l1 = label_create("Name:", 0, 0); l1->min_w = 65; l1->align_v = ALIGN_CENTER;
    widget_add_child(r1, l1);
    g_name_input = input_create(0, 0, 150);
    input_set_placeholder(g_name_input, "Full name");
    widget_set_expand(g_name_input, 1, 1);
    tooltip_set(g_name_input, "Required field");
    widget_add_child(r1, g_name_input);

    /* Email row */
    Widget *r2 = hbox_create_simple(6); r2->fill = FILL_X; r2->h = INPUT_HEIGHT;
    widget_add_child(grp, r2);
    Widget *l2 = label_create("Email:", 0, 0); l2->min_w = 65; l2->align_v = ALIGN_CENTER;
    widget_add_child(r2, l2);
    g_email_input = input_create(0, 0, 150);
    input_set_placeholder(g_email_input, "user@example.com");
    widget_set_expand(g_email_input, 1, 1);
    widget_add_child(r2, g_email_input);

    /* Password row */
    Widget *r2b = hbox_create_simple(6); r2b->fill = FILL_X; r2b->h = INPUT_HEIGHT;
    widget_add_child(grp, r2b);
    Widget *l2b = label_create("Password:", 0, 0); l2b->min_w = 65; l2b->align_v = ALIGN_CENTER;
    widget_add_child(r2b, l2b);
    g_pass_input = input_create(0, 0, 150);
    input_set_placeholder(g_pass_input, "Enter password");
    input_set_password_mode(g_pass_input, 1);
    widget_set_expand(g_pass_input, 1, 1);
    tooltip_set(g_pass_input, "Password (masked)");
    widget_add_child(r2b, g_pass_input);

    /* Age + Country row */
    Widget *r3 = hbox_create_simple(6); r3->fill = FILL_X; r3->h = INPUT_HEIGHT;
    widget_add_child(grp, r3);
    Widget *l3 = label_create("Age:", 0, 0); l3->min_w = 65; l3->align_v = ALIGN_CENTER;
    widget_add_child(r3, l3);
    g_age_spin = spinbox_create(0, 0, 70, 1, 120);
    spinbox_set_value(g_age_spin, 25);
    tooltip_set(g_age_spin, "Age 1-120");
    widget_add_child(r3, g_age_spin);

    Widget *sp3 = widget_create(WIDGET_BASE, 0, 0, 10, 1);
    widget_add_child(r3, sp3);
    Widget *cl = label_create("Country:", 0, 0); cl->align_v = ALIGN_CENTER;
    widget_add_child(r3, cl);
    g_country_combo = combobox_create(0, 0, 120);
    const char *countries[] = {"Turkey","USA","UK","Germany","France","Japan","Other"};
    for (int i = 0; i < 7; i++) combobox_add_item(g_country_combo, countries[i]);
    tooltip_set(g_country_combo, "Select country");
    widget_add_child(r3, g_country_combo);

    Widget *sp3b = widget_create(WIDGET_BASE, 0, 0, 1, 1);
    widget_set_expand(sp3b, 1, 1);
    widget_add_child(r3, sp3b);

    /* --- Gender --- */
    Widget *grp_g = groupbox_create("Gender", 0, 0, 10, 45);
    grp_g->fill = FILL_X;
    grp_g->layout = LAYOUT_HORIZONTAL;
    grp_g->spacing = 20;
    widget_add_child(page, grp_g);

    g_gender_male = radio_create("Male", 1, 0, 0);
    ((RadioData *)g_gender_male->extra)->selected = 1;
    widget_add_child(grp_g, g_gender_male);
    g_gender_female = radio_create("Female", 1, 0, 0);
    widget_add_child(grp_g, g_gender_female);
    g_gender_other = radio_create("Other", 1, 0, 0);
    widget_add_child(grp_g, g_gender_other);

    /* --- Options --- */
    Widget *grp_o = groupbox_create("Options", 0, 0, 10, 60);
    grp_o->fill = FILL_X;
    widget_add_child(page, grp_o);

    g_agree_check = checkbox_create("I agree to Terms & Conditions", 0, 0);
    tooltip_set(g_agree_check, "Required");
    widget_add_child(grp_o, g_agree_check);
    g_newsletter_check = checkbox_create("Subscribe to newsletter", 0, 0);
    widget_add_child(grp_o, g_newsletter_check);

    /* Buttons */
    Widget *brow = hbox_create_simple(8); brow->fill = FILL_X; brow->h = BUTTON_HEIGHT;
    widget_add_child(page, brow);
    Widget *bsp = widget_create(WIDGET_BASE, 0, 0, 1, 1);
    widget_set_expand(bsp, 1, 1);
    widget_add_child(brow, bsp);

    Widget *sub = button_create("Submit", 0, 0, 90, 0);
    widget_on(sub, EVENT_CLICK, on_form_submit);
    tooltip_set(sub, "Submit form");
    widget_add_child(brow, sub);

    Widget *clr = button_create("Clear", 0, 0, 80, 0);
    widget_on(clr, EVENT_CLICK, on_form_clear);
    tooltip_set(clr, "Clear all fields");
    widget_add_child(brow, clr);

    /* Result */
    g_form_result_lbl = label_create(" ", 0, 0);
    g_form_result_lbl->fill = FILL_X;
    ((LabelData *)g_form_result_lbl->extra)->text_align = ALIGN_CENTER;
    widget_add_child(page, g_form_result_lbl);
}

static void build_tab_sliders(Widget *page) {
    page->layout = LAYOUT_VERTICAL;
    page->spacing = 6;
    widget_set_padding_all(page, 8);

    Widget *title = label_create("Progress Bars & Sliders", 0, 0);
    ((LabelData *)title->extra)->text_align = ALIGN_CENTER;
    ((LabelData *)title->extra)->text_color = theme.titlebar_active;
    title->fill = FILL_X;
    widget_add_child(page, title);
    widget_add_child(page, make_hsep());

    /* Progress bars group */
    Widget *grp = groupbox_create("Progress Bars", 0, 0, 10, 130);
    grp->fill = FILL_X;
    widget_add_child(page, grp);

    /* Solid */
    Widget *r1 = hbox_create_simple(6); r1->fill = FILL_X; r1->h = PROGRESSBAR_HEIGHT + 4;
    widget_add_child(grp, r1);
    Widget *l1 = label_create("Solid:", 0, 0); l1->min_w = 60; l1->align_v = ALIGN_CENTER;
    widget_add_child(r1, l1);
    g_progress_h1 = progressbar_create(1, 0, 0, 200);
    progressbar_set_value(g_progress_h1, 45);
    widget_set_expand(g_progress_h1, 1, 1);
    tooltip_set(g_progress_h1, "Standard solid bar");
    widget_add_child(r1, g_progress_h1);

    /* Striped */
    Widget *r2 = hbox_create_simple(6); r2->fill = FILL_X; r2->h = PROGRESSBAR_HEIGHT + 4;
    widget_add_child(grp, r2);
    Widget *l2 = label_create("Striped:", 0, 0); l2->min_w = 60; l2->align_v = ALIGN_CENTER;
    widget_add_child(r2, l2);
    g_progress_h2 = progressbar_create(1, 0, 0, 200);
    progressbar_set_value(g_progress_h2, 45);
    progressbar_set_style(g_progress_h2, 1);
    progressbar_set_colors(g_progress_h2, makecol(50, 160, 50), 0, 0);
    widget_set_expand(g_progress_h2, 1, 1);
    tooltip_set(g_progress_h2, "Striped animated bar");
    widget_add_child(r2, g_progress_h2);

    /* Custom */
    Widget *r3 = hbox_create_simple(6); r3->fill = FILL_X; r3->h = PROGRESSBAR_HEIGHT + 4;
    widget_add_child(grp, r3);
    Widget *l3 = label_create("Custom:", 0, 0); l3->min_w = 60; l3->align_v = ALIGN_CENTER;
    widget_add_child(r3, l3);
    g_progress_h3 = progressbar_create(1, 0, 0, 200);
    progressbar_set_value(g_progress_h3, 45);
    progressbar_set_colors(g_progress_h3, makecol(200, 100, 30), makecol(50,50,50), makecol(255,255,200));
    widget_set_expand(g_progress_h3, 1, 1);
    tooltip_set(g_progress_h3, "Custom colored bar");
    widget_add_child(r3, g_progress_h3);

    /* Slider control group */
    Widget *grp2 = groupbox_create("Slider Control", 0, 0, 10, 80);
    grp2->fill = FILL_X;
    widget_add_child(page, grp2);

    Widget *sr = hbox_create_simple(8); sr->fill = FILL_X; sr->h = 30;
    widget_add_child(grp2, sr);
    Widget *sl = label_create("Value:", 0, 0); sl->min_w = 50; sl->align_v = ALIGN_CENTER;
    widget_add_child(sr, sl);
    g_slider_h = slider_create(1, 0, 0, 200, 0, 100);
    slider_set_value(g_slider_h, 45);
    widget_set_expand(g_slider_h, 1, 1);
    widget_on(g_slider_h, EVENT_CHANGE, on_slider_change);
    tooltip_set(g_slider_h, "Drag to control progress bars");
    widget_add_child(sr, g_slider_h);

    Widget *vr = hbox_create_simple(12); vr->fill = FILL_X; vr->h = 20;
    widget_add_child(grp2, vr);
    g_slider_val_lbl = label_create("Value: 45", 0, 0);
    g_slider_val_lbl->min_w = 80; g_slider_val_lbl->align_v = ALIGN_CENTER;
    widget_add_child(vr, g_slider_val_lbl);
    g_progress_val_lbl = label_create("Progress: 45%", 0, 0);
    widget_set_expand(g_progress_val_lbl, 1, 1);
    widget_add_child(vr, g_progress_val_lbl);

    /* Buttons */
    Widget *br = hbox_create_simple(6); br->fill = FILL_X; br->h = BUTTON_HEIGHT;
    widget_add_child(page, br);
    Widget *bsp = widget_create(WIDGET_BASE, 0, 0, 1, 1); widget_set_expand(bsp, 1, 1);
    widget_add_child(br, bsp);

    Widget *b1 = button_create("-10", 0, 0, 50, 0); widget_on(b1, EVENT_CLICK, on_dec10);
    tooltip_set(b1, "Decrease 10"); widget_add_child(br, b1);
    Widget *b2 = button_create("+10", 0, 0, 50, 0); widget_on(b2, EVENT_CLICK, on_inc10);
    tooltip_set(b2, "Increase 10"); widget_add_child(br, b2);
    Widget *b3 = button_create("Reset", 0, 0, 60, 0); widget_on(b3, EVENT_CLICK, on_reset_prog);
    widget_add_child(br, b3);
    Widget *b4 = button_create("Fill", 0, 0, 50, 0); widget_on(b4, EVENT_CLICK, on_fill_prog);
    widget_add_child(br, b4);
}

static void build_tab_listview(Widget *page) {
    page->layout = LAYOUT_VERTICAL;
    page->spacing = 4;
    widget_set_padding_all(page, 8);

    Widget *title = label_create("ListView Demo", 0, 0);
    ((LabelData *)title->extra)->text_align = ALIGN_CENTER;
    ((LabelData *)title->extra)->text_color = theme.titlebar_active;
    title->fill = FILL_X;
    widget_add_child(page, title);
    widget_add_child(page, make_hsep());

    /* Main row: list + side buttons */
    Widget *main = hbox_create_simple(8);
    main->fill = FILL_BOTH;
    widget_set_expand(main, 1, 1);
    widget_add_child(page, main);

    g_listview = listview_create(0, 0, 250, 200);
    g_listview->fill = FILL_BOTH;
    widget_set_expand(g_listview, 1, 1);
    listview_set_header(g_listview, "Fruit Name");
    widget_on(g_listview, EVENT_CHANGE, on_list_select);
    for (int i = 0; i < fruit_count; i++) listview_add_item(g_listview, fruits[i]);
    tooltip_set(g_listview, "Click to select, arrows to navigate");
    widget_add_child(main, g_listview);

    Widget *side = vbox_create_simple(4);
    side->min_w = 100;
    widget_add_child(main, side);

    Widget *ba = button_create("Add Item", 0, 0, 95, 0);
    widget_on(ba, EVENT_CLICK, on_list_add); tooltip_set(ba, "Add custom item");
    widget_add_child(side, ba);
    Widget *br2 = button_create("Remove", 0, 0, 95, 0);
    widget_on(br2, EVENT_CLICK, on_list_remove); tooltip_set(br2, "Remove selected");
    widget_add_child(side, br2);
    Widget *bc = button_create("Clear All", 0, 0, 95, 0);
    widget_on(bc, EVENT_CLICK, on_list_clear); tooltip_set(bc, "Clear list");
    widget_add_child(side, bc);
    Widget *bp = button_create("Populate", 0, 0, 95, 0);
    widget_on(bp, EVENT_CLICK, on_list_populate); tooltip_set(bp, "Reload fruits");
    widget_add_child(side, bp);

    widget_add_child(side, make_hsep());

    g_list_count_lbl = label_create("Items: 42", 0, 0);
    widget_add_child(side, g_list_count_lbl);

    g_list_info_lbl = label_create("Select an item", 0, 0);
    ((LabelData *)g_list_info_lbl->extra)->text_color = theme.disabled_fg;
    widget_add_child(side, g_list_info_lbl);

    widget_add_child(side, make_hsep());

    const char *help[] = {"Keys:", " Up/Down", " PgUp/PgDn", " Home/End"};
    for (int i = 0; i < 4; i++) {
        Widget *h = label_create(help[i], 0, 0);
        ((LabelData *)h->extra)->text_color = theme.disabled_fg;
        widget_add_child(side, h);
    }
}

static void build_tab_textarea(Widget *page) {
    page->layout = LAYOUT_VERTICAL;
    page->spacing = 4;
    widget_set_padding_all(page, 4);

    /* Toolbar */
    Widget *tb = toolbar_create();
    toolbar_add_button(tb, g_icon_new, "New", on_tb_new);
    toolbar_add_button(tb, g_icon_open, "Open", on_tb_open);
    toolbar_add_button(tb, g_icon_save, "Save", on_tb_save);
    toolbar_add_separator(tb);
    toolbar_add_button(tb, g_icon_cut, "Cut", on_tb_cut);
    toolbar_add_button(tb, g_icon_copy, "Copy", on_tb_copy);
    toolbar_add_button(tb, g_icon_paste, "Paste", on_tb_paste);
    toolbar_add_separator(tb);
    toolbar_add_button(tb, g_icon_undo, "Undo", on_tb_undo);
    toolbar_add_button(tb, g_icon_redo, "Redo", on_tb_redo);
    toolbar_add_separator(tb);
    toolbar_add_toggle(tb, g_icon_bold, "Bold", on_tb_bold);
    toolbar_add_toggle(tb, g_icon_italic, "Italic", on_tb_italic);
    toolbar_add_separator(tb);
    toolbar_add_button(tb, g_icon_info, "About", on_tb_info);
    widget_add_child(page, tb);

    /* TextArea */
    g_textarea = textarea_create(0, 0, 500, 280);
    g_textarea->fill = FILL_BOTH;
    widget_set_expand(g_textarea, 1, 1);
    textarea_set_line_numbers(g_textarea, 1);
    widget_on(g_textarea, EVENT_CHANGE, on_ta_change);
    tooltip_set(g_textarea, "Multi-line text editor");

    textarea_set_text(g_textarea,
        "/* Welcome to the Text Editor */\n"
        "\n"
        "This widget supports:\n"
        "  - Multi-line editing\n"
        "  - Arrow key navigation\n"
        "  - Home / End / PgUp / PgDn\n"
        "  - Backspace and Delete\n"
        "  - Enter for new lines\n"
        "  - Tab inserts spaces\n"
        "  - Line numbers (toggle below)\n"
        "  - Current line highlighting\n"
        "  - Read-only mode\n"
        "\n"
        "Try the toolbar buttons above!\n"
        "Click Open to load sample code.\n");
    widget_add_child(page, g_textarea);

    /* Bottom options */
    Widget *opt = hbox_create_simple(10); opt->fill = FILL_X; opt->h = 22;
    widget_add_child(page, opt);

    g_ta_linenums_check = checkbox_create("Line Numbers", 0, 0);
    ((CheckboxData *)g_ta_linenums_check->extra)->checked = 1;
    widget_on(g_ta_linenums_check, EVENT_CHANGE, on_ta_linenums);
    widget_add_child(opt, g_ta_linenums_check);

    g_ta_readonly_check = checkbox_create("Read-only", 0, 0);
    widget_on(g_ta_readonly_check, EVENT_CHANGE, on_ta_readonly);
    widget_add_child(opt, g_ta_readonly_check);

    Widget *ins = button_create("Insert Lorem", 0, 0, 100, 0);
    widget_on(ins, EVENT_CLICK, on_ta_insert);
    widget_add_child(opt, ins);

    Widget *spc = widget_create(WIDGET_BASE, 0, 0, 1, 1); widget_set_expand(spc, 1, 1);
    widget_add_child(opt, spc);

    g_ta_line_lbl = label_create("Lines: 15", 0, 0);
    g_ta_line_lbl->align_v = ALIGN_CENTER;
    widget_add_child(opt, g_ta_line_lbl);
}

static void build_tab_colorpicker(Widget *page) {
    page->layout = LAYOUT_VERTICAL;
    page->spacing = 8;
    widget_set_padding_all(page, 8);

    Widget *title = label_create("Color Picker", 0, 0);
    ((LabelData *)title->extra)->text_align = ALIGN_CENTER;
    ((LabelData *)title->extra)->text_color = theme.titlebar_active;
    title->fill = FILL_X;
    widget_add_child(page, title);
    widget_add_child(page, make_hsep());

    /* Color picker */
    Widget *grp = groupbox_create("RGB Color Selector", 0, 0, 10, 160);
    grp->fill = FILL_X;
    widget_add_child(page, grp);

    g_colorpicker = colorpicker_create(0, 0, 400, 120);
    g_colorpicker->fill = FILL_X;
    colorpicker_set_color(g_colorpicker, 100, 150, 220);
    widget_on(g_colorpicker, EVENT_CHANGE, on_color_change);
    tooltip_set(g_colorpicker, "Drag R/G/B sliders");
    widget_add_child(grp, g_colorpicker);

    /* Color info */
    Widget *info = hbox_create_simple(12); info->fill = FILL_X; info->h = 18;
    widget_add_child(page, info);

    g_color_r_lbl = label_create("R: 100", 0, 0);
    ((LabelData *)g_color_r_lbl->extra)->text_color = makecol(200, 50, 50);
    widget_add_child(info, g_color_r_lbl);

    g_color_g_lbl = label_create("G: 150", 0, 0);
    ((LabelData *)g_color_g_lbl->extra)->text_color = makecol(50, 150, 50);
    widget_add_child(info, g_color_g_lbl);

    g_color_b_lbl = label_create("B: 220", 0, 0);
    ((LabelData *)g_color_b_lbl->extra)->text_color = makecol(50, 80, 220);
    widget_add_child(info, g_color_b_lbl);

    Widget *spc = widget_create(WIDGET_BASE, 0, 0, 1, 1); widget_set_expand(spc, 1, 1);
    widget_add_child(info, spc);

    g_color_hex_lbl = label_create("#6496DC", 0, 0);
    widget_add_child(info, g_color_hex_lbl);

    /* Presets */
    Widget *pgrp = groupbox_create("Presets", 0, 0, 10, 50);
    pgrp->fill = FILL_X;
    pgrp->layout = LAYOUT_HORIZONTAL;
    pgrp->spacing = 6;
    widget_add_child(page, pgrp);

    struct { const char *name; EventCallback cb; } presets[] = {
        {"Red", on_preset_red}, {"Green", on_preset_green}, {"Blue", on_preset_blue},
        {"Gold", on_preset_gold}, {"Purple", on_preset_purple},
        {"White", on_preset_white}, {"Black", on_preset_black}, {"Orange", on_preset_orange}
    };
    for (int i = 0; i < 8; i++) {
        Widget *b = button_create(presets[i].name, 0, 0, 55, 0);
        widget_on(b, EVENT_CLICK, presets[i].cb);
        widget_add_child(pgrp, b);
    }
}

static void build_tab_splitter(Widget *page) {
    page->layout = LAYOUT_VERTICAL;
    page->spacing = 0;
    widget_set_padding_all(page, 2);

    g_splitter_h = splitter_create(1, 0, 0, 500, 400);
    g_splitter_h->fill = FILL_BOTH;
    widget_set_expand(g_splitter_h, 1, 1);
    splitter_set_position(g_splitter_h, 200);
    splitter_set_min_sizes(g_splitter_h, 100, 120);
    widget_add_child(page, g_splitter_h);

    /* Left pane */
    Widget *left = splitter_get_first(g_splitter_h);
    left->layout = LAYOUT_VERTICAL;
    left->spacing = 6;
    widget_set_padding_all(left, 8);

    Widget *lt = label_create("Left Panel", 0, 0);
    ((LabelData *)lt->extra)->text_align = ALIGN_CENTER;
    ((LabelData *)lt->extra)->text_color = theme.titlebar_active;
    lt->fill = FILL_X;
    widget_add_child(left, lt);
    widget_add_child(left, make_hsep());

    Widget *li = input_create(0, 0, 100);
    input_set_placeholder(li, "Type here...");
    li->fill = FILL_X;
    widget_add_child(left, li);

    Widget *lc1 = checkbox_create("Option A", 0, 0); widget_add_child(left, lc1);
    Widget *lc2 = checkbox_create("Option B", 0, 0); widget_add_child(left, lc2);
    Widget *lc3 = checkbox_create("Option C", 0, 0); widget_add_child(left, lc3);

    Widget *lcb = combobox_create(0, 0, 100);
    combobox_add_item(lcb, "Choice 1"); combobox_add_item(lcb, "Choice 2");
    combobox_add_item(lcb, "Choice 3"); combobox_add_item(lcb, "Choice 4");
    lcb->fill = FILL_X;
    widget_add_child(left, lcb);

    Widget *lb = button_create("Click Me", 0, 0, 100, 0);
    lb->fill = FILL_X;
    widget_add_child(left, lb);

    Widget *ls = slider_create(1, 0, 0, 100, 0, 100);
    slider_set_value(ls, 50);
    ls->fill = FILL_X;
    widget_add_child(left, ls);

    /* Right pane: vertical splitter */
    Widget *right = splitter_get_second(g_splitter_h);
    right->layout = LAYOUT_VERTICAL;
    right->spacing = 0;
    widget_set_padding_all(right, 0);

    Widget *vsplit = splitter_create(0, 0, 0, 300, 400);
    vsplit->fill = FILL_BOTH;
    widget_set_expand(vsplit, 1, 1);
    splitter_set_position(vsplit, 160);
    splitter_set_min_sizes(vsplit, 60, 60);
    widget_add_child(right, vsplit);

    /* Top-right */
    Widget *top = splitter_get_first(vsplit);
    top->layout = LAYOUT_VERTICAL;
    top->spacing = 4;
    widget_set_padding_all(top, 8);

    Widget *tt = label_create("Top-Right", 0, 0);
    ((LabelData *)tt->extra)->text_color = makecol(50, 150, 50);
    ((LabelData *)tt->extra)->text_align = ALIGN_CENTER;
    tt->fill = FILL_X;
    widget_add_child(top, tt);

    Widget *tr1 = radio_create("Small", 5, 0, 0);
    ((RadioData *)tr1->extra)->selected = 1;
    widget_add_child(top, tr1);
    Widget *tr2 = radio_create("Medium", 5, 0, 0); widget_add_child(top, tr2);
    Widget *tr3 = radio_create("Large", 5, 0, 0); widget_add_child(top, tr3);

    Widget *tp = progressbar_create(1, 0, 0, 100);
    progressbar_set_value(tp, 65);
    progressbar_set_style(tp, 1);
    tp->fill = FILL_X;
    widget_add_child(top, tp);

    /* Bottom-right */
    Widget *bot = splitter_get_second(vsplit);
    bot->layout = LAYOUT_VERTICAL;
    bot->spacing = 4;
    widget_set_padding_all(bot, 8);

    Widget *bt = label_create("Bottom-Right", 0, 0);
    ((LabelData *)bt->extra)->text_color = makecol(200, 100, 30);
    ((LabelData *)bt->extra)->text_align = ALIGN_CENTER;
    bt->fill = FILL_X;
    widget_add_child(bot, bt);

    Widget *bspin = spinbox_create(0, 0, 80, 0, 999);
    spinbox_set_value(bspin, 42);
    widget_add_child(bot, bspin);

    Widget *bslider = slider_create(1, 0, 0, 100, 0, 100);
    slider_set_value(bslider, 50);
    bslider->fill = FILL_X;
    widget_add_child(bot, bslider);

    Widget *bprog = progressbar_create(1, 0, 0, 100);
    progressbar_set_value(bprog, 70);
    progressbar_set_colors(bprog, makecol(100, 60, 200), 0, 0);
    bprog->fill = FILL_X;
    widget_add_child(bot, bprog);

    Widget *binp = input_create(0, 0, 100);
    input_set_placeholder(binp, "Notes...");
    binp->fill = FILL_X;
    widget_add_child(bot, binp);
}

static void build_tab_about(Widget *page) {
    page->layout = LAYOUT_VERTICAL;
    page->spacing = 8;
    widget_set_padding_all(page, 20);

    Widget *t1 = label_create("Allegro 4 GUI Toolkit", 0, 0);
    ((LabelData *)t1->extra)->text_align = ALIGN_CENTER;
    ((LabelData *)t1->extra)->text_color = theme.titlebar_active;
    t1->fill = FILL_X;
    widget_add_child(page, t1);

    widget_add_child(page, make_hsep());

    const char *lines[] = {
        "Single-Window Tabbed Demo Application",
        "",
        "Widget Types Demonstrated:",
        "",
        "  Window, Button, ImageButton, Label,",
        "  Input, Checkbox, Radio, ComboBox,",
        "  MenuBar, Menu, StatusBar, TabControl,",
        "  Panel, ScrollBar, HBox, VBox,",
        "  Slider, SpinBox, ProgressBar, ListView,",
        "  TextArea, ColorPicker, Toolbar, Splitter,",
        "  GroupBox, Tooltip, Separator",
        "",
        "Total: 22+ widget types",
        "",
        "Shortcuts:",
        "  Shift+ESC .... Quit",
        "  Arrow keys ... Navigate",
        "  Tab .......... Next field",
    };
    for (int i = 0; i < 18; i++) {
        Widget *l = label_create(lines[i], 0, 0);
        LabelData *ld = (LabelData *)l->extra;
        ld->text_align = ALIGN_CENTER;
        if (i == 12) ld->text_color = makecol(50, 150, 50);
        l->fill = FILL_X;
        widget_add_child(page, l);
    }
}

/* ============================================================================
 * MENUBAR
 * ============================================================================ */

static Widget *create_menubar(void) {
    Widget *mb = menubar_create();

    Widget *file = menubar_add_menu(mb, "File");
    menu_add_item(file, "New",     "Ctrl+N", on_file_new);
    menu_add_item(file, "Open...", "Ctrl+O", on_file_open);
    menu_add_item(file, "Save",   "Ctrl+S", on_file_save);
    menu_add_separator(file);
    menu_add_item(file, "Exit",   "Shift+Esc", on_file_exit);

    Widget *edit = menubar_add_menu(mb, "Edit");
    menu_add_item(edit, "Undo",       "Ctrl+Z", on_edit_action);
    menu_add_item(edit, "Redo",       "Ctrl+Y", on_edit_action);
    menu_add_separator(edit);
    menu_add_item(edit, "Cut",        "Ctrl+X", on_edit_action);
    menu_add_item(edit, "Copy",       "Ctrl+C", on_edit_action);
    menu_add_item(edit, "Paste",      "Ctrl+V", on_edit_action);

    Widget *view = menubar_add_menu(mb, "View");
    menu_add_item(view, "Reset Progress", "", on_reset_prog);
    menu_add_item(view, "Fill Progress",  "", on_fill_prog);

    Widget *help = menubar_add_menu(mb, "Help");
    menu_add_item(help, "About", "F1", on_help_about);

    return mb;
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(void) {
    gui_init(800, 600);
    create_icons();

    /* Single main window, filling the screen */
    g_main_win = window_create("Allegro 4 GUI Toolkit - Widget Demo", 0, 0, 800, 600);

    /* Menubar */
    Widget *mb = create_menubar();
    window_set_menubar(g_main_win, mb);

    /* Statusbar */
    g_statusbar = statusbar_create();
    statusbar_set_text(g_statusbar, "Ready - All widgets in categorized tabs | Shift+ESC to quit");
    window_set_statusbar(g_main_win, g_statusbar);

    /* Client area */
    Widget *client = window_get_client(g_main_win);
    client->layout = LAYOUT_VERTICAL;
    client->spacing = 0;
    widget_set_padding_all(client, 2);

    /* Tab control filling the client area */
    Widget *tabs = tab_create(0, 0, 780, 500);
    tabs->fill = FILL_BOTH;
    widget_set_expand(tabs, 1, 1);
    widget_add_child(client, tabs);

    /* Build each tab */
    Widget *p1 = tab_add(tabs, "Form");
    build_tab_form(p1);

    Widget *p2 = tab_add(tabs, "Sliders");
    build_tab_sliders(p2);

    Widget *p3 = tab_add(tabs, "ListView");
    build_tab_listview(p3);

    Widget *p4 = tab_add(tabs, "TextArea");
    build_tab_textarea(p4);

    Widget *p5 = tab_add(tabs, "Colors");
    build_tab_colorpicker(p5);

    Widget *p6 = tab_add(tabs, "Splitter");
    build_tab_splitter(p6);

    Widget *p7 = tab_add(tabs, "About");
    build_tab_about(p7);

    /* Run */
    gui_main_loop();

    /* Cleanup */
    destroy_icons();
    gui_shutdown();
    return 0;
}
END_OF_MAIN()
