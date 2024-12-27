/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp32_remoteCar_inf.h"
#include "esp32_remoteCar_priv.h"

/**
 * Utility function to log an array of bytes.
 */
void print_bytes(const uint8_t *bytes, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        MODLOG_DFLT(DEBUG, "%s0x%02x", i != 0 ? ":" : "", bytes[i]);
    }
}

void print_mbuf(const struct os_mbuf *om)
{
    int colon;

    colon = 0;
    while (om != NULL) {
        if (colon) {
            MODLOG_DFLT(DEBUG, ":");
        } else {
            colon = 1;
        }
        print_bytes(om->om_data, om->om_len);
        om = SLIST_NEXT(om, om_next);
    }
}

/* Callback function for custom service */
static int  ble_svc_gatt_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        MODLOG_DFLT(INFO, "Callback for read");
        break;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        MODLOG_DFLT(INFO, "Data received in write event,ble_gatt_access_ctxt");
        print_mbuf(ctxt->om);
        break;

    default:
        MODLOG_DFLT(INFO, "\nDefault Callback");
        break;
    }
    return 0;
}

/* Define new custom service */
const struct ble_gatt_svc_def new_ble_svc_gatt_defs[] = {
    {
        /*** Service: SPP */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID128_DECLARE(BLE_SVC_SPP_UUID128),
        .characteristics = (struct ble_gatt_chr_def[])
        { 
            {
                /* Support SPP service */
                .uuid = BLE_UUID128_DECLARE(BLE_SVC_SPP_CHR_RX_UUID128),
                .access_cb = ble_svc_gatt_handler,
                .val_handle = &ble_spp_svc_gatt_receive_val_handle,
                .flags = BLE_GATT_CHR_F_WRITE,
            }, 
            {
                /* Support SPP service */
                .uuid = BLE_UUID128_DECLARE(BLE_SVC_SPP_CHR_TX_UUID128),
                .access_cb = ble_svc_gatt_handler,
                .val_handle = &ble_spp_svc_gatt_transmit_val_handle,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            }, 
            {
                0, /* No more characteristics */
            }
        },
    },
    {
        0, /* No more services. */
    },
};