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
#include "pm2d5_parser.h"

static pm2d5_parser_ctrl_t pm2d5_parser_ctrl;

static k_stack_t pm2d5_parser_task_stack[PM2D5_PARSER_TASK_STACK_SIZE];

static uint8_t pm2d5_parser_buffer[PM2D5_PARSER_BUFFER_SIZE];

/**
 * @brief   PM2D5传感器原始数据
 * @note    传感器每次上报32字节，头部0x42和0x4d固定，用于解析包头，帧长度用于判断是否是正常数据包，所以只存储后面的28字节
 */
typedef struct pm2d5_raw_data_st {

    uint8_t     data1_h;
    uint8_t     data1_l;

    uint8_t     data2_h;
    uint8_t     data2_l;

    uint8_t     data3_h;
    uint8_t     data3_l;

    uint8_t     data4_h;
    uint8_t     data4_l;

    uint8_t     data5_h;
    uint8_t     data5_l;

    uint8_t     data6_h;
    uint8_t     data6_l;

    uint8_t     data7_h;
    uint8_t     data7_l;

    uint8_t     data8_h;
    uint8_t     data8_l;

    uint8_t     data9_h;
    uint8_t     data9_l;

    uint8_t     data10_h;
    uint8_t     data10_l;

    uint8_t     data11_h;
    uint8_t     data11_l;

    uint8_t     data12_h;
    uint8_t     data12_l;

    uint8_t     data13_h;   //版本号
    uint8_t     data13_l;   //错误代码

    uint8_t     chk_sum_h;  //和校验高8位
    uint8_t     chk_sum_l;  //和校验低8位
} pm2d5_raw_data_t;

typedef union pm2d5_raw_data_un {
    uint8_t data[30];
    pm2d5_raw_data_t pm2d5_raw_data;
}pm2d5_raw_data_u;

/**
 * @brief   向PM2D5解析器中送入一个字节数据
 * @param   data  送入的数据
 * @retval  none
 * @note    需要用户在串口中断函数中手动调用
*/
void pm2d5_parser_input_byte(uint8_t data)
{
    if (tos_chr_fifo_push(&pm2d5_parser_ctrl.parser_rx_fifo, data) == K_ERR_NONE) {
        /* 送入数据成功，释放信号量，计数 */
        tos_sem_post(&pm2d5_parser_ctrl.parser_rx_sem);
    }
}

/**
 * @brief   PM2D5解析器从chr fifo中取出一个字节数据
 * @param   none
 * @retval  正常返回读取数据，错误返回-1
*/
static int pm2d5_parser_getchar(void)
{
    uint8_t chr;
    k_err_t err;
    
    /* 永久等待信号量，信号量为空表示chr fifo中无数据 */
    if (tos_sem_pend(&pm2d5_parser_ctrl.parser_rx_sem, TOS_TIME_FOREVER) != K_ERR_NONE) {
        return -1;
    }
    
    /* 从chr fifo中取出数据 */
    err = tos_chr_fifo_pop(&pm2d5_parser_ctrl.parser_rx_fifo, &chr);

    return err == K_ERR_NONE ? chr : -1;
}

/**
 * @brief   PM2D5读取传感器原始数据并解析
 * @param   void
 * @retval  解析成功返回0，解析失败返回-1
*/
static int pm2d5_parser_read_raw_data(pm2d5_raw_data_u *pm2d5_raw_data, pm2d5_data_u *pm2d5_data)
{
    int i;
    uint8_t  len_h,len_l;
    uint16_t len;
    uint16_t check_sum;
    uint16_t check_sum_cal = 0x42 + 0x4d;
   
    /* 读取并计算帧长度 */
    len_h = pm2d5_parser_getchar();
    len_l = pm2d5_parser_getchar();
    len = (len_h << 8) | len_l;
    
    if ( len != 0x001C) {
        //非传感器值数据，清空缓存
        for (i = 0; i < len; i++) {
            pm2d5_parser_getchar();
        }
        return -1;
    }
    
    /* 读取传感器原始数据 */
    for (i = 0; i < len; i++) {
        pm2d5_raw_data->data[i] = pm2d5_parser_getchar();
    }
    
    /* 和校验 */
    //通过数据计算和校验
    check_sum_cal = check_sum_cal + len_h + len_l;
    for (i = 0; i < len -2; i++) {
        check_sum_cal += pm2d5_raw_data->data[i];
    }
    //协议中给出的和校验值
    check_sum = (pm2d5_raw_data->pm2d5_raw_data.chk_sum_h << 8) + pm2d5_raw_data->pm2d5_raw_data.chk_sum_l;
    if (check_sum_cal != check_sum) {
        return -1;
    }
    
    /* 存储传感器值 */
    for (i = 0; i < sizeof(pm2d5_data_t); i++) {
        pm2d5_data->data[i] = pm2d5_raw_data->data[i];
    }
    
    return 0;
}

extern k_mail_q_t mail_q;
pm2d5_raw_data_u pm2d5_raw_data;
pm2d5_data_u     pm2d5_data;

/**
 * @brief   PM2D5解析器任务
*/
static void pm2d5_parser_task_entry(void *arg)
{
    int chr, last_chr = 0;
    
    while (1) {
       
        chr = pm2d5_parser_getchar();
        if (chr < 0) {
            printf("parser task get char fail!\r\n");
            continue;
        }
        
        if (chr == 0x4d && last_chr == 0x42) {
            /* 解析到包头 */
            if (0 ==  pm2d5_parser_read_raw_data(&pm2d5_raw_data, &pm2d5_data)) {
                /* 正常解析之后通过邮箱发送 */
                tos_mail_q_post(&mail_q, &pm2d5_data, sizeof(pm2d5_data_t));
            }
        }
        
        last_chr = chr;
    }
}

/**
 * @brief   初始化PM2D5解析器
 * @param   none
 * @retval  全部创建成功返回0，任何一个创建失败则返回-1
*/
int pm2d5_parser_init(void)
{
    k_err_t ret;
    
    memset((pm2d5_parser_ctrl_t*)&pm2d5_parser_ctrl, 0, sizeof(pm2d5_parser_ctrl));
    
    /* 创建 chr fifo */
    ret = tos_chr_fifo_create(&pm2d5_parser_ctrl.parser_rx_fifo, pm2d5_parser_buffer, sizeof(pm2d5_parser_buffer));
    if (ret != K_ERR_NONE) {
        printf("pm2d5 parser chr fifo create fail, ret = %d\r\n", ret);
        return -1;
    }
    
    /* 创建信号量 */
    ret = tos_sem_create(&pm2d5_parser_ctrl.parser_rx_sem, 0);
    if (ret != K_ERR_NONE) {
        printf("pm2d5 parser_rx_sem create fail, ret = %d\r\n", ret);
        return -1;
    }
    
    /* 创建线程 */
    ret = tos_task_create(&pm2d5_parser_ctrl.parser_task, "pm2d5_parser_task", 
                          pm2d5_parser_task_entry, NULL, PM2D5_PARSER_TASK_PRIO,
                          pm2d5_parser_task_stack,PM2D5_PARSER_TASK_STACK_SIZE,0);
    if (ret != K_ERR_NONE) {
        printf("pm2d5 parser task create fail, ret = %d\r\n", ret);
        return -1;
    }

    return 0;
}
