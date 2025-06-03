#include "keyboard_ch.h"
#include <logger.h>

#define NUM_KEYCODES 256
#define NUM_MODES 4

enum
{
    MOD_PLAIN = 0,
    MOD_SHIFT = 1,
    MOD_ALTGR = 2,
    MOD_SHIFT_ALTGR = 3
    
};

// Modifier flags (USB HID spec)
#define MOD_LEFT_SHIFT   0x02
#define MOD_RIGHT_SHIFT  0x20
#define MOD_RIGHT_ALT    0x40

// Define 2D layout table
static const char *layout_ch[NUM_MODES][NUM_KEYCODES] = {
    [MOD_PLAIN] = {
        [0x00] = "", [0x01] = "", [0x02] = "", [0x03] = "",
        [0x04] = "a", [0x05] = "b", [0x06] = "c", [0x07] = "d", [0x08] = "e", [0x09] = "f", [0x0A] = "g", [0x0B] = "h",
        [0x0C] = "i", [0x0D] = "j", [0x0E] = "k", [0x0F] = "l", [0x10] = "m", [0x11] = "n", [0x12] = "o", [0x13] = "p",
        [0x14] = "q", [0x15] = "r", [0x16] = "s", [0x17] = "t", [0x18] = "u", [0x19] = "v", [0x1A] = "w", [0x1B] = "x",
        [0x1C] = "z", [0x1D] = "y", [0x1E] = "1", [0x1F] = "2", [0x20] = "3", [0x21] = "4", [0x22] = "5", [0x23] = "6",
        [0x24] = "7", [0x25] = "8", [0x26] = "9", [0x27] = "0", [0x28] = "\n", [0x29] = "", [0x2A] = "",
        [0x2B] = "\t", [0x2C] = " ", [0x2D] = "'", [0x2E] = "¨", [0x2F] = "ü", [0x30] = "*", [0x31] = "ä", [0x32] = "^",
        [0x33] = "ö", [0x34] = "ä", [0x35] = "$", [0x36] = ",", [0x37] = ".", [0x38] = "-", [0x39] = ""
    },
    [MOD_SHIFT] = {
        [0x04] = "A", [0x05] = "B", [0x06] = "C", [0x07] = "D", [0x08] = "E", [0x09] = "F", [0x0A] = "G", [0x0B] = "H",
        [0x0C] = "I", [0x0D] = "J", [0x0E] = "K", [0x0F] = "L", [0x10] = "M", [0x11] = "N", [0x12] = "O", [0x13] = "P",
        [0x14] = "Q", [0x15] = "R", [0x16] = "S", [0x17] = "T", [0x18] = "U", [0x19] = "V", [0x1A] = "W", [0x1B] = "X",
        [0x1C] = "Y", [0x1D] = "Z", [0x1E] = "+", [0x1F] = "\"", [0x20] = "*", [0x21] = "ç", [0x22] = "%", [0x23] = "&",
        [0x24] = "/", [0x25] = "(", [0x26] = ")", [0x27] = "=", [0x2D] = "\"", [0x2E] = "!", [0x2F] = "Ü",
        [0x30] = "°", [0x31] = "Ä", [0x32] = "¨", [0x33] = ":", [0x34] = "Ö", [0x36] = "<", [0x37] = ">", [0x38] = "_"
    },
    [MOD_ALTGR] = {
        [0x1F] = "@", [0x20] = "{", [0x21] = "[", [0x22] = "]", [0x23] = "}", [0x24] = "\\", [0x25] = "|",
        [0x26] = "~", [0x2F] = "è", [0x33] = "é", [0x34] = "à", [0x30] = "€"
    },
    [MOD_SHIFT_ALTGR] = {
        [0x21] = "¬"  // example
    }
};

// Lookup function
const char *hid_to_char_ch(uint8_t keycode, uint8_t modifiers)
{
    int mode = 0;

    if (modifiers & (MOD_LEFT_SHIFT | MOD_RIGHT_SHIFT)) {
        mode |= MOD_SHIFT;
    }
    if (modifiers & MOD_RIGHT_ALT) {
        mode |= MOD_ALTGR;
    }

    if (keycode >= NUM_KEYCODES) {
        return "";
    }

    const char *val = layout_ch[mode][keycode];
    logger_log(LOG_LEVEL_DEBUG, "[Keyboard] Keycode: %02X, Modifiers: %02X, Mode: %d, Char: %s",
               keycode, modifiers, mode, val ? val : "NULL");

    return val ? val : "";
}
