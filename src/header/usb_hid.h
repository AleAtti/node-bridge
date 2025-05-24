#ifndef USB_HID_H
#define USB_HID_H

typedef struct {
    int vid;
    int pid;
    int endpoint;
} HIDConfig;

void usb_hid_start(HIDConfig *cfg);

int list_usb_devices(void);

#endif

