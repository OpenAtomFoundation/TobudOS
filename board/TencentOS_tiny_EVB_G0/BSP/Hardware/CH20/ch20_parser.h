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
#ifndef _CH20_PARSER_H_
#define _CH20_PARSER_H_
 
#include "tos_k.h"

/* 使能是否开启调试日志 */
#define CH20_DEBUG_LOG_EN              0

/* CH20 parser config */
#define CH20_PARSER_TASK_STACK_SIZE    512
#define CH20_PARSER_TASK_PRIO          5
#define CH20_PARSER_BUFFER_SIZE        32

#if CH20_DEBUG_LOG_EN
#include <stdio.h>
#define CH20_DEBUG_LOG   printf
#else
#define CH20_DEBUG_LOG(fmt,...)
#endif


/* PM2.5 数据解析器控制块 */
typedef struct CH20_parser_control_st {
    k_task_t     parser_task;       //解析器任务控制块
    
    k_sem_t      parser_rx_sem;     //表示解析器从串口接收到数据
    k_chr_fifo_t parser_rx_fifo;    //存放解析器接收到的数据
} ch20_parser_ctrl_t;

/**
 * @brief   解析出的CH20数据值
 * @note    可以作为邮件发送给其他任务进行进一步处理
 * @param   
 */
typedef struct ch20_data_st {
    uint16_t    data;
} ch20_data_t;

void ch20_parser_input_byte(uint8_t byte);

int ch20_parser_init(void);

#endif /* _CH20_PARSER_H_ */
