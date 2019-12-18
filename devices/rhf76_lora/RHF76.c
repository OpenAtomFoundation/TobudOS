#include "RHF76.h"
#include "tos_hal.h"

static mcps_indication_t rhf76_mcps_indication;

static int rhf76_exit_low_power(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+LOWPOWER: AUTOOFF");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 3000, RHF76_LOWPOWER_SET);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int rhf76_reset(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+RESET: OK");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 3000, RHF76_ATCMD_RESET);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int rhf76_set_id(lora_id_type_t id_type, char *id)
{
    char cmd[64] = {0};
    int try = 0;
    at_echo_t echo;
    switch(id_type) {
        case LORA_ID_TYPE_DEVADDR:
            snprintf(cmd, sizeof(cmd), RHF76_ATCMD_FMT_SET_ID_DEVADDR, id);
            break;
        case LORA_ID_TYPE_DEVEUI:
            snprintf(cmd, sizeof(cmd), RHF76_ATCMD_FMT_SET_ID_DEVEUI, id);
            break;
        case LORA_ID_TYPE_APPEUI:
            snprintf(cmd, sizeof(cmd), RHF76_ATCMD_FMT_SET_ID_DEVAPPEUI, id);
            break;
        default:
            return -1;
    }
    tos_at_echo_create(&echo, NULL, 0, "+ID: ");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, cmd);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int rhf76_set_key(lora_key_type_t key_type, char *key)
{
    char cmd[64] = {0};
    int try = 0;
    at_echo_t echo;
    switch (key_type) {
        case LORA_KEY_TYPE_APPKEY:
            snprintf(cmd, sizeof(cmd), RHF76_ATCMD_FMT_SET_KEY_TYPE_APPKEY, key);
            break;
        case LORA_KEY_TYPE_APPSKEY:
            snprintf(cmd, sizeof(cmd), RHF76_ATCMD_FMT_SET_KEY_TYPE_APPSKEY, key);
            break;
        case LORA_KEY_TYPE_NWKSKEY:
            snprintf(cmd, sizeof(cmd), RHF76_ATCMD_FMT_SET_KEY_TYPE_NWKSKEY, key);
            break;
        default:
            return -1;
    }
    tos_at_echo_create(&echo, NULL, 0, "+KEY:");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, cmd);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int rhf76_set_class(lora_class_t lora_class)
{
    char *cmd = NULL, *reply = NULL;
    int try = 0;
    at_echo_t echo;
    switch (lora_class) {
        case LORA_CLASS_A:
            cmd     = RHF76_ATCMD_SET_CLASS_A;
            reply   = RHF76_ATCMD_REPLY_CLASS_A;
            break;
        case LORA_CLASS_B:
            cmd     = RHF76_ATCMD_SET_CLASS_B;
            reply   = RHF76_ATCMD_REPLY_CLASS_B;
            break;
        case LORA_CLASS_C:
            cmd     = RHF76_ATCMD_SET_CLASS_C;
            reply   = RHF76_ATCMD_REPLY_CLASS_C;
            break;
        default:
            return -1;
    }
    tos_at_echo_create(&echo, NULL, 0, reply);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, cmd);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int rhf76_set_chanel(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+CH: NUM");
   
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 3000, RHF76_ATCMD_SET_CHANNEL);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int rhf76_set_adr_off(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+ADR: OFF");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 3000, RHF76_ATCMD_SET_ADR_OFF);
        if (echo.status == AT_ECHO_STATUS_OK|| echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int rhf76_set_mode(lora_mode_t mode)
{
    char *cmd = NULL, *reply = NULL;
    int try = 0;
    at_echo_t echo;
    switch (mode) {
        case LORA_MODE_LWOTAA:
            cmd     = RHF76_ATCMD_SET_MODE_LWOTAA;
            reply   = RHF76_ATCMD_REPLY_MODE_LWOTAA;
            break;
        case LORA_MODE_LWABP:
            cmd     = RHF76_ATCMD_SET_MODE_LWABP;
            reply   = RHF76_ATCMD_REPLY_MODE_LWABP;
            break;
        default:
            return -1;
    }
    tos_at_echo_create(&echo, NULL, 0, reply);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 2500, cmd);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

int rhf76_join(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+JOIN: Network joined");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 14000, RHF76_ATCMD_JOIN);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int rhf76_init(void)
{
    char *key = "2B7E151628AED2A6ABF7158809CF4F3C";
    char *appeui = "70B3D57ED00E0017";

    printf("Init RHF76 LoRa ...\n" );

    at_delay_ms(1000);
    if (rhf76_exit_low_power() != 0) {
        printf("rhf76 reset FAILED\n");
        return -1;
    }

    if (rhf76_reset() != 0) {
        printf("rhf76 reset FAILED\n");
        return -1;
    }

    if (rhf76_set_class(LORA_CLASS_A) != 0) {
        printf("rhf76 set class FAILED\n");
        return -1;
    }

    if (rhf76_set_chanel() != 0) {
        printf("rhf76 set chanel FAILED\n");
        return -1;
    }

    if (rhf76_set_adr_off() != 0) {
        printf("rhf76 set adr FAILED\n");
        return -1;
    }

    if (rhf76_set_mode(LORA_MODE_LWOTAA) != 0) {
        printf("rhf76 set mode FAILED\n");
        return -1;
    }

    if (rhf76_set_id(LORA_ID_TYPE_APPEUI, appeui) != 0) {
        printf("rhf76 set appeui FAILED\n");
        return -1;
    }

    if (rhf76_set_key(LORA_KEY_TYPE_APPKEY, key) != 0) {
        printf("rhf76 set appkey FAILED\n");
        return -1;
    }

    at_delay_ms(3000);
    printf("Init RHF76 LoRa done\n");

    return 0;
}

__STATIC__ uint8_t __ascii2hex(char in) {
    if (('0' <= in) && (in <= '9')) {
        return in - '0';
    }
    if (('A' <= in) && (in <= 'F')) {
        return in - 'A' + 10;
    }
    if (('a' <= in) && (in <= 'f')) {
        return in - 'a' + 10;
    }
    return (uint8_t)-1;
}

__STATIC__ void __asciistr2hex(char *in, uint8_t *out, int len) {
    int i = 0;

    for (i = 0; i < len; i += 2) {
        out[i / 2] = (__ascii2hex(in[i]) << 4) + __ascii2hex(in[i + 1]);
    }
}

__STATIC__ char incoming_data_buffer[128]; 
__STATIC__ uint8_t hex_stream[128];

__STATIC__ void rhf76_incoming_data_process(void)
{
    int ret;
    uint8_t data;

    /*
    +CMSG: PORT: 8; RX: "12345678"
    RX:       prefix
    "12345678":    data content
    */

    while (1) {
        ret = tos_at_uart_read(&data, 1);
        if (data == '"') {
            break;
        }
    }
    
    ret = 0;
    memset(incoming_data_buffer, 0x00, 512);
    while (1) {
        tos_at_uart_read(&data, 1);
        if (data == '"') {
            break;
        }
        incoming_data_buffer[ret++] = data;
    }

    printf("rhf76_incoming_data_process %d: %s\n", ret, incoming_data_buffer);
    
    __asciistr2hex(incoming_data_buffer, hex_stream, strlen(incoming_data_buffer));
    rhf76_mcps_indication(hex_stream, strlen(incoming_data_buffer)/2);
}

at_event_t rhf76_at_event[] = {
    { "+CMSGHEX: PORT:", rhf76_incoming_data_process },
};



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

static void __hex2str(uint8_t *in, char *out, int len)
{
	int i = 0;

	for (i = 0; i < len; ++i) {
		out[i * 2] = __num2hex(in[i] >> 4);
		out[i * 2 + 1] = __num2hex(in[i] & 0x0F);
	}
	out[2 * len] = '\0';
}

static int rhf76_send(const void *buf, size_t len)
{

    char *str_buf = NULL;

    str_buf = tos_mmheap_calloc(2 * len + 1, sizeof(char));
    if (!str_buf) {
        return -1;
    }
    __hex2str((uint8_t *)buf, str_buf, len);
    
    char cmd[100] = {0};
    at_echo_t echo;
    snprintf(cmd, sizeof(cmd), RHF76_ATCMD_FMT_SEND_MSGHEX, str_buf);
    cmd[sizeof(cmd) - 1] = '\0';
    tos_mmheap_free(str_buf);
    tos_at_echo_create(&echo, NULL, 0, "+CMSG: ACK Received");    
    tos_at_cmd_exec(&echo, 6000, cmd);
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return -1;
    } 
    return len;
}

static int rhf76_recv_register(void* mcps_indication)
{
    rhf76_mcps_indication = (mcps_indication_t)mcps_indication;
    return 0;
}

static int rhf76_close(void)
{
    return 0;
}

lora_module_t lora_module_rhf76 = {
    .init           = rhf76_init,
    .join           = rhf76_join,
    .send           = rhf76_send,
    .recv_register  = rhf76_recv_register,
    .close          = rhf76_close
};

int rhf76_lora_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, rhf76_at_event,
                        sizeof(rhf76_at_event) / sizeof(rhf76_at_event[0])) != 0) {
        return -1;
    }

    at_delay_ms(1000);

    if (tos_lora_module_register(&lora_module_rhf76) != 0) {
        return -1;
    }

    if (tos_lora_module_init() != 0) {
        return -1;
    }

    return 0;
}


