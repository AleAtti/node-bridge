#ifndef KEYBOARD_LAYOUT_H
#define KEYBOARD_LAYOUT_H
#include <stdint.h>

typedef enum {
    LAYOUT_US = 0, // US QWERTY
    LAYOUT_DE = 1, // German QWERTZ
} KeyboardLayout;

extern const char ascii_us[];
extern const char shift_ascii_us[];
extern const char ascii_de[];
extern const char shift_ascii_de[];

char hid_to_ascii(uint8_t code, int shift, KeyboardLayout layout);

#endif