#include "tos_shell.h"
#include "tos_hal.h"

#define CMD_LEN_MAX     128
char cmd_buf[CMD_LEN_MAX];

hal_uart_t shell_uart;

void uart_output(const char *str)
{
    tos_hal_uart_write(&shell_uart, (const uint8_t *)str, strlen(str), 0xFF);

#if 0
    /* if using c lib printf through uart, a simpler one is: */
    printf(str);
#endif
}

__STATIC__ int cmd_test00(int argc, char *argv[])
{
    int i = 0;
    tos_shell_printf("test00:\n");

    for (i = 0; i < argc; ++i) {
        tos_shell_printf("argv[%d]: %s\n", i, argv[i]);
    }
    return 0;
}

__STATIC__ int cmd_test01(int argc, char *argv[])
{
    tos_shell_printf("test01:\n");
    return 0;
}

__STATIC__ int cmd_test10(int argc, char *argv[])
{
    int i = 0;
    tos_shell_printf("test10:\n");

    for (i = 0; i < argc; ++i) {
        tos_shell_printf("argv[%d]: %s\n", i, argv[i]);
    }
    return 0;
}

__STATIC__ int cmd_test11(int argc, char *argv[])
{
    tos_shell_printf("test11:\n");
    return 0;
}

__STATIC__ const shell_cmd_t custom_shell_cmds0[] = {
    { "test00",     cmd_test00,     "test00 cmd" },
    { "test01",     cmd_test01,     "test01 cmd" },
    { K_NULL,       K_NULL,         K_NULL  }, /* never forget this!!!*/
};

__STATIC__ shell_cmd_set_t custom_shell_cmd_set0 = {
    .list       = TOS_SLIST_NODE(dummy),
    .commands   = custom_shell_cmds0,
};

__STATIC__ const shell_cmd_t custom_shell_cmds1[] = {
    { "test10",     cmd_test10,     "test10 cmd" },
    { "test11",     cmd_test11,     "test11 cmd" },
    { K_NULL,       K_NULL,         K_NULL  }, /* never forget this!!!*/
};

__STATIC__ shell_cmd_set_t custom_shell_cmd_set1 = {
    .list       = TOS_SLIST_NODE(dummy),
    .commands   = custom_shell_cmds1,
};

void application_entry(void *arg)
{
    /* if test on ALIENTEK_STM32F429, switch HAL_UART_PORT_2 to HAL_UART_PORT_1 */
    tos_hal_uart_init(&shell_uart, HAL_UART_PORT_2);

    tos_shell_init(cmd_buf, sizeof(cmd_buf), uart_output, &custom_shell_cmd_set0);

    tos_shell_cmd_set_regiser(&custom_shell_cmd_set1);

    while (1) {
        tos_task_delay(1000);
    }
}

