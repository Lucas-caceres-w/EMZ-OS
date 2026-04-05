#ifndef KEYCODES_H
#define KEYCODES_H

typedef enum {
    KEY_NULL = 0,
    KEY_ESC = 27,
    KEY_BACKSPACE = '\b',
    KEY_TAB = '\t',
    KEY_ENTER = '\n',
    
    // Teclas de Función
    KEY_F1 = 0x80, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    
    // Navegación
    KEY_UP = 0x90, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_HOME, KEY_END, KEY_INSERT, KEY_DELETE, KEY_PAGE_UP, KEY_PAGE_DOWN,
    
    // Modificadores
    KEY_LSHIFT, KEY_RSHIFT, KEY_LCTRL, KEY_RCTRL, KEY_LALT, KEY_RALT,
    KEY_CAPS_LOCK, KEY_SCROLL_LOCK, KEY_NUM_LOCK
} keycode_t;

#endif