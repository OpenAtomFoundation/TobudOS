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
#include "bc35_28_95_lwm2m.h"
#include "tos_k.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"

static char __num2hex(uint8_t num)
{
    if (num <= 0x9) {
        return num + '0';
    }

    if ((0xA <= num) && (num <= 0xF)) {
        return num - 0xA + 'A';
    }

    return (char)-1;
}

void __hex2str(uint8_t *in, char *out, int len)
{
	int i = 0;

	for (i = 0; i < len; ++i) {
		out[i * 2] = __num2hex(in[i] >> 4);
		out[i * 2 + 1] = __num2hex(in[i] & 0x0F);
	}
	out[2 * len] = '\0';
}

static int bc35_28_95_reset(void){
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "Neul");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 6000, "AT+NRB\r\n");
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            printf("wait for bcxx reboot!\n");
            return 0;
        }
    }
    return -1;
}

static int bc35_28_95_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

static int bc35_28_95_check_cfun(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+CFUN:1");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CFUN?\r\n");
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int bc35_28_95_cfun_set(char mode)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        if(mode){
            tos_at_cmd_exec(&echo, 1000, "AT+CFUN=1\r\n");
        }
        else{
            tos_at_cmd_exec(&echo, 1000, "AT+CFUN=0\r\n");
        }
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int bc35_28_95_net_set(char mode)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        if(mode){
            tos_at_cmd_exec(&echo, 1000, "AT+CGATT=1\r\n");
        }
        else{
            tos_at_cmd_exec(&echo, 1000, "AT+CGATT=0\r\n");
        }
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int bc35_28_95_check_net(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+CGATT:1");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CGATT?\r\n");
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int bc35_28_95_signal_quality_check(void)
{
    int rssi, ber;
    at_echo_t echo;
    char echo_buffer[32], *str;

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+CSQ\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    str = strstr(echo.buffer, "+CSQ:");
    sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
    if (rssi == 99) {
        return -1;
    }

    return 0;
}

static int bc35_28_95_nband_set(char band){
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+NBAND=%d\r\n",band);
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}
static int bc35_28_95_auto_report(char report){
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+NNMI=%d\r\n",report);
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}


static int bc35_28_95_autoconnect_config(char mode)
{
    int try = 0;
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        if(mode){
            tos_at_cmd_exec(&echo, 1000, "AT+NCONFIG=AUTOCONNECT,TRUE\r\n");
        }
        else{
            tos_at_cmd_exec(&echo, 1000, "AT+NCONFIG=AUTOCONNECT,FALSE\r\n");
        }
        if(echo.status == AT_ECHO_STATUS_OK){
            return 0;
        }
    }
    return -1;
}

static int bc35_28_95_print_IMEI(void) {
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+CGSN=1\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;

}
static int bc35_28_95_print_CIMI(void) {
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+CIMI\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;

}
static int bc35_28_95_print_NBAND(void) {
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+NBAND=?\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;

}

static int bc35_28_95_lwm2m_init(void)
{
    printf("Init BC95_28_95 ...wait reset!\n" );
    at_delay_ms(5000);
    if (bc35_28_95_echo_close() != 0) {
        printf("echo close FAILED\n");
        return -1;
    }
    if(bc35_28_95_autoconnect_config(0)!=0){
        printf("autoconnect false FAILED\n");
        return -1;
    };
    if (bc35_28_95_cfun_set(0) != 0) {
        printf("close_cfun FAILED\n");
        return -1;
    }
    if(bc35_28_95_nband_set(5)!=0){
        printf("nband_set FAILED\n");
        return -1;
    }

    printf("Init bc35_28_95 Done\n" );
    return 0;
}

static int bc35_28_95_lwm2m_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id, try = 0, is_connected = 0;
    at_echo_t echo;
    char echo_buffer[32];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+NCDP=%s,%s\r\n,",ip,port);
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    if(bc35_28_95_reset()!=0){
        printf("reset FAILED\n");
        return -1;
    }
    tos_task_delay(1000);
    bc35_28_95_check_cfun();
    bc35_28_95_cfun_set(1);
    bc35_28_95_net_set(1);
    bc35_28_95_autoconnect_config(1);
    bc35_28_95_auto_report(1);
    bc35_28_95_print_IMEI();
    bc35_28_95_print_CIMI();
    bc35_28_95_print_NBAND();
    if (bc35_28_95_signal_quality_check() != 0) {
        printf("check csq FAILED\n");
        return -1;
    }
    while (try++ < 10) {
        if (bc35_28_95_check_net()==0){
            is_connected = 1;
            break;
        }
        tos_task_delay(1000);
       continue;
    }
    if(is_connected){
        id=0;
        sscanf(echo.buffer, "%d", &id);
        id = tos_at_channel_alloc_id(id, ip, port);
        if (id == -1) {
            return -1;
        }
    }
    else{
        return -1;
    }
    return id;
}

static int bc35_28_95_lwm2m_send(int id, const void *buf, size_t len)
{
    char *str_buf = NULL;
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }
    str_buf = tos_mmheap_calloc(2 * len + 1, sizeof(char));
    if (!str_buf) {
        tos_at_global_lock_post();
        return -1;
    }
    __hex2str((uint8_t *)buf, str_buf, len);
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000,
                            "AT+NMGS=%d,%s\r\n",
                            len, str_buf);
    tos_mmheap_free(str_buf);
    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_global_lock_post();
        return -1;
    }
    tos_at_global_lock_post();
    return len;
}

__STATIC__ uint8_t incoming_data_buffer[1024];

__STATIC__ void bc35_28_95_lwm2m_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0;

    /*
    +NNMI:2,aabb\r\n
    2: Length
    aabb: data
    */

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }


    if (data_len > sizeof(incoming_data_buffer)/2) {
        data_len = sizeof(incoming_data_buffer)/2;
    }

    if (tos_at_uart_read(incoming_data_buffer, data_len*2) != data_len*2) {
        return;
    }

    tos_at_channel_write(channel_id, incoming_data_buffer, data_len*2);
}

at_event_t bc35_28_95_at_event[] = {
    { "+NNMI:", bc35_28_95_lwm2m_incoming_data_process },
};

static int bc35_28_95_lwm2m_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
     return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int bc35_28_95_lwm2m_recv(int id, void *buf, size_t len)
{
    return bc35_28_95_lwm2m_recv_timeout(id, buf, len, (uint32_t)4000);
}

static int bc35_28_95_lwm2m_close(int id)
{
    tos_at_channel_free(id);
    return 0;
}

sal_module_t sal_module_bc35_28_95_lwm2m = {
    .init           = bc35_28_95_lwm2m_init,
    .connect        = bc35_28_95_lwm2m_connect,
    .send           = bc35_28_95_lwm2m_send,
    .recv_timeout   = bc35_28_95_lwm2m_recv_timeout,
    .recv           = bc35_28_95_lwm2m_recv,
    .close          = bc35_28_95_lwm2m_close,
    .parse_domain   = NULL,
};

int bc35_28_95_sal_lwm2m_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, bc35_28_95_at_event,
                        sizeof(bc35_28_95_at_event) / sizeof(bc35_28_95_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_bc35_28_95_lwm2m) != 0) {
        return -1;
    }

    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}
