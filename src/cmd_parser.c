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

void cmd_parser(void * p) {
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	uint8_t *in_data = req->dat;
	uint8_t len = req->l2cap-3;
	uint32_t counter = 0;

    if ((*in_data == CMD_SET_HW || *in_data == CMD_SET_CW) && len > 1) {

        for (uint8_t i = 1; i < len; i++) {
            if (i == 1) {
                counter |= (in_data[i] & 0xFF);
            } else if (i == 2) {
                counter |= ((in_data[i] << 8) & 0xFF00);
            } else if ( i == 3) {
                counter |= ((in_data[i] << 16) & 0xFF0000);
            } else {
                counter |= ((in_data[i] << 24) & 0xFF000000);
            }
        }

        if (*in_data == CMD_SET_HW) {
            watermeter_config.counters.hot_water_count = counter;
            if (watermeter_config.counters.hot_water_count > (COUNTERS_OVERFLOW-LITERS_PER_PULSE)) {
                watermeter_config.counters.hot_water_count -= (COUNTERS_OVERFLOW-LITERS_PER_PULSE);
            }
#if UART_PRINT_DEBUG_ENABLE
            printf("New counter - %u. Set hot water - %u\r\n", counter, watermeter_config.counters.hot_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        } else {
            watermeter_config.counters.cold_water_count = counter;
            if (watermeter_config.counters.cold_water_count > (COUNTERS_OVERFLOW-LITERS_PER_PULSE)) {
                watermeter_config.counters.cold_water_count -= (COUNTERS_OVERFLOW-LITERS_PER_PULSE);
            }
#if UART_PRINT_DEBUG_ENABLE
            printf("New counter - %u. Set cold water - %u\r\n", counter, watermeter_config.counters.cold_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        }
        write_config();
        set_adv_data();
    } else if (*in_data == CMD_SET_LPP && len == 2) {
	    watermeter_config.liters_per_pulse = in_data[1];
        write_config();
	} else if (*in_data == CMD_RESET_WL && len == 1) {
#if UART_PRINT_DEBUG_ENABLE
	    printf("Reset whitelist\r\nDisconnect\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
	    bls_smp_eraseAllParingInformation();
	    ev_adv_timeout(0,0,0);
	    bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
    } else if (*in_data == CMD_RESET && len == 1) {
#if UART_PRINT_DEBUG_ENABLE
        printf("Disconnect\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
        bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
        sleep_ms(2000);
#if UART_PRINT_DEBUG_ENABLE
        printf("Reboot module\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
        start_reboot();
    }



}
