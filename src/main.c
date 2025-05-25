#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


#include "utils.h"
#include "config.h"
#include "webserver.h"
#include "usb_hid.h"
#include "usb_com.h"
#include "tcp_server.h"
#include "tcp_client.h"


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

void print_config(const Config *cfg)
{
    printf("=== NodeBridge Config ===\n");
    printf("use_com: %s\n", cfg->General.version);
    printf("use_hid: %s\n", cfg->General.hostname);
    printf("use_com: %d\n", cfg->General.use_com);
    printf("use_hid: %d\n", cfg->General.use_hid);
    printf("tcp_mode: %s\n", cfg->Tcp.mode);
    printf("tcp_port: %d\n", cfg->Tcp.port);
    printf("tcp_host: %s\n", cfg->Tcp.host);

    printf("\n[Wi-Fi]\n");
    printf("ssid: %s\n", cfg->Wifi.ssid);
    printf("password: %s\n", cfg->Wifi.password);
    printf("static_ip: %s\n", cfg->Wifi.static_ip);
    printf("gateway: %s\n", cfg->Wifi.gateway);
    printf("dns: %s\n", cfg->Wifi.dns);

    printf("\n[Lan]\n");
    printf("static_ip: %s\n", cfg->Lan.static_ip);
    printf("gateway: %s\n", cfg->Lan.gateway);
    printf("dns: %s\n", cfg->Lan.dns);

    printf("\n[USB-COM]\n");
    printf("port: %s\n", cfg->UsbCom.port);
    printf("baudrate: %d\n", cfg->UsbCom.baudrate);
    printf("databits: %d\n", cfg->UsbCom.databits);
    printf("stopbits: %d\n", cfg->UsbCom.stopbits);
    printf("parity: %s\n", cfg->UsbCom.parity);

    printf("\n[USB-HID]\n");
    printf("VID: %d\n", cfg->UsbHid.vid);
    printf("PID: %d\n", cfg->UsbHid.pid);
    printf("Endpoint: %d\n", cfg->UsbHid.endpoint);
}


int main(int argc, char **argv)
{
    Config cfg = load_config("config.json");
    print_config(&cfg);
    pthread_t hid_t, com_t;
    if (cfg.General.use_hid && !cfg.General.use_com)
    {
        printf("\n[USB-HID] Starting...\n");
        list_usb_devices();
        pthread_create(&hid_t, NULL, hid_thread, &cfg.UsbHid);
    }
    else if (!cfg.General.use_hid && cfg.General.use_com)
    {
        printf("[USB-COM] Starting...\n");
        printf("[DEBUG] Port: %s\n", cfg.UsbCom.port);
        pthread_create(&com_t, NULL, com_thread, &cfg.UsbCom);
    }

    if (strcmp(cfg.Tcp.mode, "server") == 0)
    {
        printf("\n[TCP Server] Starting on port %d\n", cfg.Tcp.port);
        start_tcp_server(cfg.Tcp.port);
    }
    else if (strcmp(cfg.Tcp.mode, "client") == 0)
    {
        printf("\n[TCP Client] Connecting to %s:%d\n", cfg.Tcp.host, cfg.Tcp.port);
        start_tcp_client(cfg.Tcp.host, cfg.Tcp.port);
    }
   
    

    printf("\n[WebServer] Starting on %s:%d\n", cfg.Webserver.host, cfg.Webserver.port);
    start_webserver(cfg.Webserver.port);

    return 0;
}
