#ifndef CONFIG_H
#define CONFIG_H

typedef struct config
{
    struct general
    {
        int use_com;
        int use_hid;
        char hostname[64];
        char version[16];
    } General;

    struct webserver
    {
        char host[64];
        int port;
    } Webserver;

    // Maybe future feature??
    // struct mqtt
    // {
    //     char broker[64];
    //     int port;
    //     char topic[64];
    //     char client_id[64];
    //     char username[64];
    //     char password[64];
    // } Mqtt;

    struct tcp
    {
        char mode[16];
        int port;
        char host[64];
    } Tcp;

    struct wifi
    {
        char ssid[256];
        char password[256];
        char static_ip[32];
        char gateway[32];
        char dns[32];
    } Wifi;

    struct lan
    {
        char static_ip[32];
        char gateway[32];
        char dns[32];
    } Lan;

    struct usb_com
    {
        char port[64];
        int baudrate;
        int databits;
        int stopbits;
        char parity[8];
    } UsbCom;

    struct usb_hid
    {
        int vid;
        int pid;
        int endpoint;
    } UsbHid;

} Config;

Config load_config(const char *filename);

#endif