#include "tl_common.h"
#include "drivers/8258/gpio_8258.h"
#include "application/print/u_printf.h"

#include "app_adc.h"
#include "pulse.h"
#include "cfg.h"

//#define BIT_COUNT   32768
#define BIT_COUNT   1024
#define TASK_COUNT  (BIT_COUNT/2+BIT_COUNT)

_attribute_data_retention_ static water_counter_t hot_counter;
_attribute_data_retention_ static water_counter_t cold_counter;

_attribute_ram_code_ void pulse_init() {

    hot_counter.pressed = false;
    hot_counter.state = false;
    hot_counter.bit = 1;
    hot_counter.count = 0;

    cold_counter.pressed = false;
    cold_counter.state = false;
    cold_counter.bit = 1;
    cold_counter.count = 0;
}

_attribute_ram_code_ static void water_counters() {

    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_1M);

    if (!gpio_read(HOT_GPIO)) {
        if (hot_counter.bit == BIT_COUNT) {
            hot_counter.state = true;
        } else {
            hot_counter.bit <<= 1;
        }
    } else {
        if (hot_counter.bit == 1) {
            hot_counter.state = false;
        } else {
            hot_counter.bit >>= 1;
        }
    }

    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_UP_DOWN_FLOAT);

    if (hot_counter.state == true) {

        if (hot_counter.pressed == false) {
            hot_counter.count++;
            hot_counter.pressed = true;
        }
    } else {
        hot_counter.pressed = false;
    }

    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_1M);

    if (!gpio_read(COLD_GPIO)) {
        if (cold_counter.bit == BIT_COUNT) {
            cold_counter.state = true;
        } else {
            cold_counter.bit <<= 1;
        }
    } else {
        if (cold_counter.bit == 1) {
            cold_counter.state = false;
        } else {
            cold_counter.bit >>= 1;
        }
    }

    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_UP_DOWN_FLOAT);

    if (cold_counter.state == true) {

        if (cold_counter.pressed == false) {
            cold_counter.count++;
            cold_counter.pressed = true;
        }
    } else {
        cold_counter.pressed = false;
    }
}

_attribute_ram_code_ bool task_counters() {
    bool save_config = false;

//    do {
        water_counters();
//        printf("hot_counter.bit - 0x%X\r\n", hot_counter.bit);
//    } while (hot_counter.pressed == true || cold_counter.pressed == true);

    if (hot_counter.count) {
        save_config = true;
        add_hotwater(hot_counter.count);
        hot_counter.count = 0;
#if UART_PRINT_DEBUG_ENABLE
        printf("hot counter - %u\r\n", get_hotwater());
#endif /* UART_PRINT_DEBUG_ENABLE */
    }

    if (cold_counter.count) {
        save_config = true;
        add_coldwater(cold_counter.count);
        cold_counter.count = 0;
#if UART_PRINT_DEBUG_ENABLE
        printf("cold counter - %u\r\n", get_coldwater());
#endif /* UART_PRINT_DEBUG_ENABLE */
    }

    return save_config;
}

