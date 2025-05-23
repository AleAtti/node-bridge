#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <hidapi/hidapi.h>
#include "usb_hid.h"

typedef struct {
    int vid;
    int pid;
    int endpoint;
} HIDConfig;

void usb_hid_start(HIDConfig *cfg) {
    if (hid_init()) {
        fprintf(stderr, "[USB-HID] Initialization failed\n");
        return;
    }

    hid_device *dev = hid_open(cfg->vid, cfg->pid, NULL);
    if (!dev) {
        fprintf(stderr, "[USB-HID] Device not found VID:0x%04x PID:0x%04x\n", cfg->vid, cfg->pid);
        return;
    }

    unsigned char buf[64];
    while (1) {
        int res = hid_read(dev, buf, sizeof(buf));
        if (res > 0) {
            printf("[USB-HID] Read %d bytes: ", res);
            for (int i = 0; i < res; ++i) printf("%02X ", buf[i]);
            printf("\n");
        }
    }

    hid_close(dev);
    hid_exit();
}

// int list_usb_devices(void)
// {
// 	libusb_device **devs;
// 	int r;
// 	ssize_t cnt;

// 	r = libusb_init_context(/*ctx=*/NULL, /*options=*/NULL, /*num_options=*/0);
// 	if (r < 0)
// 		return r;

// 	cnt = libusb_get_device_list(NULL, &devs);
// 	if (cnt < 0){
// 		libusb_exit(NULL);
// 		return (int) cnt;
// 	}

// 	print_devs(devs);
// 	libusb_free_device_list(devs, 1);

// 	libusb_exit(NULL);
// 	return 0;
// }

int list_usb_devices(void)
{
    printf("[USB-HID] Listing devices...\n");
    int res = hid_init();
    if (res != 0) {
        fprintf(stderr, "Failed to initialize hidapi\n");
        return 1;
    }

    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x0, 0x0);  // 0x0, 0x0 → list all devices
    cur_dev = devs;

    while (cur_dev) {
        printf("Device:\n");
        printf("  Path:          %s\n", cur_dev->path);
        printf("  VID:PID:       %04hx:%04hx\n", cur_dev->vendor_id, cur_dev->product_id);
        printf("  Manufacturer:  %ls\n", cur_dev->manufacturer_string);
        printf("  Product:       %ls\n", cur_dev->product_string);
        printf("  Serial Number: %ls\n", cur_dev->serial_number);
        printf("  Interface #:   %d\n", cur_dev->interface_number);
        printf("\n");
        cur_dev = cur_dev->next;
    }

    hid_free_enumeration(devs);
    hid_exit();
    return 0;
}