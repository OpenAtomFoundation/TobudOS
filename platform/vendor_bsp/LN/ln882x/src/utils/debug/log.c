#include "ln88xx.h"
#include "proj_config.h"
#include "utils/debug/log.h"
#include "serial/serial.h"

#if (PRINT_TO_RTT == ENABLE)
  #include "utils/debug/RTT/SEGGER_RTT.h"
#endif

#define LOG_PORT_BAUDRATE  CFG_UART_BAUDRATE_LOG

Serial_t m_LogSerial;
#if (defined(AT_LOG_MERGE_TO_UART0) && (AT_LOG_MERGE_TO_UART0 == 1))
extern Serial_t m_ConsoleSerial;
#endif // !AT_LOG_MERGE_TO_UART0


int log_stdio_write(char *buf, size_t size)
{
    int ret = 0;
    #if (PRINT_TO_RTT == ENABLE)
        ret = SEGGER_RTT_Write(0, (const void *)buf, size);
    #else
		#if WIFI_TRACK
				ret = serial_write(&m_LogSerial, (const void *)buf, size);
        #elif (defined(AT_LOG_MERGE_TO_UART0) && (AT_LOG_MERGE_TO_UART0 == 1))
                ret = serial_write(&m_ConsoleSerial, (const void *)buf, size);
        #else
            // normal mode: log--uart0, at--uart1.
                ret = serial_write(&m_LogSerial, (const void *)buf, size);
        #endif
    #endif
    return ret;
}
void hexdump(uint8_t level, uint8_t *info, void *buff, uint32_t count)
{
    uint32_t i;
    uint8_t *pbuff = (uint8_t *)buff;
    LOG(level, "%s: ", info);
    for(i = 0; i < count; i++){
        LOG(level, "%02x ", *(pbuff+i));
    }
    LOG(level, "\r\n");
}

/**
 * @brief It's better to call `at_init()` before `log_init()`.
 */
void log_init(void)
{
#if (PRINT_TO_RTT == ENABLE)
    SEGGER_RTT_Init();
#else
    #if WIFI_TRACK
        serial_init(&m_LogSerial, SER_PORT_UART1, LOG_PORT_BAUDRATE, NULL);
    #elif (defined(AT_LOG_MERGE_TO_UART0) && (AT_LOG_MERGE_TO_UART0 == 1))
    // in this mode, no need to reinitial UART0 again.
    #else
    // normal mode.
        serial_init(&m_LogSerial, SER_PORT_UART0, LOG_PORT_BAUDRATE, NULL);//2000000 SER_PORT_UART1
    #endif // !AT_LOG_MERGE_TO_UART0

#endif
}



