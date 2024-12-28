/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp32_remoteCar_inf.h"
#include "esp32_remoteCar_priv.h"

static uint8_t gatt_svr_static_val[50];

#define PI 3.14159

//Byte 2
#define GAMEPAD_DIGITAL 0x01
#define GAMEPAD_ANALOG  0x02
#define GAMEPAD_ACCL    0x03

//Byte 5
#define START_KEY       0x1
#define SELECT_KEY      0x2
#define TRIANGLE_KEY    0x4 
#define CIRCLE_KEY      0x8
#define CROSS_KEY       0x10
#define SQUARE_KEY      0x20

//Byte 6 in case of Digital Mode GamePad
#define UP_KEY          0x1
#define DOWN_KEY        0x2
#define LEFT_KEY        0x4
#define RIGHT_KEY       0x8

/**
 * Utility function to log an array of bytes.
 */
static int
gatt_svr_chr_write(uint16_t conn_handle, uint16_t attr_handle,
                   struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
                   void *dst, uint16_t *len)
{
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

/* Callback function for custom service */
static int  ble_svc_gatt_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    static int rc;
    uint16_t act_len;
    uint16_t angle;
    uint8_t radius;
    float x_value;
    float y_value;

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) 
    {
        rc = gatt_svr_chr_write(conn_handle, attr_handle,
                                ctxt->om, 0,
                                sizeof gatt_svr_static_val,
                                &gatt_svr_static_val, &act_len);
        
        switch(gatt_svr_static_val[2])
        {
            case GAMEPAD_DIGITAL:
            {
                switch(gatt_svr_static_val[6])
                {
                    case UP_KEY:
                        MODLOG_DFLT(INFO, "UP_KEY");
                        break;

                    case DOWN_KEY:
                        MODLOG_DFLT(INFO, "DOWN_KEY");
                        break;

                    case LEFT_KEY:
                        MODLOG_DFLT(INFO, "LEFT_KEY");
                        break;

                    case RIGHT_KEY:
                        MODLOG_DFLT(INFO, "RIGHT_KEY");
                        break;

                    default:
                        break;
                }
            }
            break;

            case GAMEPAD_ANALOG:
            case GAMEPAD_ACCL:
            {
                angle =((gatt_svr_static_val[6] >> 3)*15);
                radius = gatt_svr_static_val[6] & 0x07;
                x_value = (float)(radius*((float)(cos((float)(angle*PI/180)))));
                y_value = (float)(radius*((float)(sin((float)(angle*PI/180)))));

                MODLOG_DFLT(INFO, "x=%f, y=%f", x_value, y_value);
            }
            break;

            default:
                break;

        }

        switch(gatt_svr_static_val[5])
        {
            case START_KEY:
                MODLOG_DFLT(INFO, "START_KEY");
                break;

            case SELECT_KEY:
                MODLOG_DFLT(INFO, "SELECT_KEY");
                break;

            case TRIANGLE_KEY:
                MODLOG_DFLT(INFO, "TRIANGLE_KEY");
                break;

            case CIRCLE_KEY:
                MODLOG_DFLT(INFO, "CIRCLE_KEY");
                break;

            case CROSS_KEY:
                MODLOG_DFLT(INFO, "CROSS_KEY");
                break;    

            case SQUARE_KEY:
                MODLOG_DFLT(INFO, "SQUARE_KEY");
                break;  

            default:
                break;      
        }
       
        return rc;
    }
    else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) 
    {
        gatt_svr_static_val[0] = rand();
        rc = os_mbuf_append(ctxt->om, &gatt_svr_static_val,
                            sizeof gatt_svr_static_val);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
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