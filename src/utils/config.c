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
		return cfg;

	// cfg.use_com = cJSON_GetObjectItem(root, "use_com")->valueint;
	// cfg.use_hid = cJSON_GetObjectItem(root, "use_hid")->valueint;
	// strcpy(cfg.tcp_mode, cJSON_GetObjectItem(root, "tcp_mode")->valuestring);
	// cfg.tcp_port = cJSON_GetObjectItem(root, "tcp_port")->valueint;
	// strcpy(cfg.tcp_host, cJSON_GetObjectItem(root, "tcp_host")->valuestring);

	cJSON *general = cJSON_GetObjectItem(root, "general");
	if (general)
	{
		cJSON *hn = cJSON_GetObjectItem(general, "hostname");
		cJSON *ver = cJSON_GetObjectItem(general, "version");
		cJSON *use_com = cJSON_GetObjectItem(general, "use_com");
		cJSON *use_hid = cJSON_GetObjectItem(general, "use_hid");

		if (cJSON_IsString(hn))
			strncpy(cfg.General.hostname, hn->valuestring, sizeof(cfg.General.hostname));
		if (cJSON_IsString(ver))
			strncpy(cfg.General.version, ver->valuestring, sizeof(cfg.General.version));
		if (cJSON_IsNumber(use_com))
			cfg.General.use_com = use_com->valueint;
		if (cJSON_IsNumber(use_hid))
			cfg.General.use_hid = use_hid->valueint;
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
		cJSON *port = cJSON_GetObjectItem(com, "port");
		cJSON *baud = cJSON_GetObjectItem(com, "baudrate");
		cJSON *data = cJSON_GetObjectItem(com, "databits");
		cJSON *stop = cJSON_GetObjectItem(com, "stopbits");
		cJSON *parity = cJSON_GetObjectItem(com, "parity");

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
		cJSON *vid_json = cJSON_GetObjectItem(hid, "vid");
		cJSON *pid_json = cJSON_GetObjectItem(hid, "pid");
		if (cJSON_IsString(vid_json) && cJSON_IsString(pid_json))
		{
			cfg.UsbHid.vid = (int)strtol(vid_json->valuestring, NULL, 16); // Convert hex string to int
			cfg.UsbHid.pid = (int)strtol(pid_json->valuestring, NULL, 16);
		}
		cfg.UsbHid.endpoint = cJSON_GetObjectItem(hid, "endpoint")->valueint;
	}

	cJSON_Delete(root);
	free(data);
	return cfg;
}

//! TODO: Implement save_config function

//! TODO: Load Config as JSON string for js
