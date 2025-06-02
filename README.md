# NodeBridge

## TODO
- Improve Webserver
- Improve Config Frontend
- make it more robust.

**NodeBridge** is a lightweight (not anymore xD) I/O utility designed to connect USB devices—such as handheld scanners or other peripherals—to a network.
It receives input via **USB-HID** or **USB-COM** and transmits the data over **TCP/IP**, acting as a usb bridge between local hardware and remote systems.

---

## 🔧 Features
- Read data from USB-COM or USB-HID devices
- Send and receive data via TCP/IP (Client or Server mode)
- JSON-based configuration system
- Embedded web interface for live configuration editing
- Static IP/Wi-Fi setup on boot via config file
- Runs as a daemon on Raspberry Pi

---

## 📁 Project Structure
```
nodebridge/
├── include/         # Header files
├── src/             # Source code (modularized)
├── www/             # Web interface (HTML/CSS/JS)
├── logs/            # This will be created at runtime.
├── config.json      # Main configuration file
├── Makefile         # Build system
└── README.md        # This file
```

## Required packages I have installed on my Raspberry Pi
```bash
$ sudo apt-get install build-essential git gdb libcjson-dev libmicrohttpd-dev libhidapi-dev libssl-dev
```
- libcjson-dev 
- libhidapi-dev 
- libmicrohttpd-dev
- libssl-dev
- gdb
- build-essential
- man-db --> Not necessary, but nice to have
- manpages-dev --> Not necessary, but nice to have
- glibc-doc --> Not necessary, but nice to have
- xxd --> Not necessary, but nice to have
