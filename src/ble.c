#include <stdint.h>
#include "common/string.h"
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "drivers/8258/pm.h"

#include "ble.h"
#include "app.h"
#include "app_att.h"
#include "battery.h"
#include "pulse.h"
#include "cfg.h"
#include "cmd_parser.h"

_attribute_data_retention_ uint8_t    ble_name[BLE_NAME_SIZE];
_attribute_data_retention_ adv_data_t adv_data;
_attribute_data_retention_ uint8_t    mac_public[6], mac_random_static[6];
_attribute_data_retention_ uint8_t    ble_connected = 0;
_attribute_data_retention_ uint8_t    ota_is_working = 0;
_attribute_data_retention_ uint8_t    first_start;

_attribute_data_retention_ uint8_t    blt_rxfifo_b[64 * 8] = {0};
_attribute_data_retention_ my_fifo_t  blt_rxfifo = { 64, 8, 0, 0, blt_rxfifo_b,};
_attribute_data_retention_ uint8_t    blt_txfifo_b[40 * 16] = {0};
_attribute_data_retention_ my_fifo_t  blt_txfifo = { 40, 16, 0, 0, blt_txfifo_b,};

#if UART_PRINT_DEBUG_ENABLE
void print_mac(uint8_t num, uint8_t *mac) {
    printf("MAC%u from whitelist: ", num);
    for (uint8_t i = 0; i < 6; i++) {
        printf("0x%X ", mac[i]);
    }
    printf("\r\n");

}
#endif /* UART_PRINT_DEBUG_ENABLE */


void app_enter_ota_mode(void)
{
    ota_is_working = 1;
    bls_ota_setTimeout(5 * 1000000);
}

int RxTxWrite(void * p)
{
    cmd_parser(p);
    return 0;
}

_attribute_ram_code_ static void suspend_enter_cb(u8 e, u8 *p, int n) {
    (void) e; (void) p; (void) n;
    cpu_set_gpio_wakeup(HOT_GPIO, gpio_read(HOT_GPIO)? Level_Low : Level_High, 1);  // pad wakeup deepsleep enable
    cpu_set_gpio_wakeup(COLD_GPIO, gpio_read(COLD_GPIO)? Level_Low : Level_High, 1);  // pad wakeup deepsleep enable
    bls_pm_setWakeupSource(PM_WAKEUP_PAD | PM_WAKEUP_TIMER);  // gpio pad wakeup suspend/deepsleep
}

_attribute_ram_code_ void suspend_exit_cb (uint8_t e, uint8_t *p, int n)
{
    rf_set_power_level_index (RF_POWER_P3p01dBm);
}

void ble_connect_cb(uint8_t e, uint8_t *p, int n) {

#if UART_PRINT_DEBUG_ENABLE
    printf("Connect\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

    ble_connected = 1;
    bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 249, CONN_TIMEOUT_8S);  // 2.5 S
    conn_timeout = clock_time();

}

