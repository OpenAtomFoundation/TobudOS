#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include "proj_config.h"

#include "types.h"
#ifdef __CONFIG_OS_KERNEL
  #include "osal/osal.h"
#endif
#include "utils/wrap_stdio.h"

/* extern function */
#define	CMD_MAXARGS	   4
#if (TCP_TEST_DATA_FROM_CONSOLE==1)
#define CMD_CBSIZE     256*4
#define CMD_PBSIZE     256*4
#else
#define CMD_CBSIZE     256
#define CMD_PBSIZE     256
#endif
#define CMD_END_MARK_CR   ("\r")
#define CMD_END_MARK_LF   ("\n")
#define CMD_END_MARK_CRLF ("\r\n")


typedef struct
{
    char	console_cmd_buffer[CMD_CBSIZE];
    char	cmd_line[CMD_PBSIZE];
    int		index;
    bool    echo;
#ifdef __CONFIG_OS_KERNEL
    OS_Semaphore_t              sem;
#endif
} CONSOLE_CTRL_T;


int command_stdio_write(char *buf, size_t size);
extern void console_echo_enable(bool en);
extern void console_init(void);
extern void console_exe_command(char *cmd, unsigned int cmd_len);

#if (CHIP_ROLE == CHIP_MCU)
#define console_printf(format, ...)     __wrap_sprintf((stdio_write_fn)command_stdio_write, format, ##__VA_ARGS__)
#else
#define console_printf(format, ...)     __wrap_sprintf((stdio_write_fn)NULL, format, ##__VA_ARGS__)
#endif

#endif /* end console.h */
