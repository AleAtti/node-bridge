const filename = "/config.json";
let config = null;

// const json = '{"usb_com":true, "count":42}';
// const config = JSON.parse(json);

const configForm = document.getElementById('configForm');
const configText = document.getElementById('configText');

// General section

let usbComCheckBox = document.getElementById('usbComCheckbox');

// script.js

window.addEventListener('DOMContentLoaded', () => {
  fetch(filename)
    .then(response => response.json())
    .then(config => {
      // Fill form fields from config
      usbComCheckBox.checked = !!config.use_com;
      document.getElementById('usbHidCheckbox').checked = !!config.use_hid;
      document.getElementById('tcpMode').value = config.tcp_mode || '';
      document.getElementById('tcpPort').value = config.tcp_port || '';
      document.getElementById('tcpHost').value = config.tcp_host || '';

      document.getElementById('wifiSsid').value = config.wifi?.ssid || '';
      document.getElementById('wifiPassword').value = config.wifi?.password || '';
      document.getElementById('wifiIp').value = config.wifi?.static_ip || '';
      document.getElementById('wifiGateway').value = config.wifi?.gateway || '';
      document.getElementById('wifiDns').value = config.wifi?.dns || '';

      document.getElementById('lanIp').value = config.lan?.static_ip || '';
      document.getElementById('lanGateway').value = config.lan?.gateway || '';
      document.getElementById('lanDns').value = config.lan?.dns || '';

      document.getElementById('usbComPort').value = config.usb_com?.port || '';
      document.getElementById('usbComBaud').value = config.usb_com?.baudrate || '';
      document.getElementById('usbComDataBits').value = config.usb_com?.databits || '';
      document.getElementById('usbComStopBits').value = config.usb_com?.stopbits || '';
      document.getElementById('usbComParity').value = config.usb_com?.parity || '';

      document.getElementById('usbHidVid').value = config.usb_hid?.vid || '';
      document.getElementById('usbHidPid').value = config.usb_hid?.pid || '';
      document.getElementById('usbHidEndpoint').value = config.usb_hid?.endpoint || '';

      document.getElementById('configText').value = JSON.stringify(config, null, 2);
    });
});

