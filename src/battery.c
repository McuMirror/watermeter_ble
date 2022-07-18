#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"

#include "battery.h"

uint16_t battery_mv;         /* 2200 ... 3100 mv                 */
uint8_t  battery_level;      /* 0 ... 100 %                      */


// 2200..3100 mv - 0..100%
_attribute_ram_code_ uint8_t get_battery_level(uint16_t battery_mv) {
    uint8_t battery_level = 0;
    if (battery_mv > MIN_VBAT_MV) {
        battery_level = (battery_mv - MIN_VBAT_MV) / ((MAX_VBAT_MV
                - MIN_VBAT_MV) / 100);
        if (battery_level > 100)
            battery_level = 100;
    }
    return battery_level;
}

void low_vbat(void) {
    cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER,
            clock_time() + 120 * CLOCK_16M_SYS_TIMER_CLK_1S); // go deep-sleep 2 minutes
}


_attribute_ram_code_ void check_battery(void) {

    battery_mv = 2600;
//    battery_mv = get_battery_mv();
    if (battery_mv < 2000) {
        low_vbat();
    }
    battery_level = get_battery_level(battery_mv);
}

