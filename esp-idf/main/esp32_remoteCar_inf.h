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
#include "driver/ledc.h"
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

extern void adv_stack_enableClearLegacyAdvVsCmd(bool en);
extern void advFilter_stack_enableDupExcListVsCmd(bool en);
extern void scan_stack_enableAdvFlowCtrlVsCmd(bool en);
extern void arr_stack_enableMultiConnVsCmd(bool en);
extern void pcl_stack_enableSetRssiThreshVsCmd(bool en);
extern void chanSel_stack_enableSetCsaVsCmd(bool en);
extern void log_stack_enableLogsRelatedVsCmd(bool en);
extern void hci_stack_enableSetVsEvtMaskVsCmd(bool en);
extern void winWiden_stack_enableSetConstPeerScaVsCmd(bool en);

extern void adv_stack_enableScanReqRxdVsEvent(bool en);
extern void conn_stack_enableChanMapUpdCompVsEvent(bool en);

extern int base_stack_initEnv(void);
extern int conn_stack_initEnv(void);

extern void conn_stack_deinitEnv(void);
extern void base_stack_deinitEnv(void);

extern int base_stack_enable(void);
extern int conn_stack_enable(void);
extern void mmgmt_enableRxbufOptFeature(void);

extern void conn_stack_disable(void);
extern void base_stack_disable(void);

#endif /* ESP32_REMOTECAR_INF_H */
