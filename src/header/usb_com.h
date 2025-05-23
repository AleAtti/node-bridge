#ifndef USB_COM_H
#define USB_COM_H

typedef struct {
    const char *port;
    int baudrate;
    int databits;
    int stopbits;
    const char *parity;
} COMConfig;

void usb_com_start(COMConfig *cfg);

#endif