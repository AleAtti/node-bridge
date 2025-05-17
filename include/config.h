#ifndef CONFIG_H
#define CONFIG_H

typedef struct config
{
    int use_com;
    int use_hid;
    char tcp_mode[16];
    int tcp_port;
    char tcp_host[64];

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
   
} Config;

Config load_config(const char* filename);

#endif