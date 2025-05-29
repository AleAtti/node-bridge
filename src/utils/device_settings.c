#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

void apply_hostname(const char *hostname)
{
    if (!hostname || strlen(hostname) == 0)
        return;

    char cmd[128];
    snprintf(cmd, sizeof(cmd), "hostnamectl set-hostname \"%s\"", hostname);
    int result = system(cmd);

    if (result == 0)
        printf("[Config] Hostname set to %s\n", hostname);
    else
        fprintf(stderr, "[Config] Failed to set hostname.\n");
}

void apply_wifi_settings(const struct wifi *wifi)
{
    if (!wifi || strlen(wifi->ssid) == 0 || strlen(wifi->password) == 0)
    {
        fprintf(stderr, "[Config] Invalid Wi-Fi settings.\n");
        return;
    }

    // Delete existing connection with same name
    char delete_cmd[256];
    snprintf(delete_cmd, sizeof(delete_cmd),
             "nmcli connection delete \"%s\" 2>/dev/null", wifi->ssid);
    system(delete_cmd);

    // Connect to Wi-Fi
    char connect_cmd[512];
    snprintf(connect_cmd, sizeof(connect_cmd),
             "nmcli device wifi connect \"%s\" password \"%s\" name \"%s\"",
             wifi->ssid, wifi->password, wifi->ssid);

    int result = system(connect_cmd);
    if (result != 0)
    {
        fprintf(stderr, "[Config] Failed to connect to Wi-Fi.\n");
        return;
    }

    printf("[Config] Wi-Fi connected: %s\n", wifi->ssid);

    // Optional: configure static IP
    if (strlen(wifi->static_ip) > 0)
    {
        char ip_cmd[512];
        // DHCP configuration
        if (wifi->use_dhcp)
        {
            snprintf(ip_cmd, sizeof(ip_cmd),
                     "nmcli connection modify \"%s\" ipv4.method auto", wifi->ssid);
            printf("[Config] Using DHCP for Wi-Fi: %s\n", wifi->ssid);
        }
        else // Static IP configuration
        {
            snprintf(ip_cmd, sizeof(ip_cmd),
                 "nmcli connection modify \"%s\" ipv4.method manual ipv4.addresses \"%s/24\" ipv4.gateway \"%s\" ipv4.dns \"%s\"",
                 wifi->ssid, wifi->static_ip, wifi->gateway, wifi->dns);
                 printf("[Config] Static IP applied for Wi-Fi: %s\n", wifi->static_ip);
        }
        
        system(ip_cmd);

        // Restart connection to apply static IP
        char down_cmd[256], up_cmd[256];
        snprintf(down_cmd, sizeof(down_cmd), "nmcli connection down \"%s\"", wifi->ssid);
        snprintf(up_cmd, sizeof(up_cmd), "nmcli connection up \"%s\"", wifi->ssid);
        system(down_cmd);
        system(up_cmd);

        
    }
}

void apply_lan_settings(const struct lan *lan)
{
    if (!lan || strlen(lan->static_ip) == 0)
        return;

    // Set static IP for LAN (eth0)
    const char *profile = "ethernet";
    system("nmcli connection delete \"ethernet\" 2>/dev/null");
    system("nmcli connection add type ethernet ifname eth0 con-name ethernet");

    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "nmcli connection modify \"%s\" ipv4.method manual ipv4.addresses \"%s/24\" ipv4.gateway \"%s\" ipv4.dns \"%s\"",
             profile, lan->static_ip, lan->gateway, lan->dns);
    system(cmd);

    system("nmcli connection up ethernet");
    printf("[Config] LAN static IP applied: %s\n", lan->static_ip);
}

void apply_device_settings(const Config *cfg)
{
    if (!cfg){
        return;
    }

    apply_hostname(cfg->General.hostname);
    apply_wifi_settings(&cfg->Wifi);
    apply_lan_settings(&cfg->Lan);
}
