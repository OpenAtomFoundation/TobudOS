/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/
#include "ch20_parser.h"

static ch20_parser_ctrl_t ch20_parser_ctrl;

static k_stack_t ch20_parser_task_stack[CH20_PARSER_TASK_STACK_SIZE];

static uint8_t ch20_parser_buffer[CH20_PARSER_BUFFER_SIZE];

/**
 * @brief   向ch20解析器中送入一个字节数据
 * @param   data  送入的数据
 * @retval  none
 * @note    需要用户在串口中断函数中手动调用
*/
void ch20_parser_input_byte(uint8_t data)
{
    if (tos_chr_fifo_push(&ch20_parser_ctrl.parser_rx_fifo, data) == K_ERR_NONE) {
        /* 送入数据成功，释放信号量，计数 */
        tos_sem_post(&ch20_parser_ctrl.parser_rx_sem);
    }
}

/**
 * @brief   ch20解析器从chr fifo中取出一个字节数据
 * @param   none
 * @retval  正常返回读取数据，错误返回-1
*/
static int ch20_parser_getchar(void)
{
    uint8_t chr;
    k_err_t err;
    
    /* 永久等待信号量，信号量为空表示chr fifo中无数据 */
    if (tos_sem_pend(&ch20_parser_ctrl.parser_rx_sem, TOS_TIME_FOREVER) != K_ERR_NONE) {
        return -1;
    }
    
    /* 从chr fifo中取出数据 */
    err = tos_chr_fifo_pop(&ch20_parser_ctrl.parser_rx_fifo, &chr);

    return err == K_ERR_NONE ? chr : -1;
}

/**
 * @brief   ch20读取传感器原始数据并解析
 * @param   void
 * @retval  解析成功返回0，解析失败返回-1
*/
static int ch20_parser_read_raw_data(ch20_data_t *ch20_data)
{
    uint8_t data;
    uint8_t data_h, data_l;
    uint8_t check_sum_cal = 0x17;
   
    /* 读取气体浓度单位 */
    data = ch20_parser_getchar();
    if (data != 0x04) {
        return -1;
    }
    CH20_DEBUG_LOG("--->[%#02x]\r\n", data);
    check_sum_cal += data;
    
    /* 读取小数位数 */
    data = ch20_parser_getchar();
    if (data != 0x00) {
        return -1;
    }
    CH20_DEBUG_LOG("--->[%#02x]\r\n", data);
    check_sum_cal += data;    
    
    /* 读取气体浓度高位 */
    data = ch20_parser_getchar();
    if (data == 0xFF) {
        return -1;
    }
    CH20_DEBUG_LOG("--->[%#02x]\r\n", data);
    data_h = data;
    check_sum_cal += data;
    
    /* 读取气体浓度低位 */
    data = ch20_parser_getchar();
    if (data == 0xFF) {
        return -1;
    }
    CH20_DEBUG_LOG("--->[%#02x]\r\n", data);
    data_l = data;
    check_sum_cal += data;
    
    /* 读取满量程高位 */
    data = ch20_parser_getchar();
    if (data != 0x07) {
        return -1;
    }
    CH20_DEBUG_LOG("--->[%#02x]\r\n", data);
    check_sum_cal += data;
    
    /* 读取满量程低位 */
    data = ch20_parser_getchar();
    if (data != 0xD0) {
        return -1;
    }
    CH20_DEBUG_LOG("--->[%#02x]\r\n", data);
    check_sum_cal += data;

    /* 和校验 */
    data = ch20_parser_getchar();
    CH20_DEBUG_LOG("--->[%#02x]\r\n", data);
    check_sum_cal = ~(check_sum_cal) + 1;
    CH20_DEBUG_LOG("check_sum_cal is 0x%02x \r\n", check_sum_cal);
    if (check_sum_cal != data) {
        return -1;
    }
    
    /* 存储数据 */
    ch20_data->data = (data_h << 8) + data_l;
    CH20_DEBUG_LOG("ch20_data->data is 0x%04x\r\n", ch20_data->data);
    
    return 0;
}

extern k_mail_q_t mail_q;
ch20_data_t     ch20_data;

/**
 * @brief   ch20解析器任务
*/
static void ch20_parser_task_entry(void *arg)
{
    int chr, last_chr = 0;
    
    while (1) {
       
        chr = ch20_parser_getchar();
        if (chr < 0) {
            printf("parser task get char fail!\r\n");
            continue;
        }
        
        if (chr == 0x17 && last_chr == 0xFF) {
            /* 解析到包头 */
            if (0 ==  ch20_parser_read_raw_data(&ch20_data)) {
                /* 正常解析之后通过邮箱发送 */
                tos_mail_q_post(&mail_q, &ch20_data, sizeof(ch20_data_t));
            }
        }
        
        last_chr = chr;
    }
}

/**
 * @brief   初始化ch20解析器
 * @param   none
 * @retval  全部创建成功返回0，任何一个创建失败则返回-1
*/
int ch20_parser_init(void)
{
    k_err_t ret;
    
    memset((ch20_parser_ctrl_t*)&ch20_parser_ctrl, 0, sizeof(ch20_parser_ctrl));
    
    /* 创建 chr fifo */
    ret = tos_chr_fifo_create(&ch20_parser_ctrl.parser_rx_fifo, ch20_parser_buffer, sizeof(ch20_parser_buffer));
    if (ret != K_ERR_NONE) {
        printf("ch20 parser chr fifo create fail, ret = %d\r\n", ret);
        return -1;
    }
    
    /* 创建信号量 */
    ret = tos_sem_create(&ch20_parser_ctrl.parser_rx_sem, 0);
    if (ret != K_ERR_NONE) {
        printf("ch20 parser_rx_sem create fail, ret = %d\r\n", ret);
        return -1;
    }
    
    /* 创建线程 */
    ret = tos_task_create(&ch20_parser_ctrl.parser_task, "ch20_parser_task", 
                          ch20_parser_task_entry, NULL, CH20_PARSER_TASK_PRIO,
                          ch20_parser_task_stack,CH20_PARSER_TASK_STACK_SIZE,0);
    if (ret != K_ERR_NONE) {
        printf("ch20 parser task create fail, ret = %d\r\n", ret);
        return -1;
    }

    return 0;
}
