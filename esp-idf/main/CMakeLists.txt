set(srcs "main.c"
         "gatt_svr.c"
         "esp32_remoteCar.c")

idf_component_register(SRCS "${srcs}"
                        REQUIRES bt esp_driver_gpio esp_driver_ledc esp_driver_uart nvs_flash
                       INCLUDE_DIRS ".")
