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
 * Init function
 */
int gatt_svr_init(void)
{
    int rc;

    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_svc_ans_init();

    rc = ble_gatts_count_cfg(new_ble_svc_gatt_defs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(new_ble_svc_gatt_defs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

/**
 * Utility function to log an array of bytes.
 */
static int
gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
    void *dst, uint16_t *len)
{
    uint16_t om_len;
    int rc;

    rc = 0;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        rc = BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        rc = BLE_ATT_ERR_UNLIKELY;
    }

    return rc;
}

/* Callback function for custom service 
 * This callback function `ble_svc_gatt_handler` is responsible for handling read and write operations on characteristics within the custom service. Here is a summary of what the code does:
 * The function is defined with the following parameters:
 * - `uint16_t conn_handle`: The connection handle for the BLE connection.
 * - `uint16_t attr_handle`: The attribute handle associated with the operation.
 * - `struct ble_gatt_access_ctxt *ctxt`: A context structure containing information about the GATT (Generic Attribute Profile) access operation.
 * - `void *arg`: A generic argument pointer (not used in this code).*/
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
        rc = gatt_svr_chr_write(ctxt->om, 0,
                                sizeof gatt_svr_static_val,
                                &gatt_svr_static_val, &act_len);
        
        switch(gatt_svr_static_val[2])
        {
            case GAMEPAD_DIGITAL:
            {
                switch(gatt_svr_static_val[6])
                {
                    /* 
                     * IN1/IN3	IN2/IN4	Spinning Direction
                     * Low(0)	Low(0)	Motor OFF
                     * High(1)	Low(0)	Forward
                     * Low(0)	High(1)	Reverse
                     * High(1)	High(1)	Motor OFF
                    */
                    
                    case UP_KEY:
                        //MODLOG_DFLT(INFO, "UP_KEY");
                        gpio_set_level(GPIO_DRV8833_IN1_1, true);
                        gpio_set_level(GPIO_DRV8833_IN2_1, false);
                        gpio_set_level(GPIO_DRV8833_IN3_1, false);
                        gpio_set_level(GPIO_DRV8833_IN4_1, true);

                        gpio_set_level(GPIO_DRV8833_IN1_2, false);
                        gpio_set_level(GPIO_DRV8833_IN2_2, true);
                        gpio_set_level(GPIO_DRV8833_IN3_2, false);
                        gpio_set_level(GPIO_DRV8833_IN4_2, true);
                        break;

                    case DOWN_KEY:
                        //MODLOG_DFLT(INFO, "DOWN_KEY");
                        gpio_set_level(GPIO_DRV8833_IN1_1, false);
                        gpio_set_level(GPIO_DRV8833_IN2_1, true);
                        gpio_set_level(GPIO_DRV8833_IN3_1, true);
                        gpio_set_level(GPIO_DRV8833_IN4_1, false);

                        gpio_set_level(GPIO_DRV8833_IN1_2, true);
                        gpio_set_level(GPIO_DRV8833_IN2_2, false);
                        gpio_set_level(GPIO_DRV8833_IN3_2, true);
                        gpio_set_level(GPIO_DRV8833_IN4_2, false);
                        break;

                    case LEFT_KEY:
                        //MODLOG_DFLT(INFO, "LEFT_KEY");
                        gpio_set_level(GPIO_DRV8833_IN1_1, false);
                        gpio_set_level(GPIO_DRV8833_IN2_1, false);
                        gpio_set_level(GPIO_DRV8833_IN3_1, false);
                        gpio_set_level(GPIO_DRV8833_IN4_1, true);

                        gpio_set_level(GPIO_DRV8833_IN1_2, false);
                        gpio_set_level(GPIO_DRV8833_IN2_2, false);
                        gpio_set_level(GPIO_DRV8833_IN3_2, false);
                        gpio_set_level(GPIO_DRV8833_IN4_2, true);
                        break;

                    case RIGHT_KEY:
                        //MODLOG_DFLT(INFO, "RIGHT_KEY");
                        gpio_set_level(GPIO_DRV8833_IN1_1, true);
                        gpio_set_level(GPIO_DRV8833_IN2_1, false);
                        gpio_set_level(GPIO_DRV8833_IN3_1, false);
                        gpio_set_level(GPIO_DRV8833_IN4_1, false);

                        gpio_set_level(GPIO_DRV8833_IN1_2, false);
                        gpio_set_level(GPIO_DRV8833_IN2_2, true);
                        gpio_set_level(GPIO_DRV8833_IN3_2, false);
                        gpio_set_level(GPIO_DRV8833_IN4_2, false);
                        break;

                    default:
                        gpio_set_level(GPIO_DRV8833_IN1_1, false);
                        gpio_set_level(GPIO_DRV8833_IN2_1, false);
                        gpio_set_level(GPIO_DRV8833_IN3_1, false);
                        gpio_set_level(GPIO_DRV8833_IN4_1, false);

                        gpio_set_level(GPIO_DRV8833_IN1_2, false);
                        gpio_set_level(GPIO_DRV8833_IN2_2, false);
                        gpio_set_level(GPIO_DRV8833_IN3_2, false);
                        gpio_set_level(GPIO_DRV8833_IN4_2, false);
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

                ESP_UNUSED(x_value);
                ESP_UNUSED(y_value);

                //MODLOG_DFLT(INFO, "x=%f, y=%f", x_value, y_value);
            }
            break;

            default:
                break;

        }

        switch(gatt_svr_static_val[5])
        {
            case START_KEY:
                //MODLOG_DFLT(INFO, "START_KEY");
                break;

            case SELECT_KEY:
                //MODLOG_DFLT(INFO, "SELECT_KEY");
                break;

            case TRIANGLE_KEY:
                //MODLOG_DFLT(INFO, "TRIANGLE_KEY");
                break;

            case CIRCLE_KEY:
                //MODLOG_DFLT(INFO, "CIRCLE_KEY");
                break;

            case CROSS_KEY:
                //MODLOG_DFLT(INFO, "CROSS_KEY");
                break;    

            case SQUARE_KEY:
                //MODLOG_DFLT(INFO, "SQUARE_KEY");
                break;  

            default:
                break;      
        }
    }
    else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) 
    {
        gatt_svr_static_val[0] = rand();
        rc = os_mbuf_append(ctxt->om, &gatt_svr_static_val,
                            sizeof gatt_svr_static_val);
        rc = 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return rc;
}

