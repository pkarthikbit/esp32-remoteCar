/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef ESP32_REMOTECAR_PUB_H
#define ESP32_REMOTECAR_PUB_H

extern void ble_spp_server_on_reset(int reason);
extern void ble_spp_server_on_sync(void);
extern void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
extern const struct ble_gatt_svc_def new_ble_svc_gatt_defs;
extern int ble_spp_server_gap_event(struct ble_gap_event *event, void *arg);

#endif /* ESP32_REMOTECAR_PUB_H */