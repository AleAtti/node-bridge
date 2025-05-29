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
 * @property {{version: string, hostname: string}}general
 * @property {{port: number, host: string}} webserver
 * @property {{mode: string, port: number, host: string}} tcp
 * @property {{use_interval: number, interval_ms: number}} forwarder
 * @property {{ssid: string, password: string, static_ip: string, gateway: string, dns: string}} wifi
 * @property {{static_ip: string, gateway: string, dns: string}} lan
 * @property {{use_com: number, port: string, baudrate: number, databits: number, stopbits: number, parity: string}} usb_com
 * @property {{use_hid: number, vid: string, pid: string, endpoint: number, keyboard_layout: string}} usb_hid
 */
// General section
let generalHostname = document.getElementById('generalHostname');

// Webserver section
let webserverPort = document.getElementById('webserverPort');
let webserverHost = document.getElementById('webserverHost');

// tcp mode 
let tcpMode = document.getElementById("tcpMode");
let tcpPort = document.getElementById('tcpPort');
let tcpHost = document.getElementById('tcpHost');

// Forward section
let forwardIntervalCheckbox = document.getElementById("fwdUseIntefvalCheckbox");
let forwardIntervalTimeout = document.getElementById("fwdIntervalMs");

// WiFi fields
let wifiSsid = document.getElementById('wifiSsid');
let wifiPassword = document.getElementById('wifiPassword');
let wifiIp = document.getElementById('wifiIp');
let wifiGateway = document.getElementById('wifiGateway');
let wifiDns = document.getElementById('wifiDns');
let wifDhcpCheckbox = document.getElementById("wifDhcpCheckbox");
let wifiStaticSettings = document.getElementById("wifiStaticSettings");

// LAN fields
let lanIp = document.getElementById('lanIp');
let lanGateway = document.getElementById('lanGateway');
let lanDns = document.getElementById('lanDns');

// USB COM fields
let usbComCheckBox = document.getElementById("usbComCheckbox");
let usbComPort = document.getElementById('usbComPort');
let usbComBaud = document.getElementById('usbComBaud');
let usbComDataBits = document.getElementById('usbComDataBits');
let usbComStopBits = document.getElementById('usbComStopBits');
let usbComParity = document.getElementById('usbComParity');

// USB HID fields
let usbHidCheckbox = document.getElementById("usbHidCheckbox");
let usbHidVid = document.getElementById('usbHidVid');
let usbHidPid = document.getElementById('usbHidPid');
let usbHidEndpoint = document.getElementById('usbHidEndpoint');
let usbHidKeyboardLayout = document.getElementById('usbHidKeyboardLayout');

let configTextArea = document.getElementById('configText');

/**
 * Fetches the configuration file and populates the form fields.
 * @param {string} filename
 * @returns {Promise<Config>}
 * @throws {Error} If the fetch fails or the response is not valid JSON
 * @throws {Error} If the response is not ok
 */
window.addEventListener('DOMContentLoaded', () => {
  getConfig();
  isIntervalChecked();
  isDhcpChecked();

});




// Click events

function isIntervalChecked() {
  if (forwardIntervalCheckbox.checked) {
    forwardIntervalTimeout.disabled = false;
  } else {
    forwardIntervalTimeout.disabled = true;
  }
}

function isDhcpChecked() {
  if (wifDhcpCheckbox.checked) {
    wifiStaticSettings.style.display = "none";
    wifiIp.disabled = true;
    wifiGateway.disabled = true;
    wifiDns.disabled = true;
  } else {
    wifiStaticSettings.style.display = "block";
    wifiIp.disabled = false;
    wifiGateway.disabled = false;
    wifiDns.disabled = false;
  }
}


function rebootButtonClick() {
  if (confirm("Are you sure you want to reboot the device?")) {
    postReboot();

  }
};

