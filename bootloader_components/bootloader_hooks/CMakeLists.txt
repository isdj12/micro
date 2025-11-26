# Минимальный CMakeLists.txt для хуков загрузчика. 
# Он автоматически включает hooks.c и добавляет зависимости загрузчика.
# Использование папки 'bootloader_hooks' автоматически дает ему 'bootloader_api'
# и избегает конфликтов.
idf_component_register(SRCS "hooks.c"
                       PRIV_REQUIRES bootloader_support esp_rom hal log spi_flash)