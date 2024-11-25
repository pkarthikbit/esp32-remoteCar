/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
/* inclusion - RTOS related */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/* inclusion - for logging */
#include "esp_log.h"
/* inclusion - driver related */
#include "driver/gpio.h"
#include "driver/temperature_sensor.h"

/* Variable definition */
static const char *TAG = "example";

/* constant definition */
#define BLINK_GPIO GPIO_NUM_15

/* Funciton declaration */
static void blink_task(void *pvParameter)
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

    while(1)
    {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

static void Tsensor_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp ranger range: 10~50 ℃");
    temperature_sensor_handle_t temp_sensor = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));

    ESP_LOGI(TAG, "Enable temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));

    ESP_LOGI(TAG, "Read temperature");
    int cnt = 20;
    float tsens_value;
    while (cnt--) {
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Main function */
void app_main(void)
{
    printf("Hello world!\n");
    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL); 
    xTaskCreate(&blink_task, "Tsensor_task", 2048, NULL, 5, NULL); 
}