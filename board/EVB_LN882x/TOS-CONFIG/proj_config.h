
#ifndef _PROJ_CONFIG_H_
#define _PROJ_CONFIG_H_

#define DISABLE                 0
#define ENABLE                  1


#define CHIP_SLAVE                          0 //As a device for the master chip
#define CHIP_MCU                            1


#define USE_PLL                              ENABLE

#ifdef USE_PLL 
    #define SOURCE_CLOCK                        (160000000)
#else
    #define SOURCE_CLOCK                        (40000000)
#endif

#define SYSTEM_CLOCK                            (SOURCE_CLOCK)
#define AHBUS_CLOCK                             (SYSTEM_CLOCK)
#define APBUS0_CLOCK                            (AHBUS_CLOCK/2)
#define APBUS1_CLOCK                            (AHBUS_CLOCK/4)
#define APBUS2_CLOCK                            (AHBUS_CLOCK)
#if (USE_PLL == ENABLE)
  #define QSPI_CLK                              (APBUS2_CLOCK/4)
#else
  #define QSPI_CLK                              (APBUS2_CLOCK/2)
#endif

#if ((AHBUS_CLOCK % APBUS0_CLOCK) || (AHBUS_CLOCK % APBUS1_CLOCK))
#error "AHBUS_CLOCK % APBUS0_CLOCK != 0 or AHBUS_CLOCK % APBUS1_CLOCK != 0"
#endif


/*
 * Module enable/disable control
 */
#define FLASH_XIP                             ENABLE
#define CHIP_ROLE                             CHIP_MCU

#define RTOS_FREERTOS						0
#define RTOS_ALIOS   						1
#define RTOS_RT_THREAD     				    2
#define RTOS_UCOS     						3
#define RTOS_TENCENT_OS  					4
#define __CONFIG_OS_KERNEL                  RTOS_TENCENT_OS


#define DEBUG_MODE
#define FULL_ASSERT                         ENABLE

#define PRINT_TO_RTT  0 // 1: print to segger rtt   0: print to uart

/*
 * in arm-none-eabi-gcc/7.3, BIG_ENDIAN is defined as 4321 default.
 */
#ifdef BIG_ENDIAN
  #undef BIG_ENDIAN
#endif

#define LITTLE_ENDIAN 1234


#define BLE_SUPPORT DISABLE

/*
 * Hardware config
 */
#define CFG_UART0_TX_BUF_SIZE  256
#define CFG_UART0_RX_BUF_SIZE  256
#define CFG_UART1_TX_BUF_SIZE  128
#define CFG_UART1_RX_BUF_SIZE  256
#define CFG_UART_BAUDRATE_LOG      2000000
#define CFG_UART_BAUDRATE_CONSOLE  2000000



#endif /* _PROJ_CONFIG_H_ */

