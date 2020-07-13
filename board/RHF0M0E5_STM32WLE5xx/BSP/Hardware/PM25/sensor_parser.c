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

#include "sensor_parser.h"

__STATIC__ shell_ctl_t shell_ctl;

__STATIC__ k_stack_t shell_parser_task_stack[SHELL_PARSER_TASK_STACK_SIZE];

__STATIC__ int shell_getchar(void)
{
    uint8_t chr;
    k_err_t err;

    if (tos_sem_pend(&SHELL_CTL->shell_rx_sem, TOS_TIME_FOREVER) != K_ERR_NONE) {
        return -1;
    }

    err = tos_chr_fifo_pop(&SHELL_CTL->shell_rx_fifo, &chr);

    return err == K_ERR_NONE ? chr : -1;
}

typedef struct sensor_frame_st {
    uint16_t    len;
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
    uint16_t    chk_sum;

    uint8_t     len_h;
    uint8_t     len_l;

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

    uint8_t     data13_h;
    uint8_t     data13_l;

    uint8_t     chk_sum_h;
    uint8_t     chk_sum_l;
} __PACKED__ sensor_frame_t;

extern k_mail_q_t mail_q;

k_tick_t last_report = 0;

int read_frame(sensor_frame_t *frame)
{
    frame->len_h = shell_getchar();
    frame->len_l = shell_getchar();

    frame->data1_h = shell_getchar();
    frame->data1_l = shell_getchar();

    frame->data2_h = shell_getchar();
    frame->data2_l = shell_getchar();

    frame->data3_h = shell_getchar();
    frame->data3_l = shell_getchar();

    frame->data4_h = shell_getchar();
    frame->data4_l = shell_getchar();

    frame->data5_h = shell_getchar();
    frame->data5_l = shell_getchar();

    frame->data6_h = shell_getchar();
    frame->data6_l = shell_getchar();

    frame->data7_h = shell_getchar();
    frame->data7_l = shell_getchar();

    frame->data8_h = shell_getchar();
    frame->data8_l = shell_getchar();

    frame->data9_h = shell_getchar();
    frame->data9_l = shell_getchar();

    frame->data10_h = shell_getchar();
    frame->data10_l = shell_getchar();

    frame->data11_h = shell_getchar();
    frame->data11_l = shell_getchar();

    frame->data12_h = shell_getchar();
    frame->data12_l = shell_getchar();

    frame->data13_h = shell_getchar();
    frame->data13_l = shell_getchar();

    frame->chk_sum_h = shell_getchar();
    frame->chk_sum_l = shell_getchar();


    frame->len = (frame->len_h << 8) | frame->len_l;
    frame->data1 = (frame->data1_h << 8) | frame->data1_l;
    frame->data2 = (frame->data2_h << 8) | frame->data2_l;
    frame->data3 = (frame->data3_h << 8) | frame->data3_l;
    frame->data4 = (frame->data4_h << 8) | frame->data4_l;
    frame->data5 = (frame->data5_h << 8) | frame->data5_l;
    frame->data6 = (frame->data6_h << 8) | frame->data6_l;
    frame->data7 = (frame->data7_h << 8) | frame->data7_l;
    frame->data8 = (frame->data8_h << 8) | frame->data8_l;
    frame->data9 = (frame->data9_h << 8) | frame->data9_l;
    frame->data10 = (frame->data10_h << 8) | frame->data10_l;
    frame->data11 = (frame->data11_h << 8) | frame->data11_l;
    frame->data12 = (frame->data12_h << 8) | frame->data12_l;
    frame->data13 = (frame->data13_h << 8) | frame->data13_l;
    frame->chk_sum = (frame->chk_sum_h << 8) | frame->chk_sum_l;


    uint16_t chsum;
    chsum =    frame->len_h +
    frame->len_l +
    frame->data1_h +
    frame->data1_l +
    frame->data2_h +
    frame->data2_l +
    frame->data3_h +
    frame->data3_l +
    frame->data4_h +
    frame->data4_l +
    frame->data5_h +
    frame->data5_l +
    frame->data6_h +
    frame->data6_l +
    frame->data7_h +
    frame->data7_l +
    frame->data8_h +
    frame->data8_l +
    frame->data9_h +
    frame->data9_l +
    frame->data10_h +
    frame->data10_l +
    frame->data11_h +
    frame->data11_l +
    frame->data12_h +
    frame->data12_l +
    frame->data13_h +
    frame->data13_l;

    if (chsum + 0x42 + 0x4d != frame->chk_sum) {
        return -1;
    }

    k_tick_t now = tos_systick_get();
    if (last_report == 0 || now - last_report >= 15000) {
        last_report = now;

        printf("data1: %d\n", frame->data1);
        printf("data2: %d\n", frame->data2);
        printf("data3: %d\n", frame->data3);
        printf("data4: %d\n", frame->data4);
        printf("data5: %d\n", frame->data5);
        printf("data6: %d\n", frame->data6);
        printf("data7: %d\n", frame->data7);
        printf("data8: %d\n", frame->data8);
        printf("data9: %d\n", frame->data9);
        printf("data10: %d\n", frame->data10);
        printf("data11: %d\n", frame->data11);
        printf("data12: %d\n", frame->data12);
        printf("data13: %d\n", frame->data13);
        printf("\n\n");

        return 0;
    }

    return -1;
}

