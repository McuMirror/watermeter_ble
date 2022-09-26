#ifndef SRC_INCLUDE_BLE_H_
#define SRC_INCLUDE_BLE_H_

#include "bthome.h"

#define NOTIFY_MAX 2

typedef enum {
    conn_disconnect         = 0x0000,
    conn_connect            = 0x0001,
    conn_delayed_disconnect = 0x0010,
    conn_delayed_reset      = 0x0100
} conn_t;

typedef struct __attribute__((packed)) _adv_head_uuid16_t {
    uint8_t  size;          /* sum sizes adv_head_uuid16_t+adv_pid_t+adv_battery_t+adv_voltage_t+2*adv_counter_t-1 */
    uint8_t  type;          /* 0x16, 16-bit UUID            */
    uint16_t UUID;          /* 0x181C, GATT Service HA_BLE  */
} adv_head_uuid16_t;

typedef struct __attribute__((packed)) _adv_pid_t {
    uint8_t     type_len;   /* HaBleType_uint (5-7 bit) | length (0-4 bit or 0x1F) */
    uint8_t     id;         /* HaBleID_PacketId                                    */
    uint8_t     pid;        /* 0 .. 255                                            */
} adv_pid_t;

typedef struct __attribute__((packed)) _adv_battery_t {
    uint8_t     type_len;   /* HaBleType_uint (5-7 bit) | length (0-4 bit or 0x1F) */
    uint8_t     id;         /* HaBleID_battery                                     */
    uint8_t     level;
} adv_battery_t;

typedef struct __attribute__((packed)) _adv_voltage_t {
    uint8_t     type_len;   /* HaBleType_uint (5-7 bit) | length (0-4 bit or 0x1F) */
    uint8_t     id;         /* HaBleID_voltage                                     */
    uint16_t    voltage;
} adv_voltage_t;

typedef struct __attribute__((packed)) _adv_counter_t {
    uint8_t     type_len;   /* HaBleType_uint (5-7 bit) | length (0-4 bit or 0x1F) */
    uint8_t     id;         /* HaBleID_count                                       */
    uint32_t    counter;
} adv_counter_t;

typedef struct __attribute__((packed)) _adv_data_t {
    uint8_t             flg_size;   /* 0x02                                      */
    uint8_t             flg_type;   /* 0x01                                      */
    uint8_t             flg;        /* 0x06                                      */
    adv_head_uuid16_t   head;
    adv_pid_t           pid;
    adv_battery_t       battery;
    adv_voltage_t       voltage;
    adv_counter_t       hot;
    adv_counter_t       cold;
} adv_data_t;

typedef struct __attribute__((packed)) _main_notify_t {
    uint16_t id;
    uint8_t  liter_per_pulse;
    uint8_t  version;
} main_notify_t;

extern uint16_t ble_connected;
extern uint8_t ota_is_working;
extern uint8_t mac_public[6], mac_random_static[6];
extern uint8_t ble_name[BLE_NAME_SIZE];
extern adv_data_t adv_data;
extern main_notify_t main_notify;

void init_ble();
void set_adv_data();
void ev_adv_timeout(u8 e, u8 *p, int n);
void ble_send_battery();
void ble_send_hotwater();
void ble_send_coldwater();
void ble_send_tx();
void ble_send_log();

#endif /* SRC_INCLUDE_BLE_H_ */
