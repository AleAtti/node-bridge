#ifndef CONFIG_H
#define CONFIG_H
#include "keyboard_layout.h"
typedef struct config
{
    struct general
    {
        
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

    struct fwd
    {
        int use_interval;
        int interval_ms;
    } Forwarder;

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
        int use_com;
        char port[64];
        int baudrate;
        int databits;
        int stopbits;
        char parity[8];
    } UsbCom;

    struct usb_hid
    {
        int use_hid;
        int vid;
        int pid;
        int endpoint;
        KeyboardLayout keyboard_layout; 
    } UsbHid;

} Config;

Config load_config(const char *filename);

#endif