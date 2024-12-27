set(srcs "esp32_remoteCar.c" "esp32_remoteCar_callback.c" "esp32_remoteCar_gatt.c" "esp32_remoteCar_gap.c") 

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
