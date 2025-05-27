#include "usb_com.h"
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "forwarder.h"

static speed_t get_baudrate(int baud) {
    switch (baud) {
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        default: return B9600;
    }
}

void usb_com_start(COMConfig *cfg) {

    printf("[USB-COM] Opening port: %s (baud %d)\n", cfg->port, cfg->baudrate);

    if (!cfg) {
        fprintf(stderr, "[USB-COM] ERROR: cfg is NULL\n");
        return;
    }

    if (!cfg->port) {
        fprintf(stderr, "[USB-COM] ERROR: cfg->port is NULL\n");
        return;
    }

    if (strlen(cfg->port) == 0) {
        fprintf(stderr, "[USB-COM] ERROR: cfg->port is empty\n");
        return;
    }

    printf("[USB-COM] Opening port: %s (baud %d)\n", cfg->port, cfg->baudrate);

    int fd = open(cfg->port, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("[USB-COM] Failed to open serial port");
        return;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    tcgetattr(fd, &tty);

    speed_t baud = get_baudrate(cfg->baudrate);
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &tty);

    char buf[100];
    while (1) {
        int len = read(fd, buf, sizeof(buf));
        if (len < 0) {
            perror("[USB-COM] Read error");
            continue;
        }
        
        forwarder_send(buf, len);
    }

    close(fd);
}
