#ifndef SRC_INCLUDE_BATTERY_H_
#define SRC_INCLUDE_BATTERY_H_

#include <stdint.h>
#include "app_adc.h"

#define MAX_VBAT_MV     3100 // 3100 mV - > battery = 100%
#define MIN_VBAT_MV     2200 // 2200 mV - > battery = 0%

#define get_battery_mv() get_adc_mv(SHL_ADC_VBAT)

extern uint16_t battery_mv;
extern uint8_t  battery_level;


uint8_t get_battery_level(uint16_t battery_mv);
void check_battery(void);

#endif /* SRC_INCLUDE_BATTERY_H_ */
