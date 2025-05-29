#include "keyboard_layout.h"

char hid_to_ascii(uint8_t code, int shift, KeyboardLayout layout)
{
    if (layout == LAYOUT_DE)
    { // German layout
        const char *ascii = ascii_de;
        const char *shift_ascii = shift_ascii_de;

        if (code < sizeof(ascii_de))
            return shift ? shift_ascii[code] : ascii[code];
    }
    else if (layout == LAYOUT_US)
    { // Default to US layout
        const char *ascii = ascii_us;
        const char *shift_ascii = shift_ascii_us;
        if (code < sizeof(ascii))
            return shift ? shift_ascii[code] : ascii[code];
    }

    return 0;
}