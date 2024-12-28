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
#include "modlog/modlog.h"

/* BLE */
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

/* Public inclusion */
#include "esp32_remoteCar_pub.h"

#endif /* ESP32_REMOTECAR_INF_H */
