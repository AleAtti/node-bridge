#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <unistd.h>

#include "config.h"
#include "utils.h"
#include "crypto.h"

Config load_config(const char *filename)
{
	Config cfg = {0};
	size_t size;
	char *data = read_file(filename, &size);
	if (!data || size == 0)
	{
		fprintf(stderr, "Error reading config file: %s\n", filename);
		return cfg; // Return empty config if file read fails
	}

	cJSON *root = cJSON_Parse(data);
	if (!root)
	{
		fprintf(stderr, "[Config] Invalid JSON format!\n");
		free(data);
		return cfg;
	}

	cJSON *general = cJSON_GetObjectItem(root, "general");
	if (general)
	{
		cJSON *hn = cJSON_GetObjectItem(general, "hostname");
		cJSON *ver = cJSON_GetObjectItem(general, "version");

		if (cJSON_IsString(hn))
			strncpy(cfg.General.hostname, hn->valuestring, sizeof(cfg.General.hostname));
		if (cJSON_IsString(ver))
			strncpy(cfg.General.version, ver->valuestring, sizeof(cfg.General.version));
	}

	cJSON *webserver = cJSON_GetObjectItem(root, "webserver");
	if (webserver)
	{
		strcpy(cfg.Webserver.host, cJSON_GetObjectItem(webserver, "host")->valuestring);
		cfg.Webserver.port = cJSON_GetObjectItem(webserver, "port")->valueint;
	}

	cJSON *tcp = cJSON_GetObjectItem(root, "tcp");
	if (tcp)
	{
		strcpy(cfg.Tcp.mode, cJSON_GetObjectItem(tcp, "mode")->valuestring);
		cfg.Tcp.port = cJSON_GetObjectItem(tcp, "port")->valueint;
		strcpy(cfg.Tcp.host, cJSON_GetObjectItem(tcp, "host")->valuestring);
	}

	cJSON *wifi = cJSON_GetObjectItem(root, "wifi");
	if (wifi)
	{
		strcpy(cfg.Wifi.ssid, cJSON_GetObjectItem(wifi, "ssid")->valuestring);
		strcpy(cfg.Wifi.password, cJSON_GetObjectItem(wifi, "password")->valuestring);
		strcpy(cfg.Wifi.static_ip, cJSON_GetObjectItem(wifi, "static_ip")->valuestring);
		strcpy(cfg.Wifi.gateway, cJSON_GetObjectItem(wifi, "gateway")->valuestring);
		strcpy(cfg.Wifi.dns, cJSON_GetObjectItem(wifi, "dns")->valuestring);
		cJSON *use_dhcp = cJSON_GetObjectItem(wifi, "use_dhcp");
		if (cJSON_IsNumber(use_dhcp))
		{
			cfg.Wifi.use_dhcp = use_dhcp->valueint;
		}
	}

	cJSON *lan = cJSON_GetObjectItem(root, "lan");
	if (lan)
	{
		strcpy(cfg.Lan.static_ip, cJSON_GetObjectItem(lan, "static_ip")->valuestring);
		strcpy(cfg.Lan.gateway, cJSON_GetObjectItem(lan, "gateway")->valuestring);
		strcpy(cfg.Lan.dns, cJSON_GetObjectItem(lan, "dns")->valuestring);
	}

	cJSON *com = cJSON_GetObjectItem(root, "usb_com");
	if (com)
	{
		cJSON *use_com = cJSON_GetObjectItem(com, "use_com");
		cJSON *port = cJSON_GetObjectItem(com, "port");
		cJSON *baud = cJSON_GetObjectItem(com, "baudrate");
		cJSON *data = cJSON_GetObjectItem(com, "databits");
		cJSON *stop = cJSON_GetObjectItem(com, "stopbits");
		cJSON *parity = cJSON_GetObjectItem(com, "parity");

		if (cJSON_IsNumber(use_com))
		{
			cfg.UsbCom.use_com = use_com->valueint;
		}

		if (cJSON_IsString(port))
		{
			strncpy(cfg.UsbCom.port, port->valuestring, sizeof(cfg.UsbCom.port));
			cfg.UsbCom.port[sizeof(cfg.UsbCom.port) - 1] = '\0';
		}
		else
		{
			fprintf(stderr, "[Config] usb_com.port missing or invalid\n");
		}

		if (cJSON_IsNumber(baud))
			cfg.UsbCom.baudrate = baud->valueint;
		if (cJSON_IsNumber(data))
			cfg.UsbCom.databits = data->valueint;
		if (cJSON_IsNumber(stop))
			cfg.UsbCom.stopbits = stop->valueint;

		if (cJSON_IsString(parity))
		{
			strncpy(cfg.UsbCom.parity, parity->valuestring, sizeof(cfg.UsbCom.parity));
			cfg.UsbCom.parity[sizeof(cfg.UsbCom.parity) - 1] = '\0';
		}
	}

	cJSON *hid = cJSON_GetObjectItem(root, "usb_hid");
	if (hid && cJSON_IsObject(hid))
	{
		cJSON *use_hid = cJSON_GetObjectItem(hid, "use_hid");
		cJSON *vid_json = cJSON_GetObjectItem(hid, "vid");
		cJSON *pid_json = cJSON_GetObjectItem(hid, "pid");
		cJSON *endpoint_json = cJSON_GetObjectItem(hid, "endpoint");
		cJSON *layout_json = cJSON_GetObjectItem(hid, "keyboard_layout");

		if (cJSON_IsNumber(use_hid))
		{
			cfg.UsbHid.use_hid = use_hid->valueint;
		}

		if (cJSON_IsString(vid_json) && cJSON_IsString(pid_json))
		{
			cfg.UsbHid.vid = (int)strtol(vid_json->valuestring, NULL, 16);
			cfg.UsbHid.pid = (int)strtol(pid_json->valuestring, NULL, 16);
		}
		else
		{
			fprintf(stderr, "[Config] usb_hid.vid or pid missing/invalid\n");
			cfg.UsbHid.vid = 0;
			cfg.UsbHid.pid = 0;
		}

		if (cJSON_IsNumber(endpoint_json))
		{
			cfg.UsbHid.endpoint = endpoint_json->valueint;
		}
		else
		{
			fprintf(stderr, "[Config] usb_hid.endpoint missing or invalid, defaulting to 0\n");
			cfg.UsbHid.endpoint = 0;
		}

		if (layout_json && cJSON_IsString(layout_json))
		{
			const char *layout_str = layout_json->valuestring;
			if (strcmp(layout_str, "us") == 0)
			{
				cfg.UsbHid.keyboard_layout = LAYOUT_US;
			}
			else if (strcmp(layout_str, "de") == 0)
			{
				cfg.UsbHid.keyboard_layout = LAYOUT_DE;
			}
			else
			{
				fprintf(stderr, "[Config] usb_hid.keyboard_layout invalid, using default (us)\n");
				cfg.UsbHid.keyboard_layout = LAYOUT_US;
			}
		}
		else
		{
			fprintf(stderr, "[Config] usb_hid.keyboard_layout missing, using default (us)\n");
			cfg.UsbHid.keyboard_layout = LAYOUT_US;
		}
	}

	cJSON *fwd = cJSON_GetObjectItem(root, "forwarder");
	if (fwd)
	{
		cJSON *use_interval = cJSON_GetObjectItem(fwd, "use_interval");
		if (cJSON_IsNumber(use_interval))
			cfg.Forwarder.use_interval = use_interval->valueint;

		cJSON *interval_ms = cJSON_GetObjectItem(fwd, "interval_ms");
		if (cJSON_IsNumber(interval_ms))
			cfg.Forwarder.interval_ms = interval_ms->valueint;
	}

	cJSON_Delete(root);
	free(data);
	return cfg;
}

