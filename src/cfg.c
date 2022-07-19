#include <stdint.h>
#include "drivers\8258\flash.h"
#include "tl_common.h"

#include "cfg.h"

_attribute_data_retention_ watermeter_config_t watermeter_config;
_attribute_data_retention_ static uint8_t default_config = true;

void init_default_config() {
    watermeter_config.id = ID_CFG;
    watermeter_config.active = ON;
    watermeter_config.flash_addr = BEGIN_USER_DATA;
    watermeter_config.liters_per_pulse = LITERS_PER_PULSE;
    watermeter_config.hot_water_count = 0;
    watermeter_config.cold_water_count = 0;
    memset(watermeter_config.ble_name, 0, sizeof(watermeter_config.ble_name));
    default_config = true;
    write_config();
}

uint8_t read_config() {
    watermeter_config_t config;
    uint32_t flash_addr = BEGIN_USER_DATA;

    while(1) {
        flash_read_page(flash_addr, sizeof(watermeter_config_t), (uint8_t*)&(config));

        if (config.id == ID_CFG) {
            if (config.active) {
                memcpy(&watermeter_config, &config, sizeof(watermeter_config_t));
                watermeter_config.flash_addr = flash_addr;
                return true;
            } else {
                flash_addr += FLASH_SECTOR_SIZE;
                if (flash_addr == END_USER_DATA) {
                    return false;
                }
                continue;
            }
        } else {
            return false;
        }
    }

    return false;
}

void write_config() {
    watermeter_config_t config;

    if (default_config) {
        flash_erase_sector(watermeter_config.flash_addr);
        flash_write_page(watermeter_config.flash_addr, sizeof(watermeter_config_t), (uint8_t*)&(watermeter_config));
        default_config = false;
    } else {
        flash_read_page(watermeter_config.flash_addr, sizeof(watermeter_config_t), (uint8_t*)&(config));
        config.active = OFF;
        flash_erase_sector(watermeter_config.flash_addr);
        flash_write_page(watermeter_config.flash_addr, sizeof(watermeter_config_t), (uint8_t*)&(config));
        watermeter_config.flash_addr += FLASH_SECTOR_SIZE;
        if (watermeter_config.flash_addr == END_USER_DATA) {
            watermeter_config.flash_addr = BEGIN_USER_DATA;
        }
        flash_erase_sector(watermeter_config.flash_addr);
        flash_write_page(watermeter_config.flash_addr, sizeof(watermeter_config_t), (uint8_t*)&(watermeter_config));
    }
#if UART_PRINT_DEBUG_ENABLE
    printf("Save config to flash. watermeter_config.flash_addr - 0x%x\r\n", watermeter_config.flash_addr);
#endif /* UART_PRINT_DEBUG_ENABLE */
}

void clear_config() {
    uint32_t flash_addr = BEGIN_USER_DATA;

    while(flash_addr != END_USER_DATA) {
        flash_erase_sector(flash_addr);
        flash_addr += FLASH_SECTOR_SIZE;
    }
    init_default_config();
}

//void add_hotwater(uint32_t count) {
//    watermeter_config.hot_water += (count * watermeter_config.liters_per_pulse);
//}
//
//void add_coldwater(uint32_t count) {
//    watermeter_config.cold_water += (count * watermeter_config.liters_per_pulse);
//}
//
//uint32_t get_hotwater() {
//    return watermeter_config.hot_water;
//}
//
//uint32_t get_coldwater() {
//    return watermeter_config.cold_water;
//}
//
//const uint8_t *get_module_name() {
//    return watermeter_config.ble_name;
//}
//
//void set_module_name(uint8_t *blename) {
//    memcpy(watermeter_config.ble_name, blename, blename[0]+2);
//    write_config();
//}
