#ifndef SRC_INCLUDE_CFG_H_
#define SRC_INCLUDE_CFG_H_

#include "stdint.h"

#define ID_CFG  0xFED1410
#define LITERS_PER_PULSE 10      /* How many liters per one pulse    */


typedef struct __attribute__((packed)) _watermeter_config_t {
    uint32_t id;
    uint8_t  active;             /* current active cfg               */
    uint32_t flash_addr;         /* flash page address               */
    uint8_t  liters_per_pulse;   /* liters per pulse                 */
    uint32_t hot_water_count;    /* Last number of liters hot water  */
    uint32_t cold_water_count;   /* Last number of litres cold water */
    uint8_t  ble_name[32];       /* Module name with MAC             */
} watermeter_config_t;

extern watermeter_config_t watermeter_config;

void init_default_config();
bool read_config();
void write_config();
void clear_config();

#endif /* SRC_INCLUDE_CFG_H_ */
