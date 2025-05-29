#include "keyboard_layout.h"

// HID keycode to ASCII (US layout)
const char* ascii_us[] = {
    "", "", "", "", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",
    "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n", "", "", "\t", " ", "-", "=",
    "[", "]", "\\", "#", ";", "'", "`", ",", ".", "/", "", "", "", "", "", ""
};

const char* shift_ascii_us[] = {
    "", "", "", "", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L",
    "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "\n", "", "", "\t", " ", "_", "+",
    "{", "}", "|", "~", ":", "\"", "~", "<", ">", "?", "", "", "", "", "", ""
};

// HID keycode to ASCII (German layout, QWERTZ)
const char* ascii_de[] = {
    "", "", "", "", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",
    "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n", "", "", "\t", " ", "-", "=",
    "ü", "+", "ä", "#", ";", "ö", "`", ",", ".", "-", "", "", "", "", "", ""
};

const char* shift_ascii_de[] = {
    "", "", "", "", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L",
    "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "!", "\"", "§", "$", "%", "&", "/", "(", ")", "=", "\n", "", "", "\t", " ", "_", "*",
    "Ü", "*", "Ä", "'", ":", "Ö", "~", "<", ">", "?", "", "", "", "", "", ""
};

char *hid_to_ascii(uint8_t code, int shift, KeyboardLayout layout)
{
    if (layout == LAYOUT_DE) {
        if (code < sizeof(ascii_de) / sizeof(ascii_de[0])) {
            return shift ? shift_ascii_de[code] : ascii_de[code];
        }
    } else { // LAYOUT_US or default fallback
        if (code < sizeof(ascii_us) / sizeof(ascii_us[0])) {
            return shift ? shift_ascii_us[code] : ascii_us[code];
        }
    }

    return "";
}