#include "config.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config load_config(const char *filename)
{
	Config cfg = {0};
	FILE *f = fopen(filename, "r");
	if (!f)
		return cfg;
	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	rewind(f);
	char *data = malloc(len + 1);
	fread(data, 1, len, f);
	data[len] = 0;
	fclose(f);

	cJSON *root = cJSON_Parse(data);
	if (!root)
		return cfg;

	// cfg.use_com = cJSON_GetObjectItem(root, "use_com")->valueint;
	// cfg.use_hid = cJSON_GetObjectItem(root, "use_hid")->valueint;
	// strcpy(cfg.tcp_mode, cJSON_GetObjectItem(root, "tcp_mode")->valuestring);
	// cfg.tcp_port = cJSON_GetObjectItem(root, "tcp_port")->valueint;
	// strcpy(cfg.tcp_host, cJSON_GetObjectItem(root, "tcp_host")->valuestring);

	cJSON *general = cJSON_GetObjectItem(root, "general");
	if (general)
	{
		strcpy(cfg.General.hostname, cJSON_GetObjectItem(general, "hostname")->valuestring);
		strcpy(cfg.General.version, cJSON_GetObjectItem(general, "version")->valuestring);
		cfg.General.use_com = cJSON_GetObjectItem(general, "use_com")->valuestring;
		cfg.General.use_hid = cJSON_GetObjectItem(general, "use_hid")->valuestring;
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
		strcpy(cfg.UsbCom.port, cJSON_GetObjectItem(com, "port")->valuestring);
		cfg.UsbCom.baudrate = cJSON_GetObjectItem(com, "baudrate")->valueint;
		cfg.UsbCom.databits = cJSON_GetObjectItem(com, "databits")->valueint;
		cfg.UsbCom.stopbits = cJSON_GetObjectItem(com, "stopbits")->valueint;
		strcpy(cfg.UsbCom.parity, cJSON_GetObjectItem(com, "parity")->valuestring);
	}

	cJSON *hid = cJSON_GetObjectItem(root, "usb_hid");
	if (hid)
	{
		cfg.UsbHid.vid = cJSON_GetObjectItem(hid, "vid")->valueint;
		cfg.UsbHid.pid = cJSON_GetObjectItem(hid, "pid")->valueint;
		cfg.UsbHid.endpoint = cJSON_GetObjectItem(hid, "endpoint")->valueint;
	}

	cJSON_Delete(root);
	free(data);
	return cfg;
}

//! TODO: Implement save_config function

//! TODO: Load Config as JSON string for js
