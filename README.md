# NodeBridge

**NodeBridge** is a lightweight I/O utility designed to connect USB devices—such as handheld scanners or other peripherals—to a network.
It receives input via **USB-HID** or **USB-COM** and transmits the data over **TCP/IP**, acting as a smart bridge between local hardware and remote systems.

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
├── config.json      # Main configuration file
├── Makefile         # Build system
└── README.md        # This file
```