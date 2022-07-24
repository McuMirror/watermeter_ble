#include <stdint.h>
#include "drivers\8258\flash.h"
#include "tl_common.h"

#include "cfg.h"

#ifndef MAC1
_attribute_data_retention_ uint8_t wl_mac1[6] = {0,0,0,0,0,0};
_attribute_data_retention_ uint8_t wl_mac2[6] = {0,0,0,0,0,0};
_attribute_data_retention_ uint8_t wl_mac3[6] = {0,0,0,0,0,0};
_attribute_data_retention_ uint8_t wl_mac4[6] = {0,0,0,0,0,0};
#else
_attribute_data_retention_ uint8_t wl_mac1[6] = MAC1;
    #ifndef MAC2
_attribute_data_retention_ uint8_t wl_mac2[6] = {0,0,0,0,0,0};
_attribute_data_retention_ uint8_t wl_mac3[6] = {0,0,0,0,0,0};
_attribute_data_retention_ uint8_t wl_mac4[6] = {0,0,0,0,0,0};
    #else
_attribute_data_retention_ uint8_t wl_mac2[6] = MAC2;
        #ifndef MAC3
_attribute_data_retention_ uint8_t wl_mac3[6] = {0,0,0,0,0,0};
_attribute_data_retention_ uint8_t wl_mac4[6] = {0,0,0,0,0,0};
        #else
_attribute_data_retention_ uint8_t wl_mac3[6] = MAC3;
            #ifndef MAC4
_attribute_data_retention_ uint8_t wl_mac4[6] = {0,0,0,0,0,0};
            #else
_attribute_data_retention_ uint8_t wl_mac4[6] = MAC4;
            #endif /* MAC4 */
        #endif /* MAC3 */
    #endif /* MAC2 */
#endif /* MAC1 */



_attribute_data_retention_ watermeter_config_t watermeter_config;
_attribute_data_retention_ static uint8_t default_config = true;

uint16_t check_mac_wl(uint8_t *mac) {

    return mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5];

}

static void clear_user_data() {
    uint32_t flash_addr = BEGIN_USER_DATA;

    while(flash_addr != END_USER_DATA) {
        flash_erase_sector(flash_addr);
        flash_addr += FLASH_SECTOR_SIZE;
    }
}

static void init_default_config(uint32_t hot_count, uint32_t cold_count) {
    memset(&watermeter_config, 0, sizeof(watermeter_config_t));
    watermeter_config.size = sizeof(watermeter_config_t);
    watermeter_config.id = ID_CONFIG;
    watermeter_config.active = ON;
    watermeter_config.flash_addr = BEGIN_USER_DATA;
    watermeter_config.liters_per_pulse = LITERS_PER_PULSE;
    watermeter_config.counters.id = ID_COUNTERS;
    watermeter_config.counters.hot_water_count = hot_count;
    watermeter_config.counters.cold_water_count = cold_count;
    if (check_mac_wl(wl_mac1)) {
        memcpy(watermeter_config.wl_mac1, wl_mac1, sizeof(wl_mac1));
        watermeter_config.whitelist_enable++;
        if (check_mac_wl(wl_mac2)) {
            memcpy(watermeter_config.wl_mac2, wl_mac2, sizeof(wl_mac2));
            watermeter_config.whitelist_enable++;
            if (check_mac_wl(wl_mac3)) {
                memcpy(watermeter_config.wl_mac3, wl_mac3, sizeof(wl_mac3));
                watermeter_config.whitelist_enable++;
                if (check_mac_wl(wl_mac4)) {
                    memcpy(watermeter_config.wl_mac4, wl_mac4, sizeof(wl_mac4));
                    watermeter_config.whitelist_enable++;
                }
            }
        }
    }
    default_config = true;
    write_config();
}

void init_config() {
    watermeter_config_t config;
    uint32_t flash_addr = BEGIN_USER_DATA;

    while(1) {
        flash_read_page(flash_addr, sizeof(watermeter_config_t), (uint8_t*)&(config));

        if (config.id == ID_CONFIG) {
            if (config.active) {
                if (config.size != sizeof(watermeter_config_t)) {
#if UART_PRINT_DEBUG_ENABLE
                    printf("Check new format config! Reinit.\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
                    clear_user_data();
                    if (config.counters.id == ID_COUNTERS) {
                        /* save old count in new config */
                        init_default_config(config.counters.hot_water_count, config.counters.cold_water_count);
                    } else {
#if UART_PRINT_DEBUG_ENABLE
                        printf("Can't find old water counters data. New data = 0.\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
                        init_default_config(0, 0);
                    }
                    break;
                }
                memcpy(&watermeter_config, &config, sizeof(watermeter_config_t));
                watermeter_config.flash_addr = flash_addr;
                break;
            } else {
                flash_addr += FLASH_SECTOR_SIZE;
                if (flash_addr == END_USER_DATA) {
#if UART_PRINT_DEBUG_ENABLE
                    printf("No active saved config! Reinit.\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
                    init_default_config(0, 0);
                    break;
                }
                continue;
            }
        } else {
#if UART_PRINT_DEBUG_ENABLE
            printf("No saved config! Init.\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
            clear_user_data();
            init_default_config(0, 0);
            break;
        }
    }
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

