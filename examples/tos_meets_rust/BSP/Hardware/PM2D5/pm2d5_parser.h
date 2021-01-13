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
#ifndef _PM2D5_PARSER_H_
#define _PM2D5_PARSER_H_
 
#include "tos_k.h"

/* pm2d5 parser config */
#define PM2D5_PARSER_TASK_STACK_SIZE    512
#define PM2D5_PARSER_TASK_PRIO          5
#define PM2D5_PARSER_BUFFER_SIZE        64

/* PM2.5 数据解析器控制块 */
typedef struct pm2d5_parser_control_st {
    k_task_t     parser_task;       //解析器任务控制块
    
    k_sem_t      parser_rx_sem;     //表示解析器从串口接收到数据
    k_chr_fifo_t parser_rx_fifo;    //存放解析器接收到的数
} pm2d5_parser_ctrl_t;

/**
 * @brief   解析出的PM2D5数据值
 * @note    可以作为邮件发送给其他任务进行进一步处理
 */
typedef struct pm2d5_data_st {
    uint16_t    data1;
    uint16_t    data2;
    uint16_t    data3;
    uint16_t    data4;
    uint16_t    data5;
    uint16_t    data6;
    uint16_t    data7;
    uint16_t    data8;
    uint16_t    data9;
    uint16_t    data10;
    uint16_t    data11;
    uint16_t    data12;
    uint16_t    data13;
}pm2d5_data_t;

typedef union pm2d5_data_un {
    uint16_t data[13];
    pm2d5_data_t pm2d5_data;
} pm2d5_data_u;

void pm2d5_parser_input_byte(uint8_t byte);

int pm2d5_send_read_cmd(void);

int pm2d5_parser_init(void);

#endif /* _PM2D5_PARSER_H_ */
