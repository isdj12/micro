if(BOOTLOADER_BUILD)
    idf_component_register(SRCS "bootloader_start.c"
                           PRIV_REQUIRES bootloader_support esp_rom hal log spi_flash)
else()
    idf_component_register()
endif()
