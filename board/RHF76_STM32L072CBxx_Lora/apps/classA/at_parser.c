#include "at_parser.h"
#include "uart-board.h"

#ifndef AT_CMD_BUF_LEN
#define AT_CMD_BUF_LEN 512
#endif

#ifndef UART_RECV_TIMEOUT
#define UART_RECV_TIMEOUT 15000
#endif

static at_uart_ops_t *uart_ops;
static at_table_t *cmd_table;

int at_parser_init(at_uart_ops_t *ops, at_table_t *table)
{
    if (ops && table)
    {
        uart_ops = ops;
        cmd_table = table;

        if(ops->init)
            return ops->init();
        else
            return 0;
    }
    else
        return -1;
}

int at_cmd_handle(void)
{
    uint8_t tmp_buf[AT_CMD_BUF_LEN];
    uint16_t len = 0, i;
    uint32_t n = 0;
    uint16_t cmd_len = 0;

    if(!uart_ops || !cmd_table)
        return -1;

    /*收到\r\n或超时，接收完成*/
    while(n++ < UART_RECV_TIMEOUT)
    {
        if(uart_ops->recv(tmp_buf+len, 1) == 1)
        {
            len++;
            n = 0;
        }
    }

    /*指令结束标志*/
    if(tmp_buf[len-2] == 0x0D/*'\r'*/ && tmp_buf[len-1] == 0x0A/*'\n'*/)
    {
        /*长度不包含\r\n*/
        len -= 2;
    }

    /*查询AT指令，并执行*/
    for(i = 0; cmd_table[i].cmd; i++)
    {
        cmd_len = strlen(cmd_table[i].cmd);
        if(len >= cmd_len)
        {
            #ifdef AT_IGNORE_CASE
            if(strncasecmp((const char*)cmd_table[i].cmd, (const char*)tmp_buf, cmd_len) == 0)
            #else
            if(memcmp((const char*)cmd_table[i].cmd, (const char*)tmp_buf, cmd_len) == 0)
            #endif
            {
                /*将=后的参数输入*/
                return cmd_table[i].func(tmp_buf + cmd_len, len - cmd_len);
            }
        }
    }
    
    return -1;
}