void ble_disconnect_cb(uint8_t e,uint8_t *p, int n) {

#if UART_PRINT_DEBUG_ENABLE
    printf("Disconnect\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

    if (first_start) {
        u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
        /* get latest device info */
        if (bond_number) {
#if UART_PRINT_DEBUG_ENABLE
            printf("Bonded\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
            first_start = 0;
            ev_adv_timeout(0, 0, 0);
        }
    }

    ble_connected = 0;
    ota_is_working = 0;
}

void ev_adv_timeout(u8 e, u8 *p, int n) {
    smp_param_save_t  bondInfo;
    u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
    /* get latest device info */
    if (bond_number) {
        bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )
    }

    ll_whiteList_reset();     //clear whitelist
    ll_resolvingList_reset(); //clear resolving list

    /* use whitelist to filter master device */
    if (bond_number) {
        //if master device use RPA(resolvable private address), must add irk to resolving list
        if (IS_RESOLVABLE_PRIVATE_ADDR(bondInfo.peer_addr_type, bondInfo.peer_addr)){
            /* resolvable private address, should add peer irk to resolving list */
            ll_resolvingList_add(bondInfo.peer_id_adrType, bondInfo.peer_id_addr, bondInfo.peer_irk, NULL);  //no local IRK
            ll_resolvingList_setAddrResolutionEnable(ON);
        } else {
            //if not resolvable random address, add peer address to whitelist
            ll_whiteList_add(bondInfo.peer_addr_type, bondInfo.peer_addr);
        }

        bls_ll_setAdvParam( ADV_INTERVAL_MIN, ADV_INTERVAL_MAX,
                            ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
                            0,  NULL, BLT_ENABLE_ADV_ALL, ADV_FP_ALLOW_SCAN_ANY_ALLOW_CONN_WL);
    } else {

        bls_ll_setAdvParam( ADV_INTERVAL_MIN, ADV_INTERVAL_MAX,
                            ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
                            0,  NULL, BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
        first_start = 1;
#if UART_PRINT_DEBUG_ENABLE
        printf("Not bound, first start!\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

    }

    bls_ll_setScanRspData((uint8_t *) ble_name, ble_name[0]+1);
    bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //ADV enable
}


extern attribute_t my_Attributes[ATT_END_H];
const char* hex_ascii = {"0123456789ABCDEF"};

void get_ble_name() {
    uint8_t *blename = watermeter_config.ble_name;
    uint8_t pos = sizeof(DEV_NAME_STR)-1;

    memcpy(ble_name+2, DEV_NAME_STR, pos);
    pos +=2;
    ble_name[pos++] = '_';
    ble_name[pos++] = hex_ascii[mac_public[2]>>4];
    ble_name[pos++] = hex_ascii[mac_public[2] &0x0f];
    ble_name[pos++] = hex_ascii[mac_public[1]>>4];
    ble_name[pos++] = hex_ascii[mac_public[1] &0x0f];
    ble_name[pos++] = hex_ascii[mac_public[0]>>4];
    ble_name[pos++] = hex_ascii[mac_public[0] &0x0f];
    ble_name[0]     = BLE_NAME_SIZE-1;
    ble_name[1]     = GAP_ADTYPE_LOCAL_NAME_COMPLETE;

    if (*blename == 0) {
        memcpy(blename, ble_name, ble_name[0]+1);
    } else {
        if (memcmp(ble_name, blename, BLE_NAME_SIZE)) {
            memcpy(blename, ble_name, BLE_NAME_SIZE);
        }
    }
    my_Attributes[GenericAccess_DeviceName_DP_H].attrLen = ble_name[0] - 1;
}

__attribute__((optimize("-Os"))) void init_ble(void) {

    blc_initMacAddress(FLASH_SECTOR_MAC, mac_public, mac_random_static);
    /// if bls_ll_setAdvParam( OWN_ADDRESS_RANDOM ) ->  blc_ll_setRandomAddr(mac_random_static);
    get_ble_name();

    adv_data.flg_size  = 0x02;              /* size  */
    adv_data.flg_type  = GAP_ADTYPE_FLAGS;  /* 0x01  */
    adv_data.flg       = 0x06;              /* flags */

    adv_data.head.size = (sizeof(adv_head_uuid16_t) + 3 + sizeof(adv_battery_t) + sizeof(adv_counter_t)*2 - 1) & 0xFF;
    adv_data.head.type =  GAP_ADTYPE_SERVICE_DATA_UUID_16BIT;
    adv_data.head.UUID = ADV_HA_BLE_NS_UUID16;

    adv_data.type_len  = HaBleType_uint | ((sizeof(adv_data.id) + sizeof(adv_data.pid)) & 0x1F);
    adv_data.id        = HaBleID_PacketId;
    adv_data.pid       = 0;

    adv_data.adv_battery.type_len = HaBleType_uint |
            ((sizeof(adv_data.adv_battery.id) + sizeof(adv_data.adv_battery.level)) & 0x1F);
    adv_data.adv_battery.id       = HaBleID_battery;
    adv_data.adv_battery.level = battery_level;

    adv_data.adv_hot.type_len = HaBleType_uint |
            ((sizeof(adv_data.adv_hot.id) + sizeof(adv_data.adv_hot.counter)) & 0x1F);
    adv_data.adv_hot.id  = HaBleID_count;
    adv_data.adv_hot.counter = watermeter_config.counters.hot_water_count;

    adv_data.adv_cold.type_len = HaBleType_uint |
            ((sizeof(adv_data.adv_cold.id) + sizeof(adv_data.adv_cold.counter)) & 0x1F);
    adv_data.adv_cold.id  = HaBleID_count;
    adv_data.adv_cold.counter = watermeter_config.counters.cold_water_count;

    ///////////////////// Controller Initialization /////////////////////
    blc_ll_initBasicMCU();                      //mandatory
    blc_ll_initStandby_module(mac_public);      //mandatory
    blc_ll_initAdvertising_module(mac_public);  //adv module:        mandatory for BLE slave,
    blc_ll_initConnection_module();             //connection module  mandatory for BLE slave/master
    blc_ll_initSlaveRole_module();              //slave module:      mandatory for BLE slave,
    blc_ll_initPowerManagement_module();        //pm module:         optional

    ///////////////////// Host Initialization /////////////////////
    blc_gap_peripheral_init();    //gap initialization
    my_att_init (); //gatt initialization
    blc_l2cap_register_handler (blc_l2cap_packet_receive);      //l2cap initialization
//    blc_smp_setSecurityLevel(No_Security);
    blc_smp_peripheral_init();


    ///////////////////// USER application initialization ///////////////////
    rf_set_power_level_index(MY_RF_POWER_INDEX);
    bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_ENTER, &suspend_enter_cb);
    bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &suspend_exit_cb);
    bls_app_registerEventCallback(BLT_EV_FLAG_CONNECT, &ble_connect_cb);
    bls_app_registerEventCallback(BLT_EV_FLAG_TERMINATE, &ble_disconnect_cb);

    ///////////////////// Power Management initialization///////////////////
    blc_ll_initPowerManagement_module();
    bls_pm_setSuspendMask(SUSPEND_DISABLE);
    blc_pm_setDeepsleepRetentionThreshold(40, 18);
    blc_pm_setDeepsleepRetentionEarlyWakeupTiming(200); // 240
    blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);

    bls_ota_clearNewFwDataArea(); //must
    bls_ota_registerStartCmdCb(app_enter_ota_mode);

    bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &ev_adv_timeout);
    set_adv_data();
    ev_adv_timeout(0,0,0);
}

void set_adv_data() {

#if UART_PRINT_DEBUG_ENABLE
    printf("set_adv_data()\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

    adv_data.pid = (adv_data.pid+1) & 0xFF;

    bls_ll_setAdvData((uint8_t*)&adv_data, sizeof(adv_data_t));
}

void ble_send_battery() {
    bls_att_pushNotifyData(BATT_LEVEL_INPUT_DP_H, (uint8_t *)&battery_level, sizeof(battery_level));
}

void ble_send_hotwater() {
    bls_att_pushNotifyData(HOT_LEVEL_INPUT_DP_H, (uint8_t *)&watermeter_config.counters.hot_water_count,
            sizeof(watermeter_config.counters.hot_water_count));
}

void ble_send_coldwater() {
    bls_att_pushNotifyData(COLD_LEVEL_INPUT_DP_H, (uint8_t *)&watermeter_config.counters.cold_water_count,
            sizeof(watermeter_config.counters.cold_water_count));
}


