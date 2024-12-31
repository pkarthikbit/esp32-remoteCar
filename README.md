## Folder contents

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── *.c
└── README.md           => file you are currently reading
└── kpl_build.sh        => shell file to build the project. Menu config is commented out as there is a sdkconfig.defaults available.
└── kpl_flash.sh        => shell file to flash the controller ESP32-C6
└── kpl_monitor.sh      => shell file to monitor the printf. To exit the serial monitor, type ``Ctrl-]``.
```

# esp32-remoteCar

In Bluetooth classic (BR/EDR) systems, a Serial Port Profile (SPP) is an adopted profile defined by the Bluetooth Special Interest Group (SIG) used to emulate a serial port connection over a Bluetooth wireless connection. For BLE systems, an adopted SPP profile over BLE is not defined, thus emulation of a serial port must be implemented as a vendor-specific custom profile.

This reference design consists of two Demos, the BLE SPP server and BLE SPP client that run on their respective endpoints. In this design, BLE SPP server is taken as base.

These devices connect and exchange data wirelessly with each other. This capability creates a virtual serial link over the air. Each byte input can be sent and received by both the server and client. Espressif designed the BLE SPP applications to use the UART transport layer but you could adapt this design to work with other serial protocols, such as SPI.

This module is a BLE Serial Port Profile (SPP) server using the NimBLE stack. BLE SPP enables wireless serial communication, making it valuable for IoT and embedded applications. It allows the ESP32 to communicate with other BLE devices, handling connection management, advertising, and data transmission. The code also defines a custom GATT service and initializes UART communication for data transfer.

### Initialization

Both the server and client will first initialize the UART and BLE. The server demo will set up the serial port service with standard GATT and GAP services in the attribute server. The client demo will scan the BLE broadcast over the air to find the SPP server.

### Packet Structure

After the Uart received data, the data will be posted to Uart task. Then, in the UART_DATA event, the raw data may be retrieved. The max length is 120 bytes every time.
If you run the BLE SPP demo with two ESP32 chips, the MTU size will be exchanged for 200 bytes after the ble connection is established, so every packet can be send directly.
If you only run the ble_spp_server demo, and it was connected by a phone, the MTU size may be less than 123 bytes. In such a case the data will be split into fragments and send in turn.
In every packet, we add 4 bytes to indicate that this is a fragment packet. The first two bytes contain "##" if this is a fragment packet, the third byte is the total number of the packets, the fourth byte is the current number of this packet.
The phone APP need to check the structure of the packet if it want to communicate with the ble_spp_server demo.

### Sending Data Wirelessly

The client will be sending WriteNoRsp packets to the server. The server side sends data through notifications. When the Uart receives data, the Uart task places it in the buffer.

### Receiving Data Wirelessly

    The server will receive this data in the BLE_GATT_ACCESS_OP_WRITE_CHR event.

### GATT Server Attribute Table (example)

characteristic|UUID|Permissions
:-:|:-:|:-:
SPP_DATA_RECV_CHAR|0xABF1|READ&WRITE_NR
SPP_DATA_NOTIFY_CHAR|0xABF2|READ&NOTIFY
SPP_COMMAND_CHAR|0xABF3|READ&WRITE_NR
SPP_STATUS_CHAR|0xABF4|READ & NOTIFY

This example creates GATT server and advertises data, it then gets connected to a central device.

It takes input from user and performs notify GATT operations against the specified peer.
