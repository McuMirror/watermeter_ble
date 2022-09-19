#ifndef SRC_INCLUDE_APP_H_
#define SRC_INCLUDE_APP_H_

extern uint32_t conn_timeout;
extern uint32_t time_sec;
extern uint8_t battery_notify;
extern uint8_t hot_notify;
extern uint8_t cold_notify;
extern uint8_t tx_notify;
extern uint8_t lg_notify;


void user_init_normal(void);
void user_init_deepRetn(void);
void main_loop (void);

#endif /* SRC_INCLUDE_APP_H_ */
