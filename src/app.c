#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "drivers/8258/pm.h"

#include "app.h"
#include "app_att.h"
#include "battery.h"
#include "pulse.h"
#include "cfg.h"
#include "ble.h"

#if UART_PRINT_DEBUG_ENABLE
//_attribute_data_retention_ static uint32t deepRetn_count = 0;
#endif /* UART_PRINT_DEBUG_ENABLE */

#define UPDATE_PERIOD       5000UL      /* 5 sec */
#define BATTERY_PERIOD      300000UL    /* 5 min */
_attribute_data_retention_  uint32_t update_interval  = 0;
_attribute_data_retention_  uint32_t battery_interval = 0;

void user_init_normal(void) {

#if UART_PRINT_DEBUG_ENABLE
    printf("Start user_init_normal()\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

    adc_power_on_sar_adc(OFF);
    check_battery();
	random_generator_init();  //this is must
    pulse_init();
    init_config();
    init_ble();
}

_attribute_ram_code_ void user_init_deepRetn(void) {

#if UART_PRINT_DEBUG_ENABLE
//    printf("%u Start user_init_deeptn()\r\n", ++deepRetn_count);
#endif /* UART_PRINT_DEBUG_ENABLE */

	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);

	blc_ll_recoverDeepRetention();

	irq_enable();
}

_attribute_ram_code_ void blt_pm_proc(void)
{
    if(ota_is_working){
        bls_pm_setSuspendMask(SUSPEND_DISABLE);
        bls_pm_setManualLatency(0);
    }else{
        bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
    }
}


void main_loop (void) {

    if (task_counters()) {
        adv_data.adv_hot.counter  = watermeter_config.counters.hot_water_count;
        adv_data.adv_cold.counter = watermeter_config.counters.cold_water_count;
        set_adv_data();
    }

    if ((clock_time() - update_interval) > UPDATE_PERIOD*CLOCK_SYS_CLOCK_1MS) {

        if ((clock_time() - battery_interval) > BATTERY_PERIOD*CLOCK_SYS_CLOCK_1MS) {
            check_battery();

            if (battery_level != adv_data.adv_battery.level) {
#if UART_PRINT_DEBUG_ENABLE
        printf("b_level - %u, adv_b_level - %u\r\n", battery_level, adv_data.adv_battery.level);
#endif /* UART_PRINT_DEBUG_ENABLE */
                adv_data.adv_battery.level = battery_level;
                set_adv_data();
            }
            battery_interval = clock_time();
        }

        if((blc_ll_getCurrentState() & BLS_LINK_STATE_CONN) && blc_ll_getTxFifoNumber() < 9) {

            if (batteryValueInCCC) {
                ble_send_battery();
            }

            if (hotValueInCCC) {
                ble_send_hotwater();
            }

            if (coldValueInCCC) {
                ble_send_coldwater();
            }
        }

        update_interval = clock_time();
    }

    blt_sdk_main_loop();
    blt_pm_proc();
}


