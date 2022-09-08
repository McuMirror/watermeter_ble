#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"

#include "cfg.h"
#include "ble.h"
#include "pulse.h"
#include "cmd_parser.h"

void cmd_parser(void * p) {
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	uint8_t *in_data = req->dat;
	uint8_t len = req->l2cap-3;
	uint32_t counter = 0;

    if ((*in_data == CMD_SET_HOT_COUNTER || *in_data == CMD_SET_COLD_COUNTER) && len > 1) {

        len--;
        if (len) counter |= (in_data[len--] & 0xFF);
        if (len) counter |= ((in_data[len--] << 8) & 0xFF00);
        if (len) counter |= ((in_data[len--] << 16) & 0xFF0000);
        if (len) counter |= ((in_data[len] << 24) & 0xFF000000);


        if (*in_data == CMD_SET_HOT_COUNTER) {
            watermeter_config.counters.hot_water_count = check_counter_overflow(counter);
#if UART_PRINT_DEBUG_ENABLE
            printf("New counter - %u. Set hot water - %u\r\n", counter, watermeter_config.counters.hot_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        } else {
            watermeter_config.counters.cold_water_count = check_counter_overflow(counter);
#if UART_PRINT_DEBUG_ENABLE
            printf("New counter - %u. Set cold water - %u\r\n", counter, watermeter_config.counters.cold_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        }
        write_config();
        set_adv_data();
    } else if (*in_data == CMD_SET_LITERS_PER_PULSE && len == 2) {
	    watermeter_config.liters_per_pulse = in_data[1];
        write_config();
#if UART_PRINT_DEBUG_ENABLE
            printf("New liters per pulse - %u\r\n", watermeter_config.liters_per_pulse);
#endif /* UART_PRINT_DEBUG_ENABLE */
	} else if (*in_data == CMD_CLEAR_WHIYELIST && len == 1) {
#if UART_PRINT_DEBUG_ENABLE
	    printf("Reset whitelist\r\n");
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
    } else if (*in_data == CMD_TEST && len == 1) {
#if UART_PRINT_DEBUG_ENABLE
        printf("Test command ok\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
    } else {
#if UART_PRINT_DEBUG_ENABLE
        printf("Unknown or incomplete command 0x%X\r\n", *in_data);
#endif /* UART_PRINT_DEBUG_ENABLE */

    }
}
