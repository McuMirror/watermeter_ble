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
_attribute_data_retention_ static uint32_t deepRetn_count = 0;
#endif /* UART_PRINT_DEBUG_ENABLE */

//_attribute_data_retention_	int device_in_connection_state;
//_attribute_data_retention_	u32 advertise_begin_tick;
//_attribute_data_retention_	u32	interval_update_tick;
//_attribute_data_retention_	u8	sendTerminate_before_enterDeep = 0;
//_attribute_data_retention_  u32 latest_user_event_tick;
_attribute_data_retention_  uint32_t battery_measure_tick = 0;
_attribute_data_retention_  uint32_t advertise_begin_tick = 0;

void user_init_normal(void)
{

#if UART_PRINT_DEBUG_ENABLE
    printf("Start user_init_normal()\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

    battery_mv = 3100;

    adc_power_on_sar_adc(OFF);
    check_battery();
	random_generator_init();  //this is must
    pulse_init();

    if (!read_config()) {
        init_default_config();
    }

    init_ble();
}

_attribute_ram_code_ void user_init_deepRetn(void) {

#if UART_PRINT_DEBUG_ENABLE
    printf("%u Start user_init_deepRetn()\r\n", ++deepRetn_count);
#endif /* UART_PRINT_DEBUG_ENABLE */

	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);

	blc_ll_recoverDeepRetention();

	irq_enable();

	#if (UI_KEYBOARD_ENABLE)
		/////////// keyboard gpio wakeup init ////////
		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			cpu_set_gpio_wakeup (pin[i], Level_High,1);  //drive pin pad high wakeup deepsleep
		}
	#elif (UI_BUTTON_ENABLE)

		cpu_set_gpio_wakeup (SW1_GPIO, Level_Low,1);  //button pin pad low wakeUp suspend/deepSleep
		cpu_set_gpio_wakeup (SW2_GPIO, Level_Low,1);  //button pin pad low wakeUp suspend/deepSleep
	#endif
}

#define BATTERY_MEAS_PERIOD 5000UL /* ms */
#define AVD_PERIOD          (ADV_INTERVAL_2S-1200)

_attribute_data_retention_ uint8_t send_battery = 0;
_attribute_data_retention_ uint8_t send_hot     = 0;
_attribute_data_retention_ uint8_t send_cold    = 0;

void main_loop (void) {
    if ((clock_time() - battery_measure_tick) > BATTERY_MEAS_PERIOD*CLOCK_SYS_CLOCK_1MS) {
        check_battery();
        printf("%u - check battery - %u\r\n", clock_time(), battery_level);
        battery_measure_tick = clock_time();
        send_battery = 1;
    }

    if (battery_level != advertising_data.adv_battery.level) {
        advertising_data.adv_battery.level = battery_level;
        set_adv_data();
    }

//    if((blc_ll_getCurrentState() & BLS_LINK_STATE_CONN) && blc_ll_getTxFifoNumber() < 9){
//        if (send_battery) {
//            printf("ble_connected\r\n");
//            ble_send_battery();
//            send_battery = 0;
//        }
//    }

//    if ((clock_time() - advertise_begin_tick) > AVD_PERIOD*CLOCK_SYS_CLOCK_1MS) {
//        set_adv_data();
//        advertise_begin_tick = clock_time();
//        printf("%u - avd start\r\n", clock_time());
//    }

    blt_sdk_main_loop();
    blt_pm_proc();
//    bls_pm_setSuspendMask(SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);

}


