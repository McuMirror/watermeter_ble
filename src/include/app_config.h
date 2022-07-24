#pragma once

#define ON                          1
#define OFF                         0

#define DEV_NAME_CHR               'W','a','t','e','r','m','e','t','e','r','_','B','L','E'
#define DEV_NAME_STR                "Watermeter_BLE"
/* 0x16,0x09,'W','a','t','e','r','m','e','t','e','r','_','B','L','E','_',0x00,0x00,0x00,0x00,0x00,0x00 */
#define DEV_NAME_SIZE               23
#define VERSION                     0x10    /* BCD format (0x10 -> '1.0')   */

#define UART_PRINT_DEBUG_ENABLE     1       /* if 1 use printf() over uart  */
#define WHITELIST_ENABLE            0       /* if 1 use whitelist           */

#if WHITELIST_ENABLE
#define MAC1                    {0x88, 0x10, 0x8F, 0x2D, 0x06, 0x16}
#define MAC2                    {0x88, 0x10, 0x8F, 0x2D, 0x06, 0x17}
//#define MAC3                    {0x88, 0x10, 0x8F, 0x2D, 0x06, 0x18}
//#define MAC4                    {0x88, 0x10, 0x8F, 0x2D, 0x06, 0x19}
#endif /* WHITELIST_ENABLE */

/************************ Advertising_Interval *************************************/
#define ADV_INTERVAL_MIN        ADV_INTERVAL_2S+800    /* (3200+800) * 0.625 = 2500 ms or 2.5 sec */
#define ADV_INTERVAL_MAX        ADV_INTERVAL_2S+1000   /* (3200+1000)* 0.625 = 2625 ms or 2.6 sec */


#define MY_RF_POWER_INDEX       RF_POWER_P0p04dBm

/**************************** Configure UART ***************************************/
#if UART_PRINT_DEBUG_ENABLE
#define PRINT_BAUD_RATE         115200
#define DEBUG_INFO_TX_PIN       UART_TX_PB1
#define PB1_DATA_OUT            ON
#define PB1_OUTPUT_ENABLE       ON
#define PULL_WAKEUP_SRC_PB1     PM_PIN_PULLUP_1M
#define PB1_FUNC                AS_GPIO
#endif /* UART_PRINT_DEBUG_ENABLE */

/************************* Configure HOT GPIO ***************************************/
#define HOT_GPIO                GPIO_PB6
#define PB6_INPUT_ENABLE        ON
#define PB6_DATA_OUT            OFF
#define PB6_OUTPUT_ENABLE       OFF
#define PB6_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PB6     PM_PIN_PULLUP_1M

/************************* Configure COLD GPIO **************************************/
#define COLD_GPIO               GPIO_PB7
#define PB7_INPUT_ENABLE        ON
#define PB7_DATA_OUT            OFF
#define PB7_OUTPUT_ENABLE       OFF
#define PB7_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PB7     PM_PIN_PULLUP_1M

/************************* Configure VBAT GPIO ***************************************/
#define GPIO_VBAT               GPIO_PC4
#define PC4_INPUT_ENABLE        OFF
#define PC4_OUTPUT_ENABLE       ON
#define PC4_DATA_OUT            ON
#define PC4_FUNC                AS_GPIO
#define SHL_ADC_VBAT            9 //C4P

/************************* For 512K Flash only ***************************************/
#define BEGIN_USER_DATA         0x40000
#define END_USER_DATA           0x74000
#define FLASH_SECTOR_SIZE       0x1000
#define FLASH_SECTOR_MAC        0x76000

/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE      0
#define WATCHDOG_INIT_TIMEOUT       500  //ms

///////////////////////// System Clock  Configuration /////////////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ  								24000000 //16000000

#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	#define SYS_CLK_TYPE  									SYS_CLK_16M_Crystal
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	#define SYS_CLK_TYPE  									SYS_CLK_24M_Crystal
#elif(CLOCK_SYS_CLOCK_HZ == 32000000)
	#define SYS_CLK_TYPE  									SYS_CLK_32M_Crystal
#elif(CLOCK_SYS_CLOCK_HZ == 48000000)
	#define SYS_CLK_TYPE  									SYS_CLK_48M_Crystal
#else
	#error "unsupported system clock !"
#endif

enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};

#include "vendor/common/default_config.h"

