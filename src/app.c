#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "drivers/8258/pm.h"
#include "drivers/8258/timer.h"

#include "app.h"
#include "app_att.h"
#include "battery.h"
#include "pulse.h"
#include "cfg.h"
#include "log.h"
#include "ble.h"

#if UART_PRINT_DEBUG_ENABLE
//_attribute_data_retention_ static uint32_t deepRetn_count = 0;
#endif /* UART_PRINT_DEBUG_ENABLE */

#define UPDATE_PERIOD       5000UL      /* 5 sec */
#define BATTERY_PERIOD      300000UL    /* 5 min */
#define CONN_TIMEOUT        300         /* 5 min */
#define RESET_WL_TIMEOUT    5           /* 5 sec */

_attribute_data_retention_ uint32_t update_interval;
_attribute_data_retention_ uint32_t battery_interval;
_attribute_data_retention_ uint32_t conn_timeout;

_attribute_data_retention_ uint32_t time_sec_tick;
_attribute_data_retention_ uint32_t time_tick_step = CLOCK_16M_SYS_TIMER_CLK_1S; // adjust time clock (in 1/16 us for 1 sec)
_attribute_data_retention_ uint32_t time_sec = 0;

_attribute_data_retention_ uint8_t battery_notify = NOTIFY_MAX;
_attribute_data_retention_ uint8_t hot_notify = NOTIFY_MAX;
_attribute_data_retention_ uint8_t cold_notify = NOTIFY_MAX;
_attribute_data_retention_ uint8_t tx_notify = 0;
_attribute_data_retention_ uint8_t lg_notify = 0;

_attribute_data_retention_ uint8_t  reset_wl_begin;
_attribute_data_retention_ uint32_t  reset_wl_timeout;

_attribute_ram_code_ void check_reset_wl() {

//    gpio_setup_up_down_resistor(RWL_GPIO, PM_PIN_PULLUP_1M);

    if (!gpio_read(RWL_GPIO)) {
        if (reset_wl_begin) {
            if ((time_sec - reset_wl_timeout) > (RESET_WL_TIMEOUT)) {
#if UART_PRINT_DEBUG_ENABLE
                printf("Reset whitelist\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
                bls_smp_eraseAllParingInformation();
                ev_adv_timeout(0,0,0);
                reset_wl_begin = false;
            }
        } else {
            reset_wl_timeout = time_sec;
            reset_wl_begin = true;
        }
    } else {
        reset_wl_begin = false;
    }

//    gpio_setup_up_down_resistor(RWL_GPIO, PM_PIN_UP_DOWN_FLOAT);

}

void user_init_normal(void) {

    #if UART_PRINT_DEBUG_ENABLE
    printf("Start user_init_normal()\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

    adc_power_on_sar_adc(OFF);
    check_battery();
	random_generator_init();  //this is must
    init_pulse();
    init_config();
    init_ble();
    init_log();
    update_interval  = clock_time();
    battery_interval = clock_time();

    reset_wl_begin = false;

}

_attribute_ram_code_ void user_init_deepRetn(void) {

#if UART_PRINT_DEBUG_ENABLE
        //printf("%u Start user_init_deeptn()\r\n", ++deepRetn_count);
#endif /* UART_PRINT_DEBUG_ENABLE */

	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);

	blc_ll_recoverDeepRetention();

	irq_enable();
}

_attribute_ram_code_ void blt_pm_proc(void)
{
    if(ota_is_working){
        bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN); // SUSPEND_DISABLE
        bls_pm_setManualLatency(0);
    }else{
        bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
    }
}


void main_loop (void) {

    blt_sdk_main_loop();

    while (clock_time() -  time_sec_tick > time_tick_step) {
        time_sec_tick += time_tick_step;
        time_sec++; // + 1 sec
    }

    if (task_counters()) {
        adv_data.bthome_data.hot_counter  = watermeter_config.counters.hot_water_count;
        adv_data.bthome_data.cold_counter = watermeter_config.counters.cold_water_count;
        set_adv_data();
    }

    if(!ota_is_working) {

        check_reset_wl();

        if(blc_ll_getCurrentState() & BLS_LINK_STATE_CONN) {
            if(blc_ll_getTxFifoNumber() < 9) {
                if (RxTxValueInCCC) {
                    if (tx_notify) {
                        ble_send_tx();
                        tx_notify--;
                    } else {
                        if (send_log_enable ) {
                            if (lg_notify) {
                                ble_send_log();
                                lg_notify--;
                            } else {
                                if (log_notify.debug_enabled && log_available()) {
                                    set_log_str();
                                }
                            }
                        }
                    }
                }

                if (batteryValueInCCC) {
                    if (battery_notify) {
                        ble_send_battery();
                        battery_notify--;
                    }
                }

                if (hotValueInCCC) {
                    if (hot_notify) {
                        ble_send_hotwater();
                        hot_notify--;
                    }
                }

                if (coldValueInCCC) {
                    if (cold_notify) {
                        ble_send_coldwater();
                        cold_notify--;
                    }
                }
            }

            /* connection time 5 min. */
            if ((time_sec - conn_timeout) > (CONN_TIMEOUT)) {
#if UART_PRINT_DEBUG_ENABLE
                printf("Connection timeout %u min %u sec.\r\n", CONN_TIMEOUT/60, CONN_TIMEOUT%60);
#endif /* UART_PRINT_DEBUG_ENABLE */
                ble_connected |= conn_delayed_disconnect;
                conn_timeout = time_sec;
            }
        }

        if ((clock_time() - update_interval) > UPDATE_PERIOD*CLOCK_SYS_CLOCK_1MS) {

            if (ble_connected & conn_delayed_disconnect) {
                bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
                return;
            }

            if ((clock_time() - battery_interval) > BATTERY_PERIOD*CLOCK_SYS_CLOCK_1MS) {
                check_battery();

                if (battery_level != adv_data.bthome_data.battery) {
    #if UART_PRINT_DEBUG_ENABLE
                    printf("New battery level - %u, last battery level - %u\r\n", battery_level, adv_data.bthome_data.battery);
    #endif /* UART_PRINT_DEBUG_ENABLE */
                    adv_data.bthome_data.battery = battery_level;
                    battery_notify = NOTIFY_MAX;
                    set_adv_data();
                }
                if (battery_mv != adv_data.bthome_data.voltage) {
                    if ((battery_mv > adv_data.bthome_data.voltage && (battery_mv - adv_data.bthome_data.voltage) > 50) ||
                        (battery_mv < adv_data.bthome_data.voltage && (adv_data.bthome_data.voltage - battery_mv) > 50)) {
    #if UART_PRINT_DEBUG_ENABLE
                        printf("New battery mv - %u, last battery mv - %u\r\n", battery_mv, adv_data.bthome_data.voltage);
    #endif /* UART_PRINT_DEBUG_ENABLE */
                        adv_data.bthome_data.voltage = battery_mv;
                        set_adv_data();
                    }
                }
                battery_interval = clock_time();
            }

            update_interval = clock_time();
        }

    }
    blt_pm_proc();
}


