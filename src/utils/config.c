#include "config.h"
#include "utils.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

		if (cJSON_IsNumber(use_com)){
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
	if (hid)
	{
		cJSON *use_hid = cJSON_GetObjectItem(hid, "use_hid");
		cJSON *vid_json = cJSON_GetObjectItem(hid, "vid");
		cJSON *pid_json = cJSON_GetObjectItem(hid, "pid");

		const char *layout_str = cJSON_GetObjectItem(hid, "keyboard_layout")->valuestring;
		if (strcmp(layout_str, "us") == 0){
			cfg.UsbHid.keyboard_layout = LAYOUT_US;
		}else if (strcmp(layout_str, "de") == 0){
			cfg.UsbHid.keyboard_layout = LAYOUT_DE;
		}
		else{
			fprintf(stderr, "[Config] usb_hid.keyboard_layout invalid, defaulting to US layout\n");
			fprintf(stderr, "[Config] USB HID keyboard layout must be either 'us' or 'de'\n");
			cfg.UsbHid.keyboard_layout = LAYOUT_US;
		}

		if (cJSON_IsNumber(use_hid)){
			cfg.UsbHid.use_hid = use_hid->valueint;
		}

		if (cJSON_IsString(vid_json) && cJSON_IsString(pid_json))
		{
			cfg.UsbHid.vid = (int)strtol(vid_json->valuestring, NULL, 16); // Convert hex string to int
			cfg.UsbHid.pid = (int)strtol(pid_json->valuestring, NULL, 16);
		}
		cfg.UsbHid.endpoint = cJSON_GetObjectItem(hid, "endpoint")->valueint;
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

//! TODO: Implement save_config function

//! TODO: Load Config as JSON string for js
