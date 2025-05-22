/**
 * Resource:  https://jsdoc.app/
 *            https://developer.mozilla.org/en-US/docs/Web/API/Document/DOMContentLoaded_event
 *            https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API
 *            https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch
 * @license   MIT
 *            
 */

let config = null;
const configForm = document.getElementById("configForm");

/**
 * Element IDs
 * @typedef {Object} Config
 * @property {{use_com: number, use_hid: number, version: string, hostname: string}}general
 * @property {{port: number, host: string}} webserver
 * @property {{mode: string, port: number, host: string}} tcp
 * @property {{ssid: string, password: string, static_ip: string, gateway: string, dns: string}} wifi
 * @property {{static_ip: string, gateway: string, dns: string}} lan
 * @property {{port: string, baudrate: number, databits: number, stopbits: number, parity: string}} usb_com
 * @property {{vid: number, pid: number, endpoint: number}} usb_hid
 */
// General section
let usbComCheckBox = document.getElementById("usbComCheckbox");
let usbHidCheckbox = document.getElementById("usbHidCheckbox");

// tcp mode 
let tcpMode = document.getElementById("tcpMode");
let tcpPort = document.getElementById('tcpPort');
let tcpHost = document.getElementById('tcpHost');

// WiFi fields
let wifiSsid = document.getElementById('wifiSsid');
let wifiPassword = document.getElementById('wifiPassword');
let wifiIp = document.getElementById('wifiIp');
let wifiGateway = document.getElementById('wifiGateway');
let wifiDns = document.getElementById('wifiDns');

// LAN fields
let lanIp = document.getElementById('lanIp');
let lanGateway = document.getElementById('lanGateway');
let lanDns = document.getElementById('lanDns');

// USB COM fields
let usbComPort = document.getElementById('usbComPort');
let usbComBaud = document.getElementById('usbComBaud');
let usbComDataBits = document.getElementById('usbComDataBits');
let usbComStopBits = document.getElementById('usbComStopBits');
let usbComParity = document.getElementById('usbComParity');

// USB HID fields
let usbHidVid = document.getElementById('usbHidVid');
let usbHidPid = document.getElementById('usbHidPid');
let usbHidEndpoint = document.getElementById('usbHidEndpoint');

let configTextArea = document.getElementById('configText');

/**
 * Fetches the configuration file and populates the form fields.
 * @param {string} filename
 * @returns {Promise<Config>}
 * @throws {Error} If the fetch fails or the response is not valid JSON
 * @throws {Error} If the response is not ok
 */
window.addEventListener('DOMContentLoaded', () => {
  fetch("/getConfig")
    .then(response => {
      if (!response.ok) throw new Error('Network response was not ok');
      return response.json();
    })
    .then(config => {
      console.log('Config loaded:', config.General);
      // Fill form fields from config
      usbComCheckBox.checked = !!config.general?.use_com;
      usbHidCheckbox.checked = !!config.general?.use_hid;
      tcpMode.value = config.tcp?.mode || '';
      tcpPort.value = config.tcp?.port || '';
      tcpHost.value = config.tcp?.host || '';

      // WiFi fields
      wifiSsid.value = config.wifi?.ssid || '';
      wifiPassword.value = config.wifi?.password || '';
      wifiIp.value = config.wifi?.static_ip || '';
      wifiGateway.value = config.wifi?.gateway || '';
      wifiDns.value = config.wifi?.dns || '';

      // LAN fields
      lanIp.value = config.lan?.static_ip || '';
      lanGateway.value = config.lan?.gateway || '';
      lanDns.value = config.lan?.dns || '';

      // USB COM fields
      usbComPort.value = config.usb_com?.port || '';
      usbComBaud.value = config.usb_com?.baudrate || '';
      usbComDataBits.value = config.usb_com?.databits || '';
      usbComStopBits.value = config.usb_com?.stopbits || '';
      usbComParity.value = config.usb_com?.parity || '';

      // USB HID fields
      usbHidVid.value = config.usb_hid?.vid || '';
      usbHidPid.value = config.usb_hid?.pid || '';
      usbHidEndpoint.value = config.usb_hid?.endpoint || '';

      configTextArea.value = JSON.stringify(config, null, 2);
    })
    .catch(error => {
      console.error('Error loading config:', error);
      alert('Failed to load configuration. Please check your connection or config file.');
    });
});

/**
 * Handle form submission to save config
 * @param {Event} event
 * @returns {void}
 * @throws {Error} If the fetch fails or the response is not valid JSON
 * @throws {Error} If the response is not ok
 * @throws {Error} If the config is not valid
 *  */
configForm.addEventListener('submit', function (event) {
  event.preventDefault();

  /** @type {Config} */
  const config = {
    general: {
      hostname: "Node_bridge",
      version: "1.0.0",
      use_com: usbComCheckBox.checked,
      use_hid: usbHidCheckbox.checked
    },
    webserver: {
      port: parseInt(tcpPort.value),
      host: tcpHost.value
    },
    tcp: {
      mode: tcpMode.value,
      port: parseInt(tcpPort.value),
      host: tcpHost.value
    },
    wifi: {
      ssid: wifiSsid.value,
      password: wifiPassword.value,
      static_ip: wifiIp.value,
      gateway: wifiGateway.value,
      dns: wifiDns.value
    },
    lan: {
      static_ip: lanIp.value,
      gateway: lanGateway.value,
      dns: lanDns.value
    },
    usb_com: {
      port: usbComPort.value,
      baudrate: parseInt(usbComBaud.value),
      databits: parseInt(usbComDataBits.value),
      stopbits: parseInt(usbComStopBits.value),
      parity: usbComParity.value
    },
    usb_hid: {
      vid: parseInt(usbHidVid.value),
      pid: parseInt(usbHidPid.value),
      endpoint: parseInt(usbHidEndpoint.value)
    }
  };

  /**
   * Save config to server
   * @param {string} filename
   * @param {Config} config
   * @returns {Promise<void>}
   * @throws {Error} If the fetch fails or the response is not valid JSON
   * @throws {Error} If the response is not ok
   * @throws {Error} If the config is not valid
   */
  fetch("/setConfig", {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(config)
  })
    .then(response => {
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      return response.json();
    })
    .then(data => {
      console.log('Config saved:', config);
      alert('Configuration saved successfully!');
    })
    .catch(error => {
      console.error('Error saving config:', error);
      alert('Failed to save configuration. Please check your connection or config file.');
    });
});

