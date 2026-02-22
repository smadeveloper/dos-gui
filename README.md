# dos GUI Library

A lightweight, GUI toolkit built on top of **Allegro 4** for C applications. Designed for building desktop-like interfaces with windows, menus, toolbars, and common widgets — all rendered in software with a classic Win9x-inspired theme.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-DOS%20%7C%20Windows%20%7C%20Linux-lightgrey.svg)
![Language](https://img.shields.io/badge/language-C99-orange.svg)

---
## Screenshots
![screenshot1](blob:https://github.com/c55128f3-7a51-4746-8fe7-caf29ba40a6b)
## Features

### Window Management
- Draggable, resizable windows with titlebars
- Window z-ordering (click to bring to front)
- Maximize / minimize support
- Active window tracking
- Modal window support

### Widget System
- **Hierarchical widget tree** with parent-child relationships
- Automatic layout engines: `LAYOUT_VERTICAL`, `LAYOUT_HORIZONTAL`, `LAYOUT_GRID`, `LAYOUT_NONE`
- Padding and spacing support
- Clipping for child widgets
- Focus management with tab traversal
- Dirty flag system for efficient redraws

### Widgets

| Widget | Description |
|---|---|
| `WIDGET_BUTTON` | Standard push button with label |
| `WIDGET_IMAGE_BUTTON` | Button with icon (BMP image) |
| `WIDGET_TOGGLE_BUTTON` | Toggle (on/off) button |
| `WIDGET_LABEL` | Static text label |
| `WIDGET_TEXTBOX` | Single-line text input with cursor, selection, clipboard |
| `WIDGET_CHECKBOX` | Checkbox with label |
| `WIDGET_RADIO` | Radio button with group support |
| `WIDGET_SLIDER` | Horizontal slider with min/max/value |
| `WIDGET_PROGRESSBAR` | Progress bar with percentage display |
| `WIDGET_COMBOBOX` | Dropdown combobox with item list |
| `WIDGET_LISTBOX` | Scrollable list box |
| `WIDGET_GROUPBOX` | Labeled group container |
| `WIDGET_PANEL` | Generic container panel |
| `WIDGET_STATUSBAR` | Window status bar |
| `WIDGET_TOOLBAR` | Toolbar container for buttons |
| `WIDGET_CANVAS` | Custom drawing surface |
| `WIDGET_TABS` | Tabbed container with multiple pages |
| `WIDGET_MENUBAR` | Menu bar attached to windows |
| `WIDGET_MENU` | Dropdown/popup menus with items |
| `WIDGET_TREEVIEW` | Hierarchical tree view |
| `WIDGET_SPLITPANE` | Resizable split pane (horizontal/vertical) |
| `WIDGET_SCROLLPANE` | Scrollable content area |

### Menu System
- Full menubar with dropdown menus
- Nested submenus
- Menu item separators
- Keyboard shortcut labels
- Hover tracking across menu hierarchy
- Click-to-open, click-to-close behavior
- Menu items with enable/disable state

### Theming
- Centralized theme structure with colors for all widget states
- Classic 3D beveled appearance (Win9x style)
- Customizable colors for:
  - Window chrome (titlebar, borders, buttons)
  - Widget states (normal, hovered, pressed, focused, disabled)
  - Desktop background
  - Menu and menubar colors
  - Scrollbar and slider colors

### Event System
- Event-driven architecture with callback support
- Supported events:
  - `EVENT_CLICK`, `EVENT_DOUBLE_CLICK`
  - `EVENT_MOUSE_DOWN`, `EVENT_MOUSE_UP`, `EVENT_MOUSE_MOVE`
  - `EVENT_MOUSE_ENTER`, `EVENT_MOUSE_LEAVE`
  - `EVENT_KEY_DOWN`, `EVENT_KEY_UP`
  - `EVENT_FOCUS`, `EVENT_BLUR`
  - `EVENT_CHANGE`, `EVENT_SCROLL`
  - `EVENT_RESIZE`, `EVENT_CLOSE`
  - `EVENT_PAINT`, `EVENT_TIMER`
- Multiple listeners per widget per event type

### Tooltips
- Automatic tooltip display on hover
- Configurable delay
- Follows mouse position

### Additional Features
- Double-buffered rendering (flicker-free)
- Combobox dropdown overlay rendering
- Menu popup overlay rendering
- Widget capture (mouse grab) for drag operations
- Scroll wheel support
- BMP image loading for icons and image buttons

---


