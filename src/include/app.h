#ifndef SRC_INCLUDE_APP_H_
#define SRC_INCLUDE_APP_H_

#include <stdint.h>

extern uint32_t conn_timeout;
extern uint32_t time_sec;
extern uint8_t battery_change;
extern uint8_t hot_change;
extern uint8_t cold_change;


void user_init_normal(void);
void user_init_deepRetn(void);
void main_loop (void);

#endif /* SRC_INCLUDE_APP_H_ */
