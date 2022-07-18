#ifndef SRC_INCLUDE_CFG_H_
#define SRC_INCLUDE_CFG_H_

#include "stdint.h"

#define ID_CFG  0xFED1410

typedef struct __attribute__((packed)) _watermeter_config_t {
    uint32_t id;
    uint8_t  active;             /* current active cfg               */
    uint32_t flash_addr;         /* flash page address               */
    uint8_t  liters_per_pulse;   /* liters per pulse                 */
    uint32_t hot_water;          /* Last number of liters hot water  */
    uint32_t cold_water;         /* Last number of litres cold water */
    uint8_t  ble_name[32];       /* Module name with MAC             */
} watermeter_config_t;

void init_default_config();
bool read_config();
void write_config();
void clear_config();
void add_hotwater(uint32_t count);
void add_coldwater(uint32_t count);
uint32_t get_hotwater();
uint32_t get_coldwater();
void set_module_name(uint8_t *name);
const uint8_t *get_module_name();

#endif /* SRC_INCLUDE_CFG_H_ */