function saveButtonClick() {
  /** @type {Config} */
  const config = {
    general: {
      hostname: generalHostname.value || 'Node_bridge',
      version: "1.0.0",
    },
    webserver: {
      port: parseInt(webserverPort.value),
      host: webserverHost.value
    },
    tcp: {
      mode: tcpMode.value,
      port: parseInt(tcpPort.value),
      host: tcpHost.value
    },
    forwarder: {
      use_interval: +forwardIntervalCheckbox.checked,
      interval_ms: parseInt(forwardIntervalTimeout.value)
    },
    wifi: {
      ssid: wifiSsid.value,
      password: wifiPassword.value,
      static_ip: wifiIp.value,
      gateway: wifiGateway.value,
      dns: wifiDns.value,
      use_dhcp: +wifDhcpCheckbox.checked
    },
    lan: {
      static_ip: lanIp.value,
      gateway: lanGateway.value,
      dns: lanDns.value
    },
    usb_com: {
      use_com: +usbComCheckBox.checked,
      port: usbComPort.value,
      baudrate: parseInt(usbComBaud.value),
      databits: parseInt(usbComDataBits.value),
      stopbits: parseInt(usbComStopBits.value),
      parity: usbComParity.value
    },
    usb_hid: {
      use_hid: +usbHidCheckbox.checked,
      vid: usbHidVid.value,
      pid: usbHidPid.value,
      endpoint: parseInt(usbHidEndpoint.value),
      keyboard_layout: usbHidKeyboardLayout.value
    }
  };
  postConfig(config);
  location.reload();
}

// Fetch Routes
function getConfig() {
  fetch("/getConfig")
    .then(response => {
      if (!response.ok) throw new Error('Network response was not ok');
      return response.json();
    })
    .then(config => {
      // Fill form fields from config

      generalHostname.value = config.general?.hostname || 'Node_bridge';
      webserverPort.value = config.webserver?.port || '';
      webserverHost.value = config.webserver?.host || '';


      // TCP fields
      tcpMode.value = config.tcp?.mode || '';
      tcpPort.value = config.tcp?.port || '';
      tcpHost.value = config.tcp?.host || '';

      // Forward
      forwardIntervalCheckbox.checked = !!config.forwarder?.use_interval;
      forwardIntervalTimeout.value = config.forwarder?.interval_ms || '';

      // WiFi fields
      wifiSsid.value = config.wifi?.ssid || '';
      wifiPassword.value = config.wifi?.password || '';
      wifiIp.value = config.wifi?.static_ip || '';
      wifiGateway.value = config.wifi?.gateway || '';
      wifiDns.value = config.wifi?.dns || '';
      wifDhcpCheckbox.checked = !!config.wifi?.use_dhcp;

      // LAN fields
      lanIp.value = config.lan?.static_ip || '';
      lanGateway.value = config.lan?.gateway || '';
      lanDns.value = config.lan?.dns || '';

      // USB COM fields
      usbComCheckBox.checked = !!config.usb_com?.use_com;
      usbComPort.value = config.usb_com?.port || '';
      usbComBaud.value = config.usb_com?.baudrate || '';
      usbComDataBits.value = config.usb_com?.databits || '';
      usbComStopBits.value = config.usb_com?.stopbits || '';
      usbComParity.value = config.usb_com?.parity || '';

      // USB HID fields
      usbHidCheckbox.checked = !!config.usb_hid?.use_hid;
      usbHidVid.value = config.usb_hid?.vid || '';
      usbHidPid.value = config.usb_hid?.pid || '';
      usbHidEndpoint.value = config.usb_hid?.endpoint ?? '';
      usbHidKeyboardLayout.value = config.usb_hid?.keyboard_layout || '';
      configTextArea.value = JSON.stringify(config, null, 2);

    })
    .catch(error => {
      console.error('Error loading config:', error);
      alert('Failed to load configuration. Please check your connection or config file.');
    });
}

/**
   * Save config to server
   * @param {string} filename
   * @param {Config} config
   * @returns {Promise<void>}
   * @throws {Error} If the fetch fails or the response is not valid JSON
   * @throws {Error} If the response is not ok
   * @throws {Error} If the config is not valid
   */
function postConfig(config) {
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
      return response.text();
    })
    .then(data => {
      alert('Configuration saved successfully!');
    })
    .catch(error => {
      console.error('Error saving config:', error);
      alert('Failed to save configuration. Please check your connection or config file.');
    });

};

function postReboot() {
  fetch("/reboot", {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    }
  })
    .then(response => {
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      return response.json();
    })
    .then(data => {
      alert('Reboot command sent successfully!');
    })
    .catch(error => {
      console.error('Error sending reboot command:', error);
      alert('Failed to send reboot command. Please check your connection.');
    });
}

