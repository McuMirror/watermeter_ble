#include <stdint.h>
#include "common/string.h"
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "drivers/8258/pm.h"

#include "app.h"
#include "app_att.h"
#include "battery.h"
#include "pulse.h"
#include "cfg.h"
#include "util.h"

#define        ADV_INTERVAL_MIN                  ADV_INTERVAL_2S
#define        ADV_INTERVAL_MAX                  ADV_INTERVAL_2S+800


_attribute_data_retention_ uint8_t   blt_rxfifo_b[64 * 8] = {0};
_attribute_data_retention_ my_fifo_t blt_rxfifo = { 64, 8, 0, 0, blt_rxfifo_b,};

_attribute_data_retention_ uint8_t   blt_txfifo_b[40 * 16] = {0};
_attribute_data_retention_ my_fifo_t blt_txfifo = { 40, 16, 0, 0, blt_txfifo_b,};

_attribute_data_retention_ uint8_t   ble_name[32]; /* = {0x12, 0x09,
                                                     'W', 'a', 't', 'e', 'r', 'm', 'e', 't', 'e', 'r',
                                                     '_', '0', '0', '0', '0', '0', '0',};*/

//_attribute_data_retention_ uint8_t advertising_data[] = {
//        21, 0x16, 0x1C, 0x18,                   /* Description   */
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* MAC Address   */
//        0xcc,                                   /* Battery Level */
//        0xdd, 0xdd,                             /* Battery mv    */
//        0x00, 0x00, 0x00, 0x00,                 /* Hot counter   */
//        0x00, 0x00, 0x00, 0x00,                 /* Cold counter  */
//        0x00                                    /* Count         */
//};
//

//typedef struct __attribute__((packed)) _avd_data_t {
//    uint8_t ble_name[19];
//
//};

_attribute_data_retention_ uint8_t advertising_data[] = {
    0x05, 0x09, 'V', 'H', 'I', 'D',
    0x02, 0x01, 0x05,                          // BLE limited discoverable mode and BR/EDR not supported
    0x03, 0x02, 0x0F, 0x18,        // incomplete list of service class UUIDs (0x1812, 0x180F)
};




_attribute_data_retention_ uint8_t mac_public[6], mac_random_static[6];
_attribute_data_retention_ uint8_t ble_connected = 0;
uint8_t ota_is_working = 0;

