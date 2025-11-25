#include <stdint.h>
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "hal/gpio_ll.h"
#include "bootloader_flash_config.h"
#include "bootloader_common.h"
#include "bootloader_utility.h"     
#include "esp_flash_partitions.h"   

#define BOOT_BUTTON_GPIO0 18 // Кнопка для выбора OTA_0
#define BOOT_BUTTON_GPIO1 4  // Кнопка для выбора OTA_1
#define LED_GPIO 2 

#define X_SHORT_US 200000 // 200 мс (0.2 сек) для короткого мигания (Точка или короткая пауза)
#define X_LONG_US 600000  // 600 мс (0.6 сек) для длинного мигания (Тире)


#define LED_ON 1
#define LED_OFF 0 

void bootloader_after_init(void) {
    // --- 1. Настройка GPIO ---
    
    // Кнопка 0 (Вход с подтяжкой вверх)
    esp_rom_gpio_pad_select_gpio(BOOT_BUTTON_GPIO0);
    esp_rom_gpio_connect_in_signal(BOOT_BUTTON_GPIO0, GPIO_IN_REG, false);
    gpio_ll_input_enable(&GPIO, BOOT_BUTTON_GPIO0);
    gpio_ll_pullup_en(&GPIO, BOOT_BUTTON_GPIO0);
    gpio_ll_pulldown_dis(&GPIO, BOOT_BUTTON_GPIO0);

    // Кнопка 1 (Вход с подтяжкой вверх)
    esp_rom_gpio_pad_select_gpio(BOOT_BUTTON_GPIO1);
    esp_rom_gpio_connect_in_signal(BOOT_BUTTON_GPIO1, GPIO_IN_REG, false);
    gpio_ll_input_enable(&GPIO, BOOT_BUTTON_GPIO1);
    gpio_ll_pullup_en(&GPIO, BOOT_BUTTON_GPIO1);
    gpio_ll_pulldown_dis(&GPIO, BOOT_BUTTON_GPIO1);
    
    // Светодиод (Выход)
    esp_rom_gpio_pad_select_gpio(LED_GPIO);
    gpio_ll_output_enable(&GPIO, LED_GPIO); 

    // Задержка для стабилизации (antibounce) и времени реакции пользователя.
    // Если кнопка не будет нажата за 100 мс, переходим к стандартной загрузке.
    esp_rom_delay_us(100000); // 100 мс

    // --- 2. Загрузка таблицы разделов ---
    bootloader_state_t bs = {0};
    
    if (!bootloader_utility_load_partition_table(&bs)) {
        esp_rom_printf("[BOOT] Ошибка: Не удалось прочитать таблицу разделов.\n");
        return;
    }

    int slot_index = -1; 

    // --- 3. Логика выбора ---

    // Если нажата кнопка 0 -> Грузим слот 0 (OTA_0)
    if (gpio_ll_get_level(&GPIO, BOOT_BUTTON_GPIO0) == 0) 
    {
        esp_rom_printf("\n[BOOT] КНОПКА 0: Выбран слот 0 (OTA_0)\n");
        slot_index = 0; 

        // Индикация: 5 коротких миганий (быстрый ACK)
        for(int i = 0; i < 5; i++) {
            gpio_ll_set_level(&GPIO, LED_GPIO, LED_OFF);
            esp_rom_delay_us(50000); // 50 мс
            gpio_ll_set_level(&GPIO, LED_GPIO, LED_OFF);
            esp_rom_delay_us(50000); // 50 мс
        }
    } 
    // Если нажата кнопка 1 -> Грузим слот 1 (OTA_1)
    else if (gpio_ll_get_level(&GPIO, BOOT_BUTTON_GPIO1) == 0) 
    {
        esp_rom_printf("\n[BOOT] КНОПКА 1: Выбран слот 1 (OTA_1)\n");
        slot_index = 1; 
        
        // SOS мигалка (один цикл)
        // S (. . .)
        for (int i = 0; i < 3; i++) { 
            gpio_ll_set_level(&GPIO, LED_GPIO, LED_ON); 
            esp_rom_delay_us(X_SHORT_US); 
            gpio_ll_set_level(&GPIO, LED_GPIO, LED_OFF); 
            esp_rom_delay_us(X_SHORT_US); 
        }
        esp_rom_delay_us(X_SHORT_US * 3); // Пауза между S и O
        
        // O (--- --- ---)
        for (int i = 0; i < 3; i++) { 
            gpio_ll_set_level(&GPIO, LED_GPIO, LED_ON); 
            esp_rom_delay_us(X_LONG_US); 
            gpio_ll_set_level(&GPIO, LED_GPIO, LED_OFF); 
            esp_rom_delay_us(X_SHORT_US); // Короткая пауза между тире
        }
        esp_rom_delay_us(X_SHORT_US * 3); // Пауза между O и S

        // S (. . .)
        for (int i = 0; i < 3; i++) { 
            gpio_ll_set_level(&GPIO, LED_GPIO, LED_ON); 
            esp_rom_delay_us(X_SHORT_US); 
            gpio_ll_set_level(&GPIO, LED_GPIO, LED_OFF); 
            esp_rom_delay_us(X_SHORT_US); 
        }
        esp_rom_delay_us(X_SHORT_US * 7); // Конец цикла SOS
    }

    // --- 4. Выполнение загрузки ---
    if (slot_index != -1) {
        if (slot_index < bs.app_count) {
             esp_rom_printf("[BOOT] Загрузка образа из слота %d...\n", slot_index);
             // Функция загрузки образа, которая передает управление (не возвращает).
             bootloader_utility_load_boot_image(&bs, slot_index);
        } else {
             esp_rom_printf("[BOOT] Ошибка: Слот %d не найден в таблице разделов!\n", slot_index);
        }
    } else {
        esp_rom_printf("[BOOT] Кнопки не нажаты. Стандартная загрузка.\n");
    }
}