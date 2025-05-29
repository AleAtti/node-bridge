#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdint.h>
#include <hidapi/hidapi.h>
#include "usb_hid.h"
#include "forwarder.h"
#include "utils.h"

void usb_hid_start(HIDConfig *cfg)
{
    if (hid_init())
    {
        fprintf(stderr, "[USB-HID] Initialization failed\n");
        return;
    }

    hid_device *dev = hid_open(cfg->vid, cfg->pid, NULL);
    if (!dev)
    {
        fprintf(stderr, "[USB-HID] Device not found VID:0x%04x PID:0x%04x\n", cfg->vid, cfg->pid);
        return;
    }

    unsigned char buf[8];
    unsigned char internal_buffer[256];
    size_t buf_index = 0;

    while (1)
    {
        int len = hid_read(dev, buf, sizeof(buf));
        if (len > 0)
        {
            // printf("[DEBUG] Raw HID data: ");
            // for (int i = 0; i < len; i++)
            // {
            //     printf("%02X ", buf[i]);
            // }
            // printf("\n");
            int shift = buf[1] & (0x02 | 0x20); // Left (0x02) or Right Shift (0x20)

            for (int i = 2; i < 8; i++)
            {
                if (buf[i] != 0)
                {
                    char *str = hid_to_ascii(buf[i], shift, cfg->keyboard_layout);

                    if (str || str[0] != '\0')
                    {
                        for (size_t i = 0; i < str[i] != '\0'; i++)
                        {
                            if (buf_index < 256 - 1)
                            {
                                internal_buffer[buf_index++] = str[i];
                            }else
                            {
                                //!TODO: create a function to handle buffer
                                fprintf(stderr, "[USB-HID] Buffer overflow, resetting buffer\n");
                                internal_buffer[buf_index] = '\0'; 
                                forwarder_send((unsigned char *)internal_buffer, buf_index);
                                buf_index = 0; 
                            }
                            //!TODO: add prefix and suffix to the string
                            if (str[i] == '\n' || str[i] == '\r')
                            {
                                internal_buffer[buf_index] = '\0'; // Null-terminate the string
                                printf("[USB-HID] Sending: %s\n", internal_buffer);
                                forwarder_send((unsigned char *)internal_buffer, buf_index);
                                buf_index = 0; // Reset buffer index after sending
                            }
                            
                        }
                    }
                }
            }
        }
    }

    hid_close(dev);
    hid_exit();
}

int list_usb_devices(void)
{
    printf("[USB-HID] Listing devices...\n");
    int res = hid_init();
    if (res != 0)
    {
        fprintf(stderr, "Failed to initialize hidapi\n");
        return 1;
    }

    struct hid_device_info *devs, *cur_dev;
    devs = hid_enumerate(0x0, 0x0); // 0x0, 0x0 → list all devices
    cur_dev = devs;

    while (cur_dev)
    {
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