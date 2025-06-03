// Swiss German (de_CH) HID keycode to UTF-8 character map using a 2D array
// Supports Plain, Shift, AltGr, Shift+AltGr modifiers
#ifndef KEYBOARD_CH_H
#define KEYBOARD_CH_H
#include <stdint.h>

#define MOD_LEFT_SHIFT   0x02
#define MOD_RIGHT_SHIFT  0x20
#define MOD_RIGHT_ALT    0x40

const char* hid_to_char_ch(uint8_t keycode, uint8_t modifiers);

#endif
