#include "gui_state.h"

GuiState gui_state;
volatile int gui_tick = 0;

void gui_timer_handler(void) {
    gui_tick++;
}
END_OF_FUNCTION(gui_timer_handler)
