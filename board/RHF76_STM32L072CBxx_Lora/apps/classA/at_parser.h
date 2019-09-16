#ifndef __AT_PARSER_H__
#define __AT_PARSER_H__

#include <stdint.h>
#include <string.h>

#define AT_CMD_END NULL

//#define AT_IGNORE_CASE

/*
 * Please declare and register command table as follow before call
 * at_cmd_handle.
 * NOTE. Add AT_CMD_END as last member to mark the end of command.
 * All receive text behind command text would be treated as data text
 * pass to func.
 * 
 * at_table_t cmd_table[] = {
 *     {"AT+VERSION=", ver_handle},
 *     {"AT+RESET", reset_handle},
 *     ...
 *     {AT_CMD_END, NULL},
 * };
 */

typedef struct
{
    const char *cmd;	                /* 命令文本 */
    uint8_t (*func)(uint8_t *, uint16_t size);	/* 命令处理函数 */
} at_table_t;

/*
 * Please declare and register uart operations so that AT parser is
 * able to receive text from uart.
 *
 * at_uart_ops_t uart_ops = {
 *     .init = NULL,
 *     .deinit = NULL,
 *     .send = NULL,
 *     .recv = UartReceive,
 *     .recv_timeout = UartReceive,
 * };
 */
typedef struct
{
    uint8_t (*init)(void);
    uint8_t (*deinit)(void);
    uint16_t (*send)(uint8_t *buf, uint16_t size);
    uint16_t (*recv)(uint8_t *buf, uint16_t buf_len);
    uint16_t (*recv_timeout)(uint8_t *buf, uint16_t buf_len);
}at_uart_ops_t;

int at_parser_init(at_uart_ops_t *uart_ops, at_table_t *cmd_table);
int at_cmd_handle(void);

#endif
