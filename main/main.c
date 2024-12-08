/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
/* inclusion - RTOS related */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/* inclusion - for logging */
#include "esp_log.h"
#include "nvs_flash.h"
/* inclusion - driver related */
#include "driver/gpio.h"
#include "driver/temperature_sensor.h"
#include "driver/uart.h"
/* Nimble includes */
#include "gap.h"

/* constant definition */
#define BLINK_GPIO  GPIO_NUM_15     
#define PEER_ADDR_VAL_SIZE      6

/* Library function declarations */
void ble_store_config_init(void);

/* Private function declarations */
static void on_stack_reset(int reason);
static void on_stack_sync(void);
static void nimble_host_config_init(void);
static void nimble_host_task(void *param);

/* Variable definition */
float tsens_value;
static const char *TAG = "esp32-remoteCar";     /* for module name logging */
temperature_sensor_handle_t temp_sensor = NULL;
temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);

/* Private functions */
/*
 *  Stack event callback functions
 *      - on_stack_reset is called when host resets BLE stack due to errors
 *      - on_stack_sync is called when host has synced with controller
 */
static void on_stack_reset(int reason) {
    /* On reset, print reset reason to console */
    ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}

static void on_stack_sync(void) {
    /* On stack sync, do advertising initialization */
    adv_init();
}

static void nimble_host_config_init(void) {
    /* Set host callbacks */
    ble_hs_cfg.reset_cb = on_stack_reset;
    ble_hs_cfg.sync_cb = on_stack_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Store host configuration */
    ble_store_config_init();
}

static void nimble_host_task(void *param) {
    /* Task entry log */
    ESP_LOGI(TAG, "nimble host task has been started!");

    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();

    /* Clean up at exit */
    vTaskDelete(NULL);
}

static void esp32_remotecar_init()
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
    muxed to GPIO on reset already, but some default to other
    functions and need to be switched to GPIO. Consult the
    Technical Reference for a list of pads and their default
    functions.)
    */
    gpio_reset_pin(BLINK_GPIO);

    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    /* Enable temperature sensor */ 
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));
}

static void esp32_remotecar_1000ms(void *pvParameter)
{
    while(1)
    {
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        //ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);

        /* Turn on LED based on the temp */
        if (tsens_value > 25)
        {
            /* LED on (output low) */
            gpio_set_level(BLINK_GPIO, 0);
        }
        else
        {
            /* LED off (output high) */
            gpio_set_level(BLINK_GPIO, 1);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/* Main function */
void app_main(void)
{
        /* Local variables */
    int rc = 0;
    esp_err_t ret = ESP_OK;

    /* NVS flash initialization */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nvs flash, error code: %d ", ret);
        return;
    }

    /* NimBLE host stack initialization */
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nimble stack, error code: %d ",
                 ret);
        return;
    }

    /* GAP service initialization */
    rc = gap_init();
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to initialize GAP service, error code: %d", rc);
        return;
    }

    esp32_remotecar_init();
    /* NimBLE host configuration initialization */
    nimble_host_config_init();

    /* Start NimBLE host task thread and return */
    xTaskCreate(nimble_host_task, "NimBLE Host", 4*1024, NULL, 5, NULL);

    xTaskCreate(&esp32_remotecar_1000ms, "1000ms task", 2048, NULL, 5, NULL); 

    /* Will not reach here. */
}