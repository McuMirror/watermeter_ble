#include <stdint.h>
#include "common/string.h"
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
//#include "stack/ble/service/ota/ota.h"
#include "drivers/8258/pm.h"

#include "log.h"
#include "ble.h"
#include "app.h"
#include "app_att.h"
#include "battery.h"
#include "pulse.h"
#include "cfg.h"
#include "cmd_parser.h"

_attribute_data_retention_ uint8_t          ble_name[BLE_NAME_SIZE];
_attribute_data_retention_ adv_data_t       adv_data;
_attribute_data_retention_ adv_crypt_data_t adv_crypt_data;
_attribute_data_retention_ main_notify_t    main_notify;
_attribute_data_retention_ uint8_t          mac_public[6], mac_random_static[6];
_attribute_data_retention_ uint16_t         ble_connected = conn_disconnect;
_attribute_data_retention_ uint8_t          ota_is_working = 0;
_attribute_data_retention_ uint8_t          first_start;

_attribute_data_retention_ uint8_t          blt_rxfifo_b[64 * 8] = {0};
_attribute_data_retention_ my_fifo_t        blt_rxfifo = { 64, 8, 0, 0, blt_rxfifo_b,};
_attribute_data_retention_ uint8_t          blt_txfifo_b[40 * 16] = {0};
_attribute_data_retention_ my_fifo_t        blt_txfifo = { 40, 16, 0, 0, blt_txfifo_b,};

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
    ota_is_working = true;
    bls_pm_setManualLatency(0);
    bls_ota_setTimeout(40 * 1000000); // set OTA timeout  40 seconds
#if UART_PRINT_DEBUG_ENABLE
    printf("Start OTA update\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
}

#if UART_PRINT_DEBUG_ENABLE
void app_debug_ota_result(int result) {

    printf("OTA update return code: 0x%X - ", result);

        switch(result) {

            case OTA_SUCCESS:
                printf("OTA_SUCCESS\r\n");
                break;
            case OTA_DATA_PACKET_SEQ_ERR:
                printf("OTA_DATA_PACKET_SEQ_ERR\r\n");
                break;
            case OTA_PACKET_INVALID:
                printf("OTA_PACKET_INVALID\r\n");
                break;
            case OTA_DATA_CRC_ERR:
                printf("OTA_DATA_CRC_ERR\r\n");
                break;
            case OTA_WRITE_FLASH_ERR :
                printf("OTA_WRITE_FLASH_ERR\r\n");
                break;
            case OTA_DATA_UNCOMPLETE:
                printf("OTA_DATA_UNCOMPLETE\r\n");
                break;
            case OTA_FLOW_ERR:
                printf("OTA_FLOW_ERR\r\n");
                break;
            case OTA_FW_CHECK_ERR:
                printf("OTA_FLOW_ERR\r\n");
                break;
            case OTA_VERSION_COMPARE_ERR:
                printf("OTA_VERSION_COMPARE_ERR\r\n");
                break;
            case OTA_PDU_LEN_ERR:
                printf("OTA_PDU_LEN_ERR\r\n");
                break;
            case OTA_FIRMWARE_MARK_ERR:
                printf("OTA_FIRMWARE_MARK_ERR\r\n");
                break;
            case OTA_FW_SIZE_ERR:
                printf("OTA_FW_SIZE_ERR\r\n");
                break;
            case OTA_DATA_PACKET_TIMEOUT:
                printf("OTA_DATA_PACKET_TIMEOUT\r\n");
                break;
            case OTA_TIMEOUT:
                printf("OTA_TIMEOUT\r\n");
                break;
            case OTA_FAIL_DUE_TO_CONNECTION_TERMIANTE:
                printf("OTA_FAIL_DUE_TO_CONNECTION_TERMIANTE\r\n");
                break;
            case OTA_LOGIC_ERR:
                printf("OTA_LOGIC_ERR\r\n");
                break;
            default:
                printf("Unknown\r\n");
                break;
        }

}
#endif /* UART_PRINT_DEBUG_ENABLE */


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

    log_buff_clear();

#if UART_PRINT_DEBUG_ENABLE
    printf("Connect\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */

    ble_connected = conn_connect;
    bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 249, 800);  // 2.5 S
    conn_timeout = time_sec;


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

    ota_is_working = false;

    send_log_enable = false;

    if (ble_connected & conn_delayed_reset) {
        bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
        sleep_ms(2000);
#if UART_PRINT_DEBUG_ENABLE
        printf("Reboot module\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
        start_reboot();
    }

    ble_connected = conn_disconnect;

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
#if UART_PRINT_DEBUG_ENABLE
        printf("Bound, start with whitelist!\r\n");
#endif /* UART_PRINT_DEBUG_ENABLE */
    } else {

        bls_ll_setAdvParam( ADV_INTERVAL_250MS, ADV_INTERVAL_300MS,
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

    adv_data.head.size = (sizeof(adv_head_uuid16_t)-1 + sizeof(bthome_data_t)) & 0xFF;
    adv_data.head.type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT;
    adv_data.head.UUID = ADV_BTHOME_UUID16;
    adv_data.head.device_info = device_info_encrypt_none | device_info_version;

    adv_data.bthome_data.battery_id = BTHomeID_battery;
    adv_data.bthome_data.battery = battery_level;

    adv_data.bthome_data.voltage_id = BTHomeID_voltage;
    adv_data.bthome_data.voltage = battery_mv;

    adv_data.bthome_data.hot_counter_id = BTHomeID_count32;
    adv_data.bthome_data.hot_counter = watermeter_config.counters.hot_water_count;

    adv_data.bthome_data.cold_counter_id  = BTHomeID_count32;
    adv_data.bthome_data.cold_counter = watermeter_config.counters.cold_water_count;

    ///////////////////// Controller Initialization /////////////////////
    blc_ll_initBasicMCU();                      //mandatory
    blc_ll_initStandby_module(mac_public);      //mandatory
    blc_ll_initAdvertising_module(mac_public);  //adv module:        mandatory for BLE slave,
    blc_ll_initConnection_module();             //connection module  mandatory for BLE slave/master
    blc_ll_initSlaveRole_module();              //slave module:      mandatory for BLE slave,

    ///////////////////// Host Initialization /////////////////////
    blc_gap_peripheral_init();    //gap initialization
    my_att_init (); //gatt initialization
    blc_l2cap_register_handler (blc_l2cap_packet_receive);      //l2cap initialization
    blc_smp_peripheral_init();
    blc_smp_setSecurityLevel(No_Security);


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
#if UART_PRINT_DEBUG_ENABLE
    bls_ota_registerResultIndicateCb(app_debug_ota_result);  //debug
#endif /* UART_PRINT_DEBUG_ENABLE */

    bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &ev_adv_timeout);

    if (watermeter_config.encrypted) {
        bthome_beacon_init();
    }

    set_adv_data();
    ev_adv_timeout(0,0,0);

//    bthome_beacon_init();
//    bthome_encrypt_data_beacon();
}

void set_adv_data() {

    if (watermeter_config.encrypted) {
        bthome_encrypt_data_beacon();
        bls_ll_setAdvData((uint8_t*)&adv_crypt_data, sizeof(adv_crypt_data_t));
    } else {
        bls_ll_setAdvData((uint8_t*)&adv_data, sizeof(adv_data_t));
    }


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

void ble_send_tx() {
    bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (uint8_t *)&main_notify, sizeof(main_notify_t));
}

void ble_send_log() {
    bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, (uint8_t *)&log_notify, sizeof(log_notify_t));
}
