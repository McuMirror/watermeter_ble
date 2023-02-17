#include <stdint.h>
#include "tl_common.h"

#include "pulse.h"
#include "cfg.h"
#include "app.h"
#include "log.h"
#include "ble.h"

#define BIT_COUNT   64                          /* number of polls for debounce */
#define TASK_COUNT  (BIT_COUNT/2+BIT_COUNT)     /* task duration                */

#define ST_CLOSE    1
#define ST_OPEN     0
#define ST_PRESSED  1
#define ST_RELEASE  0

_attribute_data_retention_ static status_water_counter_t status_hot_counter;
_attribute_data_retention_ static status_water_counter_t status_cold_counter;
_attribute_data_retention_ uint8_t save_config = false;

_attribute_ram_code_ uint32_t check_counter_overflow(uint32_t check_count) {
    uint32_t count;

    if (check_count >= COUNTERS_OVERFLOW) {
        count = check_count - COUNTERS_OVERFLOW;
#if UART_PRINT_DEBUG_ENABLE
        printf("Counter overflow\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
    } else {
        count = check_count;
    }

    return count;
}

_attribute_ram_code_ void init_pulse() {

    status_hot_counter.status = ST_OPEN;
    if (!gpio_read(HOT_GPIO)) {
        status_hot_counter.bit = BIT_COUNT;
        status_hot_counter.pressed = ST_PRESSED;
    } else {
        status_hot_counter.bit = 1;
        status_hot_counter.pressed = ST_RELEASE;
    }

    status_cold_counter.status = ST_OPEN;
    if (!gpio_read(COLD_GPIO)) {
        status_cold_counter.bit = BIT_COUNT;
        status_cold_counter.pressed = ST_PRESSED;
    } else {
        status_cold_counter.bit = 1;
        status_cold_counter.pressed = ST_RELEASE;
    }

}

_attribute_ram_code_ static void water_counters() {

    if (!gpio_read(HOT_GPIO)) {
        if (status_hot_counter.bit == BIT_COUNT) {
            status_hot_counter.status = ST_CLOSE;
        } else {
            status_hot_counter.bit <<= 1;
        }
    } else {
        if (status_hot_counter.bit == 1) {
            status_hot_counter.status = ST_OPEN;
        } else {
            status_hot_counter.bit >>= 1;
        }
    }
    if (status_hot_counter.status) {

        if (!status_hot_counter.pressed) {
            status_hot_counter.pressed = ST_PRESSED;
            save_config = true;
            /* detect hot counter overflow */
            watermeter_config.counters.hot_water_count =
                    check_counter_overflow(watermeter_config.counters.hot_water_count +
                    watermeter_config.liters_per_pulse);
            hot_notify = NOTIFY_MAX;
#if UART_PRINT_DEBUG_ENABLE
            printf("hot counter - %u\r\n", watermeter_config.counters.hot_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        }
    } else {
        status_hot_counter.pressed = ST_RELEASE;
    }

    if (!gpio_read(COLD_GPIO)) {
        if (status_cold_counter.bit == BIT_COUNT) {
            status_cold_counter.status = ST_CLOSE;
        } else {
            status_cold_counter.bit <<= 1;
        }
    } else {
        if (status_cold_counter.bit == 1) {
            status_cold_counter.status = ST_OPEN;
        } else {
            status_cold_counter.bit >>= 1;
        }
    }
    if (status_cold_counter.status) {

        if (!status_cold_counter.pressed) {
            status_cold_counter.pressed = ST_PRESSED;
            save_config = true;
            /* detect cold counter overflow */
            watermeter_config.counters.cold_water_count =
                    check_counter_overflow(watermeter_config.counters.cold_water_count +
                    watermeter_config.liters_per_pulse);
            cold_notify = NOTIFY_MAX;
#if UART_PRINT_DEBUG_ENABLE
            printf("cold counter - %u\r\n", watermeter_config.counters.cold_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        }
    } else {
        status_cold_counter.pressed = ST_RELEASE;
    }


}

_attribute_ram_code_ uint8_t task_counters() {

    save_config = false;

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_10K);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_10K);
#endif

    if ((!gpio_read(HOT_GPIO) && status_hot_counter.bit == 1) ||
        (gpio_read(HOT_GPIO) && status_hot_counter.bit == BIT_COUNT) ||
        (!gpio_read(COLD_GPIO) && status_cold_counter.bit == 1) ||
        (gpio_read(COLD_GPIO) && status_cold_counter.bit == BIT_COUNT)) {

        for (uint16_t i = 0; i < TASK_COUNT; i++) {
            water_counters();
            sleep_ms(3);
        }

        if (save_config) {
            write_config();
        }

    }

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_1M);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_1M);
#endif

    return save_config;
}

