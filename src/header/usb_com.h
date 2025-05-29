#ifndef USB_COM_H
#define USB_COM_H

typedef struct {
    int use_com;
    char port[64];
    int baudrate;
    int databits;
    int stopbits;
    char parity[8];
} COMConfig;

void usb_com_start(COMConfig *cfg);

#endif