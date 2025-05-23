// src/usb/usb_com.c
#include "usb_com.h"
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

void usb_com_start(COMConfig *cfg) {
    int fd = open(cfg->port, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("[USB-COM] Failed to open serial port");
        return;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    tcgetattr(fd, &tty);

    cfsetospeed(&tty, cfg->baudrate);
    cfsetispeed(&tty, cfg->baudrate);
    tty.c_cflag |= (CLOCAL | CREAD);

    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;     // 8 data bits

    tcsetattr(fd, TCSANOW, &tty);

    char buf[100];
    while (1) {
        int n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            printf("[USB-COM] Read %d bytes: ", n);
            for (int i = 0; i < n; ++i) printf("%02X ", (unsigned char)buf[i]);
            printf("\n");
        }
    }

    close(fd);
}
