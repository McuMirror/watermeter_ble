#ifndef SRC_INCLUDE_APP_ATT_H_
#define SRC_INCLUDE_APP_ATT_H_

///////////////////////////////////// ATT  HANDLER define ///////////////////////////////////////
typedef enum
{
    ATT_H_START = 0,


    //// Gap ////
    /**********************************************************************************************/
    GenericAccess_PS_H,                     //UUID: 2800,   VALUE: uuid 1800
    GenericAccess_DeviceName_CD_H,          //UUID: 2803,   VALUE:              Prop: Read | Notify
    GenericAccess_DeviceName_DP_H,          //UUID: 2A00,   VALUE: device name
    GenericAccess_Appearance_CD_H,          //UUID: 2803,   VALUE:              Prop: Read
    GenericAccess_Appearance_DP_H,          //UUID: 2A01,   VALUE: appearance
    CONN_PARAM_CD_H,                        //UUID: 2803,   VALUE:              Prop: Read
    CONN_PARAM_DP_H,                        //UUID: 2A04,   VALUE: connParameter


    //// gatt ////
    /**********************************************************************************************/
    GenericAttribute_PS_H,                  //UUID: 2800,   VALUE: uuid 1801
    GenericAttribute_ServiceChanged_CD_H,   //UUID: 2803,   VALUE:              Prop: Indicate
    GenericAttribute_ServiceChanged_DP_H,   //UUID: 2A05,   VALUE: service change
    GenericAttribute_ServiceChanged_CCB_H,  //UUID: 2902,   VALUE: serviceChangeCCC


    //// device information ////
    /**********************************************************************************************/
    DeviceInformation_PS_H,                 //UUID: 2800,   VALUE: uuid 180A
    DeviceInformation_ModName_CD_H,         //UUID: 2803,   VALUE:              Prop: Read
    DeviceInformation_ModName_DP_H,         //UUID: 2A24,   VALUE: Model Number String
    DeviceInformation_SerialN_CD_H,         //UUID: 2803,   VALUE:              Prop: Read
    DeviceInformation_SerialN_DP_H,         //UUID: 2A25,   VALUE: Serial Number String
    DeviceInformation_FirmRev_CD_H,         //UUID: 2803,   VALUE:              Prop: Read
    DeviceInformation_FirmRev_DP_H,         //UUID: 2A26,   VALUE: Firmware Revision String
    DeviceInformation_HardRev_CD_H,         //UUID: 2803,   VALUE:              Prop: Read
    DeviceInformation_HardRev_DP_H,         //UUID: 2A27,   VALUE: Hardware Revision String
    DeviceInformation_SoftRev_CD_H,         //UUID: 2803,   VALUE:              Prop: Read
    DeviceInformation_SoftRev_DP_H,         //UUID: 2A28,   VALUE: Software Revision String
    DeviceInformation_ManName_CD_H,         //UUID: 2803,   VALUE:              Prop: Read
    DeviceInformation_ManName_DP_H,         //UUID: 2A29,   VALUE: Manufacturer Name String


    //// battery service ////
    /**********************************************************************************************/
    BATT_PS_H,                              //UUID: 2800,   VALUE: uuid 180f
    BATT_LEVEL_INPUT_CD_H,                  //UUID: 2803,   VALUE:              Prop: Read | Notify
    BATT_LEVEL_INPUT_DP_H,                  //UUID: 2A19    VALUE: batVal
    BATT_LEVEL_INPUT_CCB_H,                 //UUID: 2902,   VALUE: batValCCC

    //// Count service ////
    /**********************************************************************************************/
    COUNT_PS_H,                             //UUID: 2800,    VALUE: uuid 183B
    HOT_LEVEL_INPUT_CD_H,                   //UUID: 2803,    VALUE:              Prop: Read | Notify
    HOT_LEVEL_INPUT_DP_H,                   //UUID: 2AEA,    VALUE: hot count
    HOT_LEVEL_INPUT_CCB_H,                  //UUID: 2902,    VALUE: hotValCCC

    COLD_LEVEL_INPUT_CD_H,                  //UUID: 2803,   VALUE:              Prop: Read | Notify
    COLD_LEVEL_INPUT_DP_H,                  //UUID: 2AEB,   VALUE: cold count
    COLD_LEVEL_INPUT_CCB_H,                 //UUID: 2902,   VALUE: coldValCCC

    //// Ota ////
    /**********************************************************************************************/
    OTA_PS_H,                               //UUID: 2800,   VALUE: telink ota service uuid
    OTA_CMD_OUT_CD_H,                       //UUID: 2803,   VALUE:              Prop: read | write_without_rsp | Notify
    OTA_CMD_OUT_DP_H,                       //UUID: telink ota uuid,  VALUE: otaData
    OTA_CMD_OUT_DESC_H,                     //UUID: 2901,   VALUE: otaName

    //// Custom RxTx ////
    /**********************************************************************************************/
    RxTx_PS_H,                              //UUID: 2800,   VALUE: 1F10 RxTx service uuid
    RxTx_CMD_OUT_CD_H,                      //UUID: 2803,   VALUE:              Prop: read | write_without_rsp
    RxTx_CMD_OUT_DP_H,                      //UUID: 1F1F,   VALUE: RxTxData
    RxTx_CMD_OUT_DESC_H,                    //UUID: 2902,   VALUE: RxTxValueInCCC

    ATT_END_H,

}ATT_HANDLE;

extern int otaWrite(void * p);
extern int RxTxWrite(void * p);
extern uint16_t batteryValueInCCC;
extern uint16_t hotValueInCCC;
extern uint16_t coldValueInCCC;
extern uint16_t RxTxValueInCCC;
void my_att_init(void);
uint32_t size_my_devName();



#endif /* SRC_INCLUDE_APP_ATT_H_ */
