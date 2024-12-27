/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef ESP32_REMOTECAR_PRIV_H
#define ESP32_REMOTECAR_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * #define UUID_Service  "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
 * #define UUID_Transmit "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
 * #define UUID_Receive  "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
 */

/* 128 Bit SPP Service UUID */
#define BLE_SVC_SPP_UUID128                             \
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,     \
    0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E

/* 128 Bit SPP Service Characteristic UUID */
#define BLE_SVC_SPP_CHR_UUID128                         \
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,     \
    0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

#ifdef __cplusplus
}
#endif

#endif /* ESP32_REMOTECAR_PRIV_H */
