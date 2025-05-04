set(srcs "esp32_remoteCar.c" "esp32_remoteCar_callback.c" "esp32_remoteCar_gatt.c" "esp32_remoteCar_gap.c") 

set(hdrs ".")

idf_component_register(SRCS "${srcs}"
                       REQUIRES bt esp_driver_gpio esp_driver_ledc esp_driver_uart nvs_flash
                       INCLUDE_DIRS "${hdrs}")
