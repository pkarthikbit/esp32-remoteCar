/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef ESP32_REMOTECAR_H
#define ESP32_REMOTECAR_H

extern uint8_t ext_adv_pattern_1[26];
extern int  esp32_remoteCar_gatt_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
extern const struct ble_gatt_svc_def esp32_remoteCar_gatt_def[];
extern void gpio_init();

#endif /* ESP32_REMOTECAR_H */
