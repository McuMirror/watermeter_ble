#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"

#include "cfg.h"
#include "ble.h"

#define CMD_SET_LPP     0xF1    /* set liters per pulse  */
#define CMD_SET_WL      0xF2    /* add mac for whitelist */
#define CMD_SET_HW      0xF3    /* set hot water         */
#define CMD_SET_CW      0xF4    /* set cold water        */
#define CMD_RESET       0xF5    /* reset module          */
#define HOT_WATER       0x00
#define COLD_WATER      0x01

static void set_water(uint8_t hot_cold, uint8_t b32, uint8_t b24, uint8_t b16, uint8_t b8) {
    uint32_t counter;
    counter = ((b32 << 24)  & 0xFF000000);
    counter |= ((b24 << 16) & 0xFF0000);
    counter |= ((b16 << 8)  & 0xFF00);
    counter |= (b8          & 0xFF);

    if (hot_cold == HOT_WATER) {
#if UART_PRINT_DEBUG_ENABLE
        printf("Set hot water - %u\r\n", counter);
#endif /* UART_PRINT_DEBUG_ENABLE */
        watermeter_config.counters.hot_water_count = counter;
        write_config();
    } else if (hot_cold == COLD_WATER) {
#if UART_PRINT_DEBUG_ENABLE
        printf("Set cold water - %u\r\n", counter);
#endif /* UART_PRINT_DEBUG_ENABLE */
        watermeter_config.counters.cold_water_count = counter;
        write_config();
    }
}

void cmd_parser(void * p) {
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	uint8_t inData = req->dat[0];

	if (inData == CMD_SET_LPP) {
	    watermeter_config.liters_per_pulse = req->dat[1];
        write_config();
	} else if (inData == CMD_SET_WL) {
	    if (watermeter_config.whitelist_enable < 4) {
	        uint8_t mac[7];
            mac[0] = req->dat[1];
            mac[1] = req->dat[2];
            mac[2] = req->dat[3];
            mac[3] = req->dat[4];
            mac[4] = req->dat[5];
            mac[5] = req->dat[6];
            mac[6] = 0;
	        watermeter_config.whitelist_enable++;
	        if (watermeter_config.whitelist_enable == 1) {
#if UART_PRINT_DEBUG_ENABLE
	            printf("MAC1 0x%X added to whitelist\r\n", mac);
#endif /* UART_PRINT_DEBUG_ENABLE */
	            memcpy(watermeter_config.wl_mac1, mac, 6);
	            ll_whiteList_reset();
	            ll_whiteList_add(BLE_ADDR_PUBLIC, watermeter_config.wl_mac1);
	        } else if (watermeter_config.whitelist_enable == 2) {
#if UART_PRINT_DEBUG_ENABLE
                printf("MAC2 0x%X added to whitelist\r\n", mac);
#endif /* UART_PRINT_DEBUG_ENABLE */
                memcpy(watermeter_config.wl_mac2, mac, 6);
                ll_whiteList_add(BLE_ADDR_PUBLIC, watermeter_config.wl_mac2);
            } else if (watermeter_config.whitelist_enable == 3) {
#if UART_PRINT_DEBUG_ENABLE
                printf("MAC3 0x%X added to whitelist\r\n", mac);
#endif /* UART_PRINT_DEBUG_ENABLE */
                memcpy(watermeter_config.wl_mac3, mac, 6);
                ll_whiteList_add(BLE_ADDR_PUBLIC, watermeter_config.wl_mac3);
            } else if (watermeter_config.whitelist_enable == 4) {
#if UART_PRINT_DEBUG_ENABLE
                printf("MAC4 0x%X added to whitelist\r\n", mac);
#endif /* UART_PRINT_DEBUG_ENABLE */
                memcpy(watermeter_config.wl_mac4, mac, 6);
                ll_whiteList_add(BLE_ADDR_PUBLIC, watermeter_config.wl_mac4);
	        }
	        write_config();
	        ev_adv_timeout(0,0,0);
	    } else {
#if UART_PRINT_DEBUG_ENABLE
        printf("Whitelist is full\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

	    }
	} else if (inData == CMD_SET_WL) {
	} else if (inData == CMD_SET_HW) {
	    set_water(HOT_WATER, req->dat[1], req->dat[2], req->dat[3], req->dat[4]);
	} else if (inData == CMD_SET_CW) {
        set_water(COLD_WATER, req->dat[1], req->dat[2], req->dat[3], req->dat[4]);
	}



}