void app_switch_to_indirect_adv(uint8_t e, uint8_t *p, int n)
{
    bls_ll_setAdvParam(ADV_INTERVAL_MIN, ADV_INTERVAL_MAX,
                       ADV_TYPE_CONNECTABLE_UNDIRECTED,
                       OWN_ADDRESS_PUBLIC, 0,  NULL,
                       BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
    bls_ll_setAdvEnable(BLC_ADV_ENABLE);
}

void app_enter_ota_mode(void)
{
    ota_is_working = 1;
    bls_ota_setTimeout(5 * 1000000);
}



_attribute_ram_code_ void blt_pm_proc(void)
{
    if(ota_is_working){
        bls_pm_setSuspendMask(SUSPEND_DISABLE);
        bls_pm_setManualLatency(0);
    }else{
        bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
    }
}

void ble_disconnect_callback(uint8_t e,uint8_t *p, int n)
{
    ble_connected = 0;
    ota_is_working = 0;
}

_attribute_ram_code_ void user_set_rf_power (uint8_t e, uint8_t *p, int n)
{
    rf_set_power_level_index (RF_POWER_P3p01dBm);
}

void ev_adv_timeout(u8 e, u8 *p, int n) {
    (void) e; (void) p; (void) n;
    bls_ll_setAdvParam(ADV_INTERVAL_MIN, ADV_INTERVAL_MAX,
            ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, 0, NULL,
            BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
    //  set_adv_data();
    bls_ll_setScanRspData((uint8_t *) ble_name, ble_name[0]+1);
    bls_ll_setAdvEnable(BLC_ADV_ENABLE);  //ADV enable
}



void ble_connect_callback(uint8_t e, uint8_t *p, int n)
{
    ble_connected = 1;
//  bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 19, CONN_TIMEOUT_4S);  // 200mS
    bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 99, CONN_TIMEOUT_4S);  // 1 S
//  bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 149, CONN_TIMEOUT_8S);  // 1.5 S
//  bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 199, CONN_TIMEOUT_8S);  // 2 S
//  bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 249, CONN_TIMEOUT_8S);  // 2.5 S
//  bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 299, CONN_TIMEOUT_8S);  // 3 S
}



extern attribute_t my_Attributes[ATT_END_H];
const char* hex_ascii = {"0123456789ABCDEF"};

void get_ble_name() {
    const uint8_t *blename = get_module_name();
    if (blename[0] == 0) {
        ble_name[2]  = 'W';
        ble_name[3]  = 'a';
        ble_name[4]  = 't';
        ble_name[5]  = 'e';
        ble_name[6]  = 'r';
        ble_name[7]  = 'm';
        ble_name[8]  = 'e';
        ble_name[9]  = 't';
        ble_name[10] = 'e';
        ble_name[11] = 'r';
        ble_name[12] = '_';
        ble_name[13] = hex_ascii[mac_public[2]>>4];
        ble_name[14] = hex_ascii[mac_public[2] &0x0f];
        ble_name[15] = hex_ascii[mac_public[1]>>4];
        ble_name[16] = hex_ascii[mac_public[1] &0x0f];
        ble_name[17] = hex_ascii[mac_public[0]>>4];
        ble_name[18] = hex_ascii[mac_public[0] &0x0f];
        ble_name[19] = 0;
        ble_name[0]  = 18;
        ble_name[1]  = GAP_ADTYPE_LOCAL_NAME_COMPLETE;
        set_module_name(ble_name);
    } else {
        memcpy(ble_name, blename, strlen((char*)blename)+1);
    }
    my_Attributes[GenericAccess_DeviceName_DP_H].attrLen = strlen((char*)ble_name) + 1;
    for (int i = 0; i < strlen((char*)ble_name); i++) {
        printf("0x%x - %c\r\n", ble_name[i], ble_name[i]);
    }
}

__attribute__((optimize("-Os"))) void init_ble(void) {

    blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);
    /// if bls_ll_setAdvParam( OWN_ADDRESS_RANDOM ) ->  blc_ll_setRandomAddr(mac_random_static);
    get_ble_name();

    advertising_data[4] = mac_public[5];
    advertising_data[5] = mac_public[4];
    advertising_data[6] = mac_public[3];
    advertising_data[7] = mac_public[2];
    advertising_data[8] = mac_public[1];
    advertising_data[9] = mac_public[0];

    ////// Controller Initialization  //////////
    blc_ll_initBasicMCU();                      //mandatory
    blc_ll_initStandby_module(mac_public);      //mandatory
    blc_ll_initAdvertising_module(mac_public);  //adv module:        mandatory for BLE slave,
    blc_ll_initConnection_module();             //connection module  mandatory for BLE slave/master
    blc_ll_initSlaveRole_module();              //slave module:      mandatory for BLE slave,
    blc_ll_initPowerManagement_module();        //pm module:         optional

    ////// Host Initialization  //////////
    blc_gap_peripheral_init();    //gap initialization
    my_att_init (); //gatt initialization
    blc_l2cap_register_handler (blc_l2cap_packet_receive);      //l2cap initialization
    blc_smp_setSecurityLevel(No_Security);

    ///////////////////// USER application initialization ///////////////////
    bls_ll_setScanRspData((uint8_t *)ble_name, strlen((char*)ble_name));
    bls_ll_setAdvParam(ADV_INTERVAL_MIN, ADV_INTERVAL_MAX,
                       ADV_TYPE_CONNECTABLE_UNDIRECTED,
                       OWN_ADDRESS_PUBLIC, 0,  NULL,
                       BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
    bls_ll_setAdvEnable(BLC_ADV_ENABLE);
    rf_set_power_level_index(MY_RF_POWER_INDEX);
    bls_app_registerEventCallback(BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power);
    bls_app_registerEventCallback(BLT_EV_FLAG_CONNECT, &ble_connect_callback);
    bls_app_registerEventCallback(BLT_EV_FLAG_TERMINATE, &ble_disconnect_callback);

    ///////////////////// Power Management initialization///////////////////
    blc_ll_initPowerManagement_module();
    bls_pm_setSuspendMask(SUSPEND_DISABLE);
    blc_pm_setDeepsleepRetentionThreshold(45, 45);
    blc_pm_setDeepsleepRetentionEarlyWakeupTiming(200); // 240
    blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW32K);

    bls_ota_clearNewFwDataArea(); //must
    bls_ota_registerStartCmdCb(app_enter_ota_mode);

    bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &ev_adv_timeout);
    ev_adv_timeout(0,0,0);

}

void set_adv_data(uint32_t hot, uint32_t cold, uint8_t battery_level, uint16_t battery_mv) {

//    advertising_data[10] = battery_level;
//
//    advertising_data[11] = battery_mv >> 8;
//    advertising_data[12] = battery_mv & 0xff;
//
//    advertising_data[13] = (hot >> 24) & 0xff;
//    advertising_data[14] = (hot >> 16) & 0xff;
//    advertising_data[15] = (hot >> 8) & 0xff;
//    advertising_data[16] = hot & 0xff;
//
//    advertising_data[13] = (cold >> 24) & 0xff;
//    advertising_data[14] = (cold >> 16) & 0xff;
//    advertising_data[15] = (cold >> 8) & 0xff;
//    advertising_data[16] = cold & 0xff;
//
//    advertising_data[21]++;

    bls_ll_setAdvData((uint8_t*)advertising_data, sizeof(advertising_data));
}

void ble_send_battery() {
    bls_att_pushNotifyData(BATT_LEVEL_INPUT_DP_H, (uint8_t *)&battery_level, 1);
}


