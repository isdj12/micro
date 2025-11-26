#include <stdint.h>
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "hal/gpio_ll.h"

#include "bootloader_common.h"
#include "esp_flash_partitions.h"

// --- Inlined from bootloader_config.h and bootloader_utility.h ---
#define MAX_OTA_SLOTS 16

typedef struct {
    esp_partition_pos_t ota_info;
    esp_partition_pos_t factory;
    esp_partition_pos_t test;
    esp_partition_pos_t ota[MAX_OTA_SLOTS];
    uint32_t app_count;
    uint32_t selected_subtype;
} bootloader_state_t;

bool bootloader_utility_load_partition_table(bootloader_state_t* bs);
void bootloader_utility_load_boot_image(const bootloader_state_t *bs, int start_index);
// ------------------------------------------------------------------

#define BOOT_BUTTON_GPIO0 21 // Кнопка для выбора OTA_0
#define BOOT_BUTTON_GPIO1 19 // Кнопка для выбора OTA_1
#define LED_GPIO 2 

#define X_SHORT_US 200000 // 200 мс (0.2 сек) для короткого мигания (Точка или короткая пауза)
#define X_LONG_US 600000  // 600 мс (0.6 сек) для длинного мигания (Тире)


#define LED_ON 1
#define LED_OFF 0 

void bootloader_before_init(void) {
    // Empty - just testing if hook is called
}