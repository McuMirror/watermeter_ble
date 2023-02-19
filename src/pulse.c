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

_attribute_data_retention_ static water_counter_t hot_counter;
_attribute_data_retention_ static water_counter_t cold_counter;

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

    hot_counter.status = ST_OPEN;
    hot_counter.counter = 0;
    if (!gpio_read(HOT_GPIO)) {
        hot_counter.bit = BIT_COUNT;
        hot_counter.pressed = ST_PRESSED;
    } else {
        hot_counter.bit = 1;
        hot_counter.pressed = ST_RELEASE;
    }

    cold_counter.status = ST_OPEN;
    cold_counter.counter = 0;
    if (!gpio_read(COLD_GPIO)) {
        cold_counter.bit = BIT_COUNT;
        cold_counter.pressed = ST_PRESSED;
    } else {
        cold_counter.bit = 1;
        cold_counter.pressed = ST_RELEASE;
    }

}

_attribute_ram_code_ static void water_counters() {

    if (!gpio_read(HOT_GPIO)) {
        if (hot_counter.bit == BIT_COUNT) {
            hot_counter.status = ST_CLOSE;
        } else {
            hot_counter.bit <<= 1;
        }
    } else {
        if (hot_counter.bit == 1) {
            hot_counter.status = ST_OPEN;
        } else {
            hot_counter.bit >>= 1;
        }
    }
    if (hot_counter.status) {

        if (!hot_counter.pressed) {
            hot_counter.pressed = ST_PRESSED;
            hot_counter.counter++;
        }
    } else {
        hot_counter.pressed = ST_RELEASE;
    }

    if (!gpio_read(COLD_GPIO)) {
        if (cold_counter.bit == BIT_COUNT) {
            cold_counter.status = ST_CLOSE;
        } else {
            cold_counter.bit <<= 1;
        }
    } else {
        if (cold_counter.bit == 1) {
            cold_counter.status = ST_OPEN;
        } else {
            cold_counter.bit >>= 1;
        }
    }
    if (cold_counter.status) {

        if (!cold_counter.pressed) {
            cold_counter.pressed = ST_PRESSED;
            cold_counter.counter++;
        }
    } else {
        cold_counter.pressed = ST_RELEASE;
    }


}

_attribute_ram_code_ uint8_t task_counters() {

    uint8_t save_config = false;

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_10K);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_10K);
#endif

    if ((!gpio_read(HOT_GPIO) && hot_counter.bit == 1) ||
        (gpio_read(HOT_GPIO) && hot_counter.bit == BIT_COUNT) ||
        (!gpio_read(COLD_GPIO) && cold_counter.bit == 1) ||
        (gpio_read(COLD_GPIO) && cold_counter.bit == BIT_COUNT)) {

        for (uint16_t i = 0; i < TASK_COUNT; i++) {
            water_counters();
            sleep_ms(3);
        }

        if (hot_counter.counter) {
            save_config = true;
            /* detect hot counter overflow */
            watermeter_config.counters.hot_water_count =
                    check_counter_overflow(watermeter_config.counters.hot_water_count +
                    (watermeter_config.liters_per_pulse * hot_counter.counter));
            hot_counter.counter = 0;
            if (ble_connected & conn_connect) hot_notify = NOTIFY_MAX;
#if UART_PRINT_DEBUG_ENABLE
            printf("hot counter - %u\r\n", watermeter_config.counters.hot_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
        }

        if (cold_counter.counter) {
            save_config = true;
            /* detect cold counter overflow */
            watermeter_config.counters.cold_water_count =
                    check_counter_overflow(watermeter_config.counters.cold_water_count +
                    (watermeter_config.liters_per_pulse * cold_counter.counter));
            cold_counter.counter = 0;
            if (ble_connected & conn_connect) cold_notify = NOTIFY_MAX;
#if UART_PRINT_DEBUG_ENABLE
            printf("cold counter - %u\r\n", watermeter_config.counters.cold_water_count);
#endif /* UART_PRINT_DEBUG_ENABLE */
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

