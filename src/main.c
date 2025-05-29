#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "utils.h"
#include "config.h"
#include "webserver.h"
#include "usb_hid.h"
#include "usb_com.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "forwarder.h"

void *hid_thread(void *arg)
{
    usb_hid_start((HIDConfig *)arg);
    return NULL;
}

void *com_thread(void *arg)
{
    usb_com_start((COMConfig *)arg);
    return NULL;
}

int main(int argc, char **argv)
{
    Config cfg = load_config("config.json");
    print_config(&cfg);

    forwarder_init(&cfg);

    if (cfg.UsbHid.use_hid && !cfg.UsbCom.use_com)
    {
        pthread_t hid_t;
        printf("\n[USB-HID] Starting...\n");
        list_usb_devices();
        pthread_create(&hid_t, NULL, hid_thread, &cfg.UsbHid);
    }
    else if (!cfg.UsbHid.use_hid && cfg.UsbCom.use_com)
    {
        pthread_t com_t;
        printf("[USB-COM] Starting...\n");
        printf("[DEBUG] Port: %s\n", cfg.UsbCom.port);
        pthread_create(&com_t, NULL, com_thread, &cfg.UsbCom);
    }

    printf("\n[WebServer] Starting on %s:%d\n", cfg.Webserver.host, cfg.Webserver.port);
    start_webserver(cfg.Webserver.port);

    return 0;
}
