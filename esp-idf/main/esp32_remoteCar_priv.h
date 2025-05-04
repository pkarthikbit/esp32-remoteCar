/*
 * SPDX-FileCopyrightText: Karthik Palanisamy
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef ESP32_REMOTECAR_PRIV_H
#define ESP32_REMOTECAR_PRIV_H

/* Includes necessary headers and defines UUIDs for the SPP service and characteristic.
 * #define UUID_Service  "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
 * #define UUID_Transmit "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
 * #define UUID_Receive  "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
 */

/* 128 Bit SPP Service UUID */
#define BLE_SVC_SPP_UUID128                             \
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,     \
    0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E

/* 128 Bit SPP Service Receive Characteristic UUID */
#define BLE_SVC_SPP_CHR_RX_UUID128                      \
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,     \
    0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E

/* 128 Bit SPP Service Transmit Characteristic UUID */
#define BLE_SVC_SPP_CHR_TX_UUID128                      \
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,     \
    0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

/******************* preprocessor directives ********************/
/******************* GPIO ********************/
#define GPIO_DRV8833_IN1_1   GPIO_NUM_17 
#define GPIO_DRV8833_IN2_1   GPIO_NUM_19
#define GPIO_DRV8833_IN3_1   GPIO_NUM_22
#define GPIO_DRV8833_IN4_1   GPIO_NUM_18

#define GPIO_DRV8833_IN1_2   GPIO_NUM_16 
#define GPIO_DRV8833_IN2_2   GPIO_NUM_23
#define GPIO_DRV8833_IN3_2   GPIO_NUM_2
#define GPIO_DRV8833_IN4_2   GPIO_NUM_1

#define GPIO_OUTPUT_PIN_SEL ((1ULL<<GPIO_DRV8833_IN1_1) |       \
                              (1ULL<<GPIO_DRV8833_IN2_1) |      \
                               (1ULL<<GPIO_DRV8833_IN3_1) |     \
                                (1ULL<<GPIO_DRV8833_IN4_1) |    \
                                 (1ULL<<GPIO_DRV8833_IN1_2) |   \
                                  (1ULL<<GPIO_DRV8833_IN2_2) |  \
                                   (1ULL<<GPIO_DRV8833_IN3_2) | \
                                    (1ULL<<GPIO_DRV8833_IN4_2)  \
                            )

/******************* LEDC ********************/
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

enum ledc_channel {
    LEDC_CHANNEL_IN1_1 = 0, /*!< LEDC channel 0 */
    LEDC_CHANNEL_IN2_1,     /*!< LEDC channel 1 */
    LEDC_CHANNEL_IN3_1,     /*!< LEDC channel 2 */
    LEDC_CHANNEL_IN4_1,     /*!< LEDC channel 3 */
    LEDC_CHANNEL_IN1_2,     /*!< LEDC channel 4 */
    LEDC_CHANNEL_IN2_2,     /*!< LEDC channel 5 */
    LEDC_CHANNEL_IN3_2,     /*!< LEDC channel 6 */
    LEDC_CHANNEL_IN4_2,     /*!< LEDC channel 7 */
};

#endif /* ESP32_REMOTECAR_PRIV_H */