int secure_config_and_save(const char *raw_json, const char *path)
{
	
    if (!raw_json || !path) {
        fprintf(stderr, "[Config] Invalid arguments to secure_config_and_save\n");
        return -1;
    }

    printf("[Config] Raw input JSON: %s\n", raw_json);

    cJSON *root = cJSON_Parse(raw_json);
    if (!root) {
        fprintf(stderr, "[Config] Failed to parse JSON input\n");
        return -1;
    }

    cJSON *wifi = cJSON_GetObjectItemCaseSensitive(root, "wifi");
    if (!wifi || !cJSON_IsObject(wifi)) {
        fprintf(stderr, "[Config] 'wifi' object missing or invalid in config\n");
    } else {
        cJSON *pw = cJSON_GetObjectItemCaseSensitive(wifi, "password");

        if (!pw) {
            fprintf(stderr, "[Config] 'password' key not found in wifi section\n");
        } else if (!cJSON_IsString(pw)) {
            fprintf(stderr, "[Config] 'password' is not a string\n");
        } else if (!pw->valuestring) {
            fprintf(stderr, "[Config] 'password' string is NULL\n");
        } else if (strlen(pw->valuestring) == 0) {
            fprintf(stderr, "[Config] 'password' is an empty string — skipping encryption\n");
        } else {
            printf("[Config] Encrypting Wi-Fi password: %s\n", pw->valuestring);

            char *encrypted = NULL;
            if (!encrypt(pw->valuestring, &encrypted)) {
                fprintf(stderr, "[Config] Encryption failed\n");
                cJSON_Delete(root);
                return -1;
            }

            // Replace plaintext password with encrypted
            cJSON_DeleteItemFromObject(wifi, "password");
            cJSON_AddStringToObject(wifi, "password", encrypted);
            free(encrypted);
        }
    }

    char *modified_json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if (!modified_json) {
        fprintf(stderr, "[Config] Failed to serialize modified JSON\n");
        return -1;
    }

    printf("[Config] Saving modified config to: %s\n", path);
    int result = save_file(path, modified_json, strlen(modified_json));
    free(modified_json);

    if (result != 0) {
        fprintf(stderr, "[Config] Failed to write to config file: %s\n", path);
    } else {
        printf("[Config] Config saved successfully.\n");
    }

    return result;
}

