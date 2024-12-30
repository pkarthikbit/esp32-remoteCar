/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef ESP32_REMOTECAR_INF_H
#define ESP32_REMOTECAR_INF_H

/* STD */
#include <stdbool.h>
#include <math.h>

/* ESP32 Generic */
#include "esp_peripheral.h"
#include "esp_log.h"
#include "nvs_flash.h"

/* ESP32 Driver */
#include "driver/uart.h"
#include "driver/gpio.h"
#include "modlog/modlog.h"

/* BLE */
#include "nimble/ble.h"
/* Includes the declaration of functions required for the initialization of the nimble stack. */
#include "nimble/nimble_port.h"
/* Initializes and enables nimble host task. */
#include "nimble/nimble_port_freertos.h"
/* Defines the functionalities to handle the host event. */
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
/* Defines the macros for device name, and device appearance and declares the function to set them. */
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

/* Public inclusion */
#include "esp32_remoteCar_pub.h"

#endif /* ESP32_REMOTECAR_INF_H */
