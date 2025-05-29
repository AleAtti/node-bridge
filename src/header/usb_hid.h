/**
 * @file usb_hid.h
 * @brief Header file for USB HID functionality.
 * This file defines the HIDConfig structure and declares functions for starting USB HID communication and listing USB devices.
 * @note This project uses the C17 language standard version.
 * @note Ensure that the hidapi library is linked when compiling this code.
 * @note The hidapi library must be installed on your system to use this functionality.
 * @note resources: https://github.com/libusb/hidapi?tab=readme-ov-file#what-does-the-api-look-like
 */

#ifndef USB_HID_H
#define USB_HID_H
#include "keyboard_layout.h"
typedef struct {
    int use_hid;
    int vid;
    int pid;
    int endpoint;
    KeyboardLayout keyboard_layout; 
} HIDConfig;

void usb_hid_start(HIDConfig *cfg);

int list_usb_devices(void);

#endif

