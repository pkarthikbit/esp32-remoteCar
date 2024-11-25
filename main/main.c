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

/* constant definition */
#define BLINK_GPIO  GPIO_NUM_15
#define TIME_1000MS 1000u        

/* Variable definition */
static const char *TAG = "esp32-remoteCar";     /* for module name logging */
xTimeOutType x_timeout;                         /* for time out function */
portTickType openTimeout;                       /* for time out funciton */
temperature_sensor_handle_t temp_sensor = NULL;
temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
float tsens_value;
bool led_st;

/* Funciton declaration */
IRAM_ATTR static bool temp_sensor_monitor_cbs(temperature_sensor_handle_t tsens, const temperature_sensor_threshold_event_data_t *edata, void *user_data)
{
    ESP_DRAM_LOGI(TAG, "Temperature value is higher or lower than threshold, value is %d\n...\n\n", edata->celsius_value);
    led_st = true;
    return false;
}

static void esp32_remotecar_init(void *pvParameter)
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

    vTaskSetTimeOutState(&x_timeout);
    openTimeout = TIME_1000MS; /*ms*/

    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));

    temperature_sensor_event_callbacks_t cbs = {
    .on_threshold = temp_sensor_monitor_cbs,
    };

    temperature_sensor_abs_threshold_config_t threshold_cfg = {
    .high_threshold = 20,
    .low_threshold = 16,
    };

    ESP_ERROR_CHECK(temperature_sensor_set_absolute_threshold(temp_sensor, &threshold_cfg));

    ESP_ERROR_CHECK(temperature_sensor_register_callbacks(temp_sensor, &cbs, NULL));

    /* Enable temperature sensor */ 
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));
}

void esp32_remotecar_1000ms()
{
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
    ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);

    if (led_st)
    {
        /* LED on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
    }
    else
    {
        /* LED off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
    }

    led_st = false;
}

/* Main function */
void app_main(void)
{
    xTaskCreate(&esp32_remotecar_init, "init task", 2048, NULL, 5, NULL); 

    /* List 1000 ms periodic functions under this */
    if(xTaskCheckForTimeOut(&x_timeout, &openTimeout) == pdTRUE)
    {
        esp32_remotecar_1000ms();
        openTimeout = TIME_1000MS; /*reload the var*/
    }
}