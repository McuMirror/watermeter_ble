#ifndef SRC_INCLUDE_BTHOME_H_
#define SRC_INCLUDE_BTHOME_H_

#define ADV_HA_BLE_NS_UUID16 0x181C /* 16-bit UUID Service 0x181C HA_BLE, no security     */
#define ADV_HA_BLE_SE_UUID16 0x181E /* 16-bit UUID Service 0x181E HA_BLE, security enable */

typedef enum {
    HaBleID_PacketId = 0,       /* 0x00, uint8,  0..255                 */
    HaBleID_battery,            /* 0x01, uint8,  %                      */
    HaBleID_temperature,        /* 0x02, sint16, 0.01 °C                */
    HaBleID_humidity,           /* 0x03, uint16, 0.01 %                 */
    HaBleID_pressure,           /* 0x04, uint24, 0.01 hPa               */
    HaBleID_illuminance,        /* 0x05, uint24, 0.01 lux               */
    HaBleID_mass,               /* 0x06, uint16, 0.01 kg                */
    HaBleID_mass_s,             /* 0x07, uint16, 0.01 lb                */
    HaBleID_dewpoint,           /* 0x08, sint16, 0.01 °C                */
    HaBleID_count,              /* 0x09, uint8/16/24/32                 */
    HaBleID_energy,             /* 0x0A, uint24, 0.001 kWh              */
    HaBleID_power,              /* 0x0B, uint24, 0.01 W                 */
    HaBleID_voltage,            /* 0x0C, uint16, 0.001 V                */
    HaBleID_pm2x5,              /* 0x0D, uint16, ug/m3                  */
    HaBleID_pm10,               /* 0x0E, uint16, ug/m3                  */
    HaBleID_boolean,            /* 0x0F, uint8,  True = On              */
    HaBleID_power_s,            /* 0x10, uint8,  True = On              */
    HaBleID_opening,            /* 0x11, uint8,  False = Closed         */
    HaBleID_co2,                /* 0x12, uint16, ppm                    */
    HaBleID_tvoc,               /* 0x13, uint16, ug/m3                  */
    HaBleID_moisture,           /* 0x14, uint16, 0.01 %                 */
    HaBleID_battery_s,          /* 0x15, uint8,  False = Low            */
    HaBleID_battery_charging,   /* 0x16, uint8,  True = Charging        */
    HaBleID_carbon_monoxide,    /* 0x17, uint8,  False = Not detected   */
    HaBleID_cold,               /* 0x18, uint8,  True = Cold            */
    HaBleID_connectivity,       /* 0x19, uint8,  False = Disconnected   */
    HaBleID_door,               /* 0x1a, uint8,  False = Closed         */
    HaBleID_garage_door,        /* 0x1b, uint8,  True = Open            */
    HaBleID_gas,                /* 0x1c, uint8,  True = Detected        */
    HaBleID_heat,               /* 0x1d, uint8,  False = Normal         */
    HaBleID_light,              /* 0x1e, uint8,  True = Light detected  */
    HaBleID_lock,               /* 0x1f, uint8,  True = Unlocked        */
    HaBleID_moisture_s,         /* 0x20, uint8,  True = Wet             */
    HaBleID_motion,             /* 0x21, uint8,  False = Clear          */
    HaBleID_moving,             /* 0x22, uint8,  True = Moving          */
    HaBleID_occupancy,          /* 0x23, uint8,  True = Detected        */
    HaBleID_plug,               /* 0x24, uint8,  False = Unplugged      */
    HaBleID_pressence,          /* 0x25, uint8,  False = Away           */
    HaBleID_problem,            /* 0x26, uint8,  True = Problem         */
    HaBleID_running,            /* 0x27, uint8,  True = Running         */
    HaBleID_safety,             /* 0x28, uint8,  False = Unsafe         */
    HaBleID_smoke,              /* 0x29, uint8,  True = Detected        */
    HaBleID_sound,              /* 0x2a, uint8,  False = Clear          */
    HaBleID_tamper,             /* 0x2b, uint8,  False = Off            */
    HaBleID_vibration,          /* 0x2c, uint8,  True = Detected        */
    HaBleID_window              /* 0x2d, uint8,  True = Closed          */
} HaBleIDs_t;

// Type bit 5-7
typedef enum {
    HaBleType_uint      = 0,
    HaBleType_sint      = (1<<5),
    HaBleType_float     = (2<<5),
    HaBleType_string    = (3<<5),
    HaBleType_MAC       = (4<<5)
} HaBleTypes_t;

#endif /* SRC_INCLUDE_BTHOME_H_ */
