/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp32_remoteCar_inf.h"
#include "esp32_remoteCar_priv.h"

/******************* Variable declaration/ definition ********************/
bool conn_handle_subs[CONFIG_BT_NIMBLE_MAX_CONNECTIONS + 1];
uint16_t ble_spp_svc_gatt_read_val_handle;

QueueHandle_t spp_common_uart_queue = NULL;

/******************* Function declaration ********************/
void ble_store_config_init(void);

/******************* Function definition ********************/
void ble_server_uart_task(void *pvParameters)
{
    MODLOG_DFLT(INFO, "BLE server UART_task started\n");
    uart_event_t event;
    int rc = 0;
    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(spp_common_uart_queue, (void * )&event, (TickType_t)portMAX_DELAY))            {
            switch (event.type) {
            //Event of UART receiving data
            case UART_DATA:
                if (event.size) {
                    uint8_t *ntf;
                    ntf = (uint8_t *)malloc(sizeof(uint8_t) * event.size);
                    memset(ntf, 0x00, event.size);
                    uart_read_bytes(UART_NUM_0, ntf, event.size, portMAX_DELAY);

                    for (int i = 0; i <= CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
                        /* Check if client has subscribed to notifications */
                        if (conn_handle_subs[i]) {
                            struct os_mbuf *txom;
                            txom = ble_hs_mbuf_from_flat(ntf, event.size);
                            rc = ble_gatts_notify_custom(i, ble_spp_svc_gatt_read_val_handle,
                                                         txom);
                            if (rc == 0) {
                                MODLOG_DFLT(INFO, "Notification sent successfully");
                            } else {
                                MODLOG_DFLT(INFO, "Error in sending notification rc = %d", rc);
                            }
                        }
                    }

		    free(ntf);
                }
                break;
            default:
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

void ble_spp_server_host_task(void *param)
{
    MODLOG_DFLT(INFO, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

static void esp32_remoteCar_Init()
{
    int rc;
    esp_err_t ret;

    /* Initialize NVS — it is used to store PHY calibration data */
    ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ret = nvs_flash_erase();
        ESP_ERROR_CHECK(ret);

        ret = nvs_flash_init();
        ESP_ERROR_CHECK(ret);
    }

    ret = nimble_port_init();
    if (ret != ESP_OK) 
    {
        MODLOG_DFLT(ERROR, "Failed to init nimble %d \n", ret);
        return;
    }

    /* Initialize connection_handle array */
    for (int i = 0; i <= CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) 
    {
        conn_handle_subs[i] = false;
    }

    /* Initialize uart driver and start uart task */
    uart_config_t uart_config = 
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };
    //Install UART driver, and get the queue.
    uart_driver_install(UART_NUM_0, 4096, 8192, 10, &spp_common_uart_queue, 0);
    //Set UART parameters
    uart_param_config(UART_NUM_0, &uart_config);
    //Set UART pins
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    xTaskCreate(ble_server_uart_task, "uTask", 4096, (void *)UART_NUM_0, 8, NULL);

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = ble_spp_server_on_reset;
    ble_hs_cfg.sync_cb = ble_spp_server_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_hs_cfg.sm_io_cap = CONFIG_EXAMPLE_IO_TYPE;
#ifdef CONFIG_EXAMPLE_BONDING
    ble_hs_cfg.sm_bonding = 1;
#endif
#ifdef CONFIG_EXAMPLE_MITM
    ble_hs_cfg.sm_mitm = 1;
#endif
#ifdef CONFIG_EXAMPLE_USE_SC
    ble_hs_cfg.sm_sc = 1;
#else
    ble_hs_cfg.sm_sc = 0;
#endif
#ifdef CONFIG_EXAMPLE_BONDING
    ble_hs_cfg.sm_our_key_dist = 1;
    ble_hs_cfg.sm_their_key_dist = 1;
#endif

    /* Register custom service */
    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(new_ble_svc_gatt_defs);
    assert(rc == 0);

    rc = ble_gatts_add_svcs(new_ble_svc_gatt_defs);
    assert(rc == 0);

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("nimble-ble-spp-svr");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(ble_spp_server_host_task);
}

void app_main(void)
{
    esp32_remoteCar_Init();
}
