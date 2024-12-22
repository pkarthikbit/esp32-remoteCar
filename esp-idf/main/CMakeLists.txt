set(srcs "esp32_remoteCar.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
