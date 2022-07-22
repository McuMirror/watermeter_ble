#ifndef SRC_INCLUDE_BLE_H_
#define SRC_INCLUDE_BLE_H_

#define ADV_HA_BLE_NS_UUID16 0x181C // 16-bit UUID Service 0x181C HA_BLE, no security
#define ADV_HA_BLE_SE_UUID16 0x181E // 16-bit UUID Service 0x181E HA_BLE, security enable

// https://github.com/custom-components/ble_monitor/issues/548
typedef enum {
    HaBleID_PacketId = 0,   //0x00, uint8
    HaBleID_battery,      //0x01, uint8, %
    HaBleID_temperature,  //0x02, sint16, 0.01 °C
    HaBleID_humidity,     //0x03, uint16, 0.01 %
    HaBleID_pressure,     //0x04, uint24, 0.01 hPa
    HaBleID_illuminance,  //0x05, uint24, 0.01 lux
    HaBleID_weight,       //0x06, uint16, 0.01 kg
    HaBleID_weight_s,     //0x07, string, kg
    HaBleID_dewpoint,     //0x08, sint16, 0.01 °C
    HaBleID_count,        //0x09,   uint8/16/24/32
    HaBleID_energy,       //0x0A, uint24, 0.001 kWh
    HaBleID_power,        //0x0B, uint24, 0.01 W
    HaBleID_voltage,      //0x0C, uint16, 0.001 V
    HaBleID_pm2x5,        //0x0D, uint16, kg/m3
    HaBleID_pm10,         //0x0E, uint16, kg/m3
    HaBleID_boolean,      //0x0F, uint8
    HaBleID_switch,       //0x10
    HaBleID_opened        //0x11
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
    uint8_t  size;
    uint8_t  type;       /* 0x16, 16-bit UUID            */
    uint16_t UUID;       /* 0x181C, GATT Service HA_BLE  */
} adv_head_uuid16_t;

typedef struct __attribute__((packed)) _adv_battery_t {
    uint8_t     type_len;               /* HaBleType_uint (5-7 bit) | length (0-4 bit or 0x1F) */
    uint8_t     id;                     /* HaBleID_battery                                     */
    uint8_t     level;
} adv_battery_t;

typedef struct __attribute__((packed)) _adv_counter_t {
    uint8_t     type_len;               /* HaBleType_uint (5-7 bit) | length (0-4 bit or 0x1F) */
    uint8_t     id;                     /* HaBleID_count                                       */
    uint32_t    counter;
} adv_counter_t;

typedef struct __attribute__((packed)) _adv_data_t {
    uint8_t             flg_size;       /* 0x02                     */
    uint8_t             flg_type;       /* 0x01                     */
    uint8_t             flg;            /* 0x06                     */
    adv_head_uuid16_t   head;
    uint8_t             type_len;       /* HaBleType_uint | length  */
    uint8_t             id;             /* HaBleID_PacketId         */
    uint8_t             pid;            /* 0 .. 255                 */
    adv_battery_t       adv_battery;
    adv_counter_t       adv_hot;
    adv_counter_t       adv_cold;
} adv_data_t;

extern uint8_t ble_connected;
extern uint8_t mac_public[6], mac_random_static[6];
extern uint8_t ble_name[32];
extern adv_data_t adv_data;

void init_ble();
void set_adv_data();
void ble_send_battery();
void blt_pm_proc(void);


#endif /* SRC_INCLUDE_BLE_H_ */
