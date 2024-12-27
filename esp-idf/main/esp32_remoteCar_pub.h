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
extern const struct ble_gatt_svc_def new_ble_svc_gatt_defs[];
extern void ble_spp_server_advertise(void);
extern int ble_spp_server_gap_event(struct ble_gap_event *event, void *arg);
extern bool conn_handle_subs[CONFIG_BT_NIMBLE_MAX_CONNECTIONS + 1];
extern uint16_t ble_spp_svc_gatt_receive_val_handle;

#endif /* ESP32_REMOTECAR_PUB_H */