/* Define new custom service 
 * The provided code defines a custom service for BLE communication. Here is a summary of what this code does:
 * 1. It defines a custom BLE service using the `ble_gatt_svc_def` structure.
 * 2. This service is marked as a primary service using `BLE_GATT_SVC_TYPE_PRIMARY`. The UUID (Universally Unique Identifier) for the service is specified as `BLE_SVC_SPP_UUID128`.
 * 3. Inside the service, there is a list of characteristics defined using the `ble_gatt_chr_def` structure. In this case, there's only one characteristic defined. The characteristic supports various operations, including reading, writing, and notifications. The UUID for the characteristic is specified as `BLE_SVC_SPP_CHR_UUID128`.
 * 4. A callback function named `ble_svc_gatt_handler` is assigned to handle access to this characteristic.
 * 5. The characteristic is set to support reading (`BLE_GATT_CHR_F_READ`), writing (`BLE_GATT_CHR_F_WRITE`), and notifications (`BLE_GATT_CHR_F_NOTIFY`).
 * 6. The `val_handle` field is used to store the handle for reading the characteristic's value.
 * In summary, this code defines a custom BLE service with one characteristic, and it specifies that this characteristic can be read, written to, and used for notifications. The UUIDs and callback functions for the service and characteristic are also specified.
 */
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