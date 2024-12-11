idf_build_get_property(target IDF_TARGET)
set(priv_requires esp_driver_gpio esp_driver_uart esp_driver_tsens esp_event nvs_flash esp_netif lwip)

if(${target} STREQUAL "linux")
    list(APPEND priv_requires "")
endif()

idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES ${priv_requires}
                    INCLUDE_DIRS ".")