void print_config(const Config *cfg)
{
	printf("=== NodeBridge Config ===\n");
	printf("verision: %s\n", cfg->General.version);
	printf("hostname: %s\n", cfg->General.hostname);

	printf("\n[WebServer]\n");
	printf("host: %s\n", cfg->Webserver.host);
	printf("port: %d\n", cfg->Webserver.port);

	printf("\n[TCP]\n");
	printf("tcp_mode: %s\n", cfg->Tcp.mode);
	printf("tcp_port: %d\n", cfg->Tcp.port);
	printf("tcp_host: %s\n", cfg->Tcp.host);

	printf("\n[Wi-Fi]\n");
	printf("ssid: %s\n", cfg->Wifi.ssid);
	printf("password: %s\n", cfg->Wifi.password);
	printf("static_ip: %s\n", cfg->Wifi.static_ip);
	printf("gateway: %s\n", cfg->Wifi.gateway);
	printf("dns: %s\n", cfg->Wifi.dns);
	printf("use_dhcp: %d\n", cfg->Wifi.use_dhcp);

	printf("\n[Lan]\n");
	printf("static_ip: %s\n", cfg->Lan.static_ip);
	printf("gateway: %s\n", cfg->Lan.gateway);
	printf("dns: %s\n", cfg->Lan.dns);

	printf("\n[USB-COM]\n");
	printf("use_com: %d\n", cfg->UsbCom.use_com);
	printf("port: %s\n", cfg->UsbCom.port);
	printf("baudrate: %d\n", cfg->UsbCom.baudrate);
	printf("databits: %d\n", cfg->UsbCom.databits);
	printf("stopbits: %d\n", cfg->UsbCom.stopbits);
	printf("parity: %s\n", cfg->UsbCom.parity);

	printf("\n[USB-HID]\n");
	printf("use_hid: %d\n", cfg->UsbHid.use_hid);
	printf("VID: %04x (%d)\n", cfg->UsbHid.vid, cfg->UsbHid.vid);
	printf("PID: %04x (%d)\n", cfg->UsbHid.pid, cfg->UsbHid.pid);
	printf("Endpoint: %d\n", cfg->UsbHid.endpoint);
	printf("Layout: %s\n", cfg->UsbHid.keyboard_layout == LAYOUT_DE ? "de" : "us");

	printf("\n[Forwarder]\n");
	printf("use_interval: %d\n", cfg->Forwarder.use_interval);
	printf("interval_ms: %d\n", cfg->Forwarder.interval_ms);
	printf("=========================\n");
}

//! TODO: Load Config as JSON string for js
