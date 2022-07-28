#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"

#include "cfg.h"
#include "ble.h"

#define CMD_SET_HW      0xF1    /* set hot water        */
#define CMD_SET_CW      0xF2    /* set cold water       */
#define CMD_SET_LPP     0xF3    /* set liters per pulse */
#define CMD_RESET_WL    0xF4    /* reset whitelist      */
#define CMD_RESET       0xF5    /* reset module         */

#define HOT_WATER       0x00
#define COLD_WATER      0x01

static void set_water(uint8_t hot_cold, uint8_t b32, uint8_t b24, uint8_t b16, uint8_t b8) {
    uint32_t counter;
    counter = ((b32 << 24)  & 0xFF000000);
    counter |= ((b24 << 16) & 0xFF0000);
    counter |= ((b16 << 8)  & 0xFF00);
    counter |= (b8          & 0xFF);

    if (hot_cold == HOT_WATER) {
        watermeter_config.counters.hot_water_count = counter;
        if (watermeter_config.counters.hot_water_count > (COUNTERS_OVERFLOW-LITERS_PER_PULSE)) {
            watermeter_config.counters.hot_water_count -= (COUNTERS_OVERFLOW-LITERS_PER_PULSE);
        }
#if UART_PRINT_DEBUG_ENABLE
        printf("New counter - %u. Set hot water - %u\r\n", counter, watermeter_config.counters.hot_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        write_config();
    } else if (hot_cold == COLD_WATER) {
        watermeter_config.counters.cold_water_count = counter;
        if (watermeter_config.counters.cold_water_count > (COUNTERS_OVERFLOW-LITERS_PER_PULSE)) {
            watermeter_config.counters.cold_water_count -= (COUNTERS_OVERFLOW-LITERS_PER_PULSE);
        }
#if UART_PRINT_DEBUG_ENABLE
        printf("New counter - %u. Set cold water - %u\r\n", counter, watermeter_config.counters.cold_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        write_config();
    }
    ev_adv_timeout(0,0,0);
}

void cmd_parser(void * p) {
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	uint8_t *in_data = req->dat;
	uint8_t len = req->l2cap-3;

    if (*in_data == CMD_SET_HW && len == 5) {
        set_water(HOT_WATER, in_data[1], in_data[2], in_data[3], in_data[4]);
    } else if (*in_data == CMD_SET_CW && len == 5) {
        set_water(COLD_WATER, in_data[1], in_data[2], in_data[3], in_data[4]);
    } else if (*in_data == CMD_SET_LPP && len == 2) {
	    watermeter_config.liters_per_pulse = in_data[1];
        write_config();
	} else if (*in_data == CMD_RESET_WL && len == 1) {
	    watermeter_config.whitelist_enable = 0;
#if UART_PRINT_DEBUG_ENABLE
	    printf("Reset whitelist\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
	    bls_smp_eraseAllParingInformation();
	    ev_adv_timeout(0,0,0);
    } else if (*in_data == CMD_RESET && len == 1) {
#if UART_PRINT_DEBUG_ENABLE
        printf("Reboot module\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
        start_reboot();
    }



}
