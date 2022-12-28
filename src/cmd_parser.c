#include <stdint.h>
#include "tl_common.h"
#include "stack/ble/ble.h"

#include "cfg.h"
#include "log.h"
#include "ble.h"
#include "pulse.h"
#include "app.h"
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
            if (watermeter_config.counters.hot_water_count != counter) {
                watermeter_config.counters.hot_water_count = check_counter_overflow(counter);
                hot_notify = NOTIFY_MAX;
                adv_data.bthome_data.hot_counter = watermeter_config.counters.hot_water_count;
#if UART_PRINT_DEBUG_ENABLE
                printf("New counter - %u. Set hot water - %u\r\n", counter, watermeter_config.counters.hot_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
            }
        } else {
            if (watermeter_config.counters.cold_water_count != counter) {
                watermeter_config.counters.cold_water_count = check_counter_overflow(counter);
                cold_notify = NOTIFY_MAX;
                adv_data.bthome_data.cold_counter = watermeter_config.counters.cold_water_count;
#if UART_PRINT_DEBUG_ENABLE
                printf("New counter - %u. Set cold water - %u\r\n", counter, watermeter_config.counters.cold_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
            }
        }
        if ( hot_notify || cold_notify) {
            write_config();
            set_adv_data();
        }
    } else if (*in_data == CMD_SET_LITERS_PER_PULSE && len == 2) {
        main_notify.liter_per_pulse = watermeter_config.liters_per_pulse = in_data[1];
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
//	    bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
    } else if (*in_data == CMD_RESET && len == 1) {
#if UART_PRINT_DEBUG_ENABLE
        printf("Reset is enabled after disconnection\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
        ble_connected |= conn_delayed_reset;
    } else if (*in_data == CMD_MAIN_NOTIFY) {
        main_notify.id = WATERMETER_ID;
        main_notify.liter_per_pulse = watermeter_config.liters_per_pulse;
        main_notify.version = VERSION;
        memcpy(main_notify.bindkey, watermeter_config.bindkey, sizeof(main_notify.bindkey));
        //main_notify.encrypted = watermeter_config.encrypted;
        tx_notify = NOTIFY_MAX;
        ble_send_tx();
#if UART_PRINT_DEBUG_ENABLE
        printf("Main notify start\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
    } else if (*in_data == CMD_LOG_NOTIFY) {
        if (!send_log_enable) {
            send_log_enable = true;
            lg_notify = NOTIFY_MAX;
//            if (!log_notify.debug_enabled) {
//                ble_send_log();
//            }
#if UART_PRINT_DEBUG_ENABLE
            printf("Log notify start\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
        }
    } else if (*in_data == CMD_SET_BINDKEY && len == 17) {
        in_data++;
        memcpy(watermeter_config.bindkey, in_data, sizeof(watermeter_config.bindkey));
        if (watermeter_config.encrypted == device_info_encrypt_none) {
            watermeter_config.encrypted = device_info_encrypt;
            bthome_beacon_init();
        }
        write_config();
        set_adv_data();
#if UART_PRINT_DEBUG_ENABLE
        printf("Set new bindkey - \"");
        for(int i = 0; i < 16; i++) {
            printf("%02x", watermeter_config.bindkey[i]);
        }
        printf("\"\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
    } else if (*in_data == CMD_RESET_BINDKEY) {
        watermeter_config.encrypted = device_info_encrypt_none;
        memset(watermeter_config.bindkey, 0, sizeof(watermeter_config.bindkey));
        write_config();
        set_adv_data();
#if UART_PRINT_DEBUG_ENABLE
        printf("Reset BindKey\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
    } else {
#if UART_PRINT_DEBUG_ENABLE
        printf("Unknown or incomplete command 0x%X\r\n", *in_data);
#endif /* UART_PRINT_DEBUG_ENABLE */

    }
}
