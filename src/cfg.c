#include <stdint.h>
#include "drivers\8258\flash.h"
#include "tl_common.h"

#include "cfg.h"
#include "log.h"

_attribute_data_retention_ watermeter_config_t watermeter_config;
_attribute_data_retention_ static uint8_t default_config = false;

_attribute_ram_code_ static void clear_user_data() {
    uint32_t flash_addr = BEGIN_USER_DATA;

    while(flash_addr < END_USER_DATA) {
        flash_erase_sector(flash_addr);
        flash_addr += FLASH_SECTOR_SIZE;
    }
}

static void init_default_config(uint32_t hot_count, uint32_t cold_count) {
    memset(&watermeter_config, 0, sizeof(watermeter_config_t));
    watermeter_config.size = sizeof(watermeter_config_t);
    watermeter_config.id = ID_CONFIG;
    watermeter_config.top = 0;
    watermeter_config.flash_addr = BEGIN_USER_DATA;
    watermeter_config.liters_per_pulse = LITERS_PER_PULSE;
    watermeter_config.counters.id = ID_COUNTERS;
    watermeter_config.counters.hot_water_count = hot_count;
    watermeter_config.counters.cold_water_count = cold_count;
    default_config = true;
    write_config();
}

_attribute_ram_code_ void init_config() {
    watermeter_config_t config_curr, config_next;
    uint32_t flash_addr = BEGIN_USER_DATA;
    uint8_t find_config = false;

    flash_read_page(flash_addr, sizeof(watermeter_config_t), (uint8_t*)&config_curr);

    if (config_curr.id != ID_CONFIG) {
#if UART_PRINT_DEBUG_ENABLE
        printf("No saved config! Init.\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
        clear_user_data();
        init_default_config(0, 0);
        return;
    }

    flash_addr += FLASH_PAGE_SIZE;

    while(flash_addr < END_USER_DATA) {
        flash_read_page(flash_addr, sizeof(watermeter_config_t), (uint8_t*)&config_next);

        if (config_next.id == ID_CONFIG) {
            if ((config_curr.top + 1) == config_next.top ||
                    (config_curr.top == TOP_MASK && config_next.top == 0)) {
                memcpy(&config_curr, &config_next, sizeof(watermeter_config_t));
                flash_addr += FLASH_PAGE_SIZE;
                continue;
            }
            find_config = true;
            break;
        }
        find_config = true;
        break;
    }

    if (find_config) {
        if (config_curr.size != sizeof(watermeter_config_t)) {
#if UART_PRINT_DEBUG_ENABLE
            printf("Check new format config! Reinit.\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
            clear_user_data();
            if (config_curr.counters.id == ID_COUNTERS) {
                /* save old count in new config */
#if UART_PRINT_DEBUG_ENABLE
                printf("Find old water counters data.\r\n");
                printf("Hot  counter - %u\r\n", config_curr.counters.hot_water_count);
                printf("Cold counter - %u\r\n", config_curr.counters.cold_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
                init_default_config(config_curr.counters.hot_water_count, config_curr.counters.cold_water_count);
            } else {
#if UART_PRINT_DEBUG_ENABLE
                printf("Can't find old water counters data. New data = 0.\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
                init_default_config(0, 0);
            }
        } else {
            memcpy(&watermeter_config, &config_curr, sizeof(watermeter_config_t));
            watermeter_config.flash_addr = flash_addr-FLASH_PAGE_SIZE;
#if UART_PRINT_DEBUG_ENABLE
            printf("Read config from flash address - 0x%x\r\n", watermeter_config.flash_addr);
#endif /* UART_PRINT_DEBUG_ENABLE */
        }
    } else {
#if UART_PRINT_DEBUG_ENABLE
        printf("No active saved config! Reinit.\r\n");
//        printf("No saved config! Init.\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
        clear_user_data();
        init_default_config(0, 0);
    }
}

_attribute_ram_code_ void write_config() {
    if (default_config) {
        flash_erase_sector(watermeter_config.flash_addr);
        flash_write_page(watermeter_config.flash_addr, sizeof(watermeter_config_t), (uint8_t*)&(watermeter_config));
        default_config = false;
    } else {
        watermeter_config.flash_addr += FLASH_PAGE_SIZE;
        if (watermeter_config.flash_addr == END_USER_DATA) {
            watermeter_config.flash_addr = BEGIN_USER_DATA;
        }
        if (watermeter_config.flash_addr % FLASH_SECTOR_SIZE == 0) {
            flash_erase_sector(watermeter_config.flash_addr);
        }
        watermeter_config.top++;
        watermeter_config.top &= TOP_MASK;
        flash_write_page(watermeter_config.flash_addr, sizeof(watermeter_config_t), (uint8_t*)&(watermeter_config));
    }
#if UART_PRINT_DEBUG_ENABLE
    printf("Save config to flash address - 0x%x\r\n", watermeter_config.flash_addr);
#endif /* UART_PRINT_DEBUG_ENABLE */
}

