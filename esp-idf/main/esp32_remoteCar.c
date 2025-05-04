/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

 #include "esp32_remoteCar_inf.h"
 #include "esp32_remoteCar_priv.h"
 
 /******************* Variable declaration/ definition ********************/
 bool conn_handle_subs[CONFIG_BT_NIMBLE_MAX_CONNECTIONS + 1];
 uint16_t ble_spp_svc_gatt_receive_val_handle;
 uint16_t ble_spp_svc_gatt_transmit_val_handle;
 
 QueueHandle_t spp_common_uart_queue = NULL;
 
 /******************* Function declaration ********************/
 void ble_store_config_init(void);
 
 /******************* Function definition ********************/
 /*
  * The function `ble_server_uart_task` serves as a BLE server with UART communication capabilities. Here's a summary of what this code does:
  * 1. The task starts and logs a message to indicate that the BLE server UART task has begun. It enters an infinite loop (`for (;;)`) to continuously process UART events.
  * 2. Inside the loop, it waits for a UART event to occur using the `xQueueReceive` function. When a UART event is received, it checks the type of event.
  * 3. If the event type is `UART_DATA`, it reads the data from the UART and stores it in a dynamically allocated buffer (`ntf`). 
  *    The buffer is then used to send notifications to connected BLE clients.
  * 4. It iterates through a loop for each possible BLE connection (up to `CONFIG_BT_NIMBLE_MAX_CONNECTIONS` connections).
  * 5. For each connection, it checks if a client has subscribed to notifications (`conn_handle_subs[i]`).
  * 6. If a client has subscribed, it prepares a BLE notification message (`txom`) containing the UART data and sends it to the client using the `ble_gatts_notify_custom` function.
  * 7. It logs whether the notification was sent successfully or if there was an error. The loop continues processing UART events, and the task repeats.
  * 8. Finally, the task is deleted using `vTaskDelete(NULL)` when it is no longer needed.
  */
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
                             rc = ble_gatts_notify_custom(i, ble_spp_svc_gatt_receive_val_handle,
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
 
     //zero-initialize the config structure.
     gpio_config_t io_conf = {};
 
     /* Initialize NVS — it is used to store PHY calibration data 
      * In a Bluetooth Low Energy (BLE) device, cryptographic keys used for encryption and authentication are often stored in Non-Volatile Storage (NVS). BLE stores the peer keys, CCCD keys, peer records, etc on NVS. By storing these keys in NVS, the BLE device can quickly retrieve them when needed, without the need for time-consuming key generations.
      */
     ret = nvs_flash_init();
 
     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
     {
         ret = nvs_flash_erase();
         ESP_ERROR_CHECK(ret);
 
         ret = nvs_flash_init();
         ESP_ERROR_CHECK(ret);
     }
 
     /* The main function calls `nimble_port_init()` to initialize BT Controller and nimble stack. 
      * This function initializes the BT controller by first creating its configuration structure 
      * named `esp_bt_controller_config_t` with default settings generated by the `BT_CONTROLLER_INIT_CONFIG_DEFAULT()` macro. 
      * It implements the Host Controller Interface (HCI) on the controller side, the Link Layer (LL), and the Physical Layer (PHY). 
      * The BT Controller is invisible to the user applications and deals with the lower layers of the BLE stack. 
      * The controller configuration includes setting the BT controller stack size, priority, and HCI baud rate. 
      * With the settings created, the BT controller is initialized and enabled with the `esp_bt_controller_init()` and `esp_bt_controller_enable()` functions.
      * 
      * The controller should be enabled in `ESP_BT_MODE_BLE` if you want to use the BLE mode. 
      * There are four Bluetooth modes supported:
      * 1. `ESP_BT_MODE_IDLE`: Bluetooth not running
      * 2. `ESP_BT_MODE_BLE`: BLE mode
      * 3. `ESP_BT_MODE_CLASSIC_BT`: BT Classic mode
      * 4. `ESP_BT_MODE_BTDM`: Dual mode (BLE + BT Classic)
      * After the initialization of the BT controller, the nimble stack, which includes the common definitions and 
      * APIs for BLE, is initialized by using `esp_nimble_init()`.
     */
     ret = nimble_port_init();
     if (ret != ESP_OK) 
     {
         MODLOG_DFLT(ERROR, "Failed to init nimble %d \n", ret);
         return;
     }
 
     /* Initialize connection_handle array `conn_handle_subs`.
      * It is being initialized for the purpose of managing Bluetooth connections. 
      * The for loop iterates through a range of values, setting each element of the array to false. 
      * This action signifies that there are no active connections at the initialization stage.
      */
     for (int i = 0; i <= CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) 
     {
         conn_handle_subs[i] = false;
     }
 
     /* Initialize uart driver and start uart task.
      * It is is responsible for initializing the UART (Universal Asynchronous Receiver-Transmitter) 
      * communication for the BLE SPP server application. */
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
     /* Install UART driver, and get the queue.
      * `uart_driver_install` installs the UART driver, allocating memory for the UART communication buffers and 
      * creating a queue (`spp_common_uart_queue`) to manage UART events.
     */
     uart_driver_install(UART_NUM_0, 4096, 8192, 10, &spp_common_uart_queue, 0);
     /* Set UART parameters
      * `uart_param_config` sets the UART parameters based on the configuration structure defined earlier.
      */
     uart_param_config(UART_NUM_0, &uart_config);
     /* `uart_set_pin` configures the UART pins for communication. In this case, it uses default pin configurations. */
     uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
     /* creates a FreeRTOS task (`ble_server_uart_task`) that will handle UART communication. This task is responsible for sending and receiving data over UART. */
     xTaskCreate(ble_server_uart_task, "uTask", 4096, (void *)UART_NUM_0, 8, NULL);
 
     /* Initialize the NimBLE host configuration. 
      * The host is configured by setting up the callbacks for 
      * Stack-reset, Stack-sync, registration of each GATT resource (service, characteristic, or descriptor),
      * and storage status.*/
     ble_hs_cfg.reset_cb = ble_spp_server_on_reset;
     ble_hs_cfg.sync_cb = ble_spp_server_on_sync;
     ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
     ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
 
     /* Security Manager (sm_ members) is configurable at runtime to simplify security testing. 
      * Defaults for those are configured by selecting proper options via menuconfig for example configurations. */
     ble_hs_cfg.sm_io_cap = CONFIG_EXAMPLE_IO_TYPE;        //default value 3
 
 #ifdef CONFIG_EXAMPLE_BONDING
     /* `CONFIG_EXAMPLE_BONDING` is set to enable the bonding on BLE SPP server. 
      * By default this flag is disabled. It can be configured through the sdkconfig file or from the menuconfig options. */
     ble_hs_cfg.sm_bonding = 1;
 #endif
 #ifdef CONFIG_EXAMPLE_MITM
     /* When the `CONFIG_EXAMPLE_MITM` flag is set during pairing, 
      * it will enable the MITM bit in the auth req field of pairing request, 
      * which in turn necessitates protection against Man-In-The-Middle attacks during the pairing process. 
      * The flag can be configured through sdkconfig or menuconfig as mentioned above. */
     ble_hs_cfg.sm_mitm = 1;
 #endif
 
     /* When the `CONFIG_EXAMPLE_USE_SC` flag is set in the Pairing Request/Response, 
      * it enables the usage of LE Secure Connections for pairing, provided that the remote device also supports it. 
      * If the remote device does not support LE Secure Connections, the pairing process falls back to using legacy pairing.
      * So, the main function configures security features in a BLE application like Bonding, MITM protection, Secure connections, and Key distribution. */
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
     /* The gatt_svr_init function is called during the initialization phase of a BLE application 
      * to set up the GATT server and define the services and characteristics it supports. */
     ble_svc_gatt_init();
 
     rc = ble_gatts_count_cfg(new_ble_svc_gatt_defs);
     assert(rc == 0);
 
     rc = ble_gatts_add_svcs(new_ble_svc_gatt_defs);
     assert(rc == 0);
 
     /* Set the default device name. */
     rc = ble_svc_gap_device_name_set("nimble-ble-spp-svr");
     assert(rc == 0);
 
     /* The main function calls `ble_store_config_init()` to configure the host 
      * by setting up the storage callbacks which handle the read, write, and 
      * deletion of security material. */
     ble_store_config_init();
 
     /* GPIO configuration */
     //disable interrupt
     io_conf.intr_type = GPIO_INTR_DISABLE;
     //set as output mode
     io_conf.mode = GPIO_MODE_OUTPUT;
     //bit mask of the pins that you want to set
     io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
     //disable pull-down mode
     io_conf.pull_down_en = 0;
     //disable pull-up mode
     io_conf.pull_up_en = 0;
     //configure GPIO with the given settings
     gpio_config(&io_conf);
 
     /* The main function creates a task where nimble will run using `nimble_port_freertos_init()`. 
      * This enables the nimble stack by using `esp_nimble_enable()`. */
     nimble_port_freertos_init(ble_spp_server_host_task);
 }
 
void app_main(void)
{
    esp32_remoteCar_Init();
}
 