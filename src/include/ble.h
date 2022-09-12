#ifndef SRC_INCLUDE_BLE_H_
#define SRC_INCLUDE_BLE_H_

#define ADV_HA_BLE_NS_UUID16 0x181C // 16-bit UUID Service 0x181C HA_BLE, no security
#define ADV_HA_BLE_SE_UUID16 0x181E // 16-bit UUID Service 0x181E HA_BLE, security enable
#define NOTIFY_MAX 3

// https://github.com/custom-components/ble_monitor/issues/548
typedef enum {
    HaBleID_PacketId = 0, //0x00, uint8
    HaBleID_battery,      //0x01, uint8, %
    HaBleID_temperature,  //0x02, sint16, 0.01 °C
    HaBleID_humidity,     //0x03, uint16, 0.01 %
    HaBleID_pressure,     //0x04, uint24, 0.01 hPa
    HaBleID_illuminance,  //0x05, uint24, 0.01 lux
    HaBleID_weight,       //0x06, uint16, 0.01 kg
    HaBleID_weight_s,     //0x07, string, kg
    HaBleID_dewpoint,     //0x08, sint16, 0.01 °C
    HaBleID_count,        //0x09, uint8/16/24/32
    HaBleID_energy,       //0x0A, uint24, 0.001 kWh
    HaBleID_power,        //0x0B, uint24, 0.01 W
    HaBleID_voltage,      //0x0C, uint16, 0.001 V
    HaBleID_pm2x5,        //0x0D, uint16, kg/m3
    HaBleID_pm10,         //0x0E, uint16, kg/m3
    HaBleID_boolean,      //0x0F, uint8
    HaBleID_switch,       //0x10
    HaBleID_opening,      //0x11
    HaBleID_co2,          //0x12
    HaBleID_tvoc          //0x13
} HaBleIDs_e;

// Type bit 5-7
typedef enum {
    HaBleType_uint = 0,
    HaBleType_sint = (1<<5),
    HaBleType_float = (2<<5),
    HaBleType_string  = (3<<5),
    HaBleType_MAC  = (4<<5)
} HaBleTypes_e;


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
    uint8_t  version[sizeof(VERSION)];
} main_notify_t;

extern uint8_t ble_connected;
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

#endif /* SRC_INCLUDE_BLE_H_ */
