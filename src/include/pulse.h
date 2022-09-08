#ifndef SRC_INCLUDE_PULSE_H_
#define SRC_INCLUDE_PULSE_H_

#include "stdint.h"

typedef struct _water_conter_t {
    uint8_t  pressed;
    uint8_t  released;
    uint8_t  state;
    uint16_t bit;
    uint16_t count;
} water_counter_t;

void pulse_init();
bool task_counters();
uint32_t check_counter_overflow(uint32_t check_count);

#endif /* SRC_INCLUDE_PULSE_H_ */
