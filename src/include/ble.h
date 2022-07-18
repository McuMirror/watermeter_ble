#ifndef SRC_INCLUDE_BLE_H_
#define SRC_INCLUDE_BLE_H_

extern uint8_t ble_connected;
extern uint8_t mac_public[6], mac_random_static[6];
extern uint8_t ble_name[32];


void init_ble();
void set_adv_data(uint32_t hot, uint32_t cold, uint8_t battery_level, uint16_t battery_mv);
void ble_send_battery();
void blt_pm_proc(void);


#endif /* SRC_INCLUDE_BLE_H_ */