sensor_frame_t frame;

__STATIC__ int shell_readline(void)
{
    int chr, last_chr = 0;
    char *buf = SHELL_CTL->cmd_buffer;

    while (K_TRUE) {
        if (buf - SHELL_CTL->cmd_buffer >= (SHELL_CTL->cmd_buffer_size - 1)) {
            return -1;
        }

        chr = shell_getchar();
        if (chr < 0) {
            return -1;
        }

        if (chr == 0x4d && last_chr == 0x42) {
            if (read_frame(&frame) == 0) {
                tos_mail_q_post(&mail_q, &frame.data1_h, 13 * 2);
            }
        }

        *buf++      = chr;
        last_chr    = chr;
    }
}


__STATIC__ void shell_parser(void *arg)
{
    while (K_TRUE) {
        shell_readline();
    }
}

__API__ int tos_shell_init(char *cmd_buf, int cmd_buf_size, shell_output_t output)
{
    void *buffer = K_NULL;

    TOS_PTR_SANITY_CHECK_RC(cmd_buf, -1);
    TOS_PTR_SANITY_CHECK_RC(output, -1);

    memset(SHELL_CTL, 0, sizeof(shell_ctl_t));

    SHELL_CTL->cmd_buffer       = cmd_buf;
    SHELL_CTL->cmd_buffer_size  = cmd_buf_size;
    SHELL_CTL->output           = output;

    buffer = tos_mmheap_alloc(cmd_buf_size * 2);
    if (!buffer) {
        return -1;
    }

    SHELL_CTL->shell_rx_fifo_buffer = (uint8_t *)buffer;
    tos_chr_fifo_create(&SHELL_CTL->shell_rx_fifo, buffer, cmd_buf_size * 2);

    if (tos_sem_create(&SHELL_CTL->shell_rx_sem, (k_sem_cnt_t)0u) != K_ERR_NONE) {
        goto errout0;
    }

    if (tos_task_create(&SHELL_CTL->parser, "shell_parser", shell_parser,
                        K_NULL, SHELL_PARSER_TASK_PRIO, shell_parser_task_stack,
                        SHELL_PARSER_TASK_STACK_SIZE, 0) != K_ERR_NONE) {
        goto errout1;
    }

    return 0;

errout1:
    tos_sem_destroy(&SHELL_CTL->shell_rx_sem);

errout0:
    tos_mmheap_free(SHELL_CTL->shell_rx_fifo_buffer);
    SHELL_CTL->shell_rx_fifo_buffer = K_NULL;
    tos_chr_fifo_destroy(&SHELL_CTL->shell_rx_fifo);

    return -1;
}

__API__ void tos_shell_deinit(void)
{
    tos_task_destroy(&SHELL_CTL->parser);
    tos_sem_destroy(&SHELL_CTL->shell_rx_sem);

    tos_mmheap_free(SHELL_CTL->shell_rx_fifo_buffer);
    SHELL_CTL->shell_rx_fifo_buffer = K_NULL;
    tos_chr_fifo_destroy(&SHELL_CTL->shell_rx_fifo);
}

__API__ void tos_shell_printf(const char *format, ...)
{
    va_list args;
    static char buffer[SHELL_OUTPUT_MAX];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    (SHELL_CTL->output)(buffer);
}

__API__ void tos_shell_input_byte(uint8_t data)
{
    if (tos_chr_fifo_push(&SHELL_CTL->shell_rx_fifo, data) == K_ERR_NONE) {
        tos_sem_post(&SHELL_CTL->shell_rx_sem);
    }
}

