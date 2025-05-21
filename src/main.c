#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "webserver.h"

void print_config(const Config* cfg) {
    printf("=== NodeBridge Config ===\n");
    printf("use_com: %d\n", cfg->use_com);
    printf("use_hid: %d\n", cfg->use_hid);
    printf("tcp_mode: %s\n", cfg->tcp_mode);
    printf("tcp_port: %d\n", cfg->tcp_port);
    printf("tcp_host: %s\n", cfg->tcp_host);

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

int main(int argc, char** argv){
    Config cfg = load_config("config.json");
    print_config(&cfg);
    start_webserver(8080);

    return 0;
}
