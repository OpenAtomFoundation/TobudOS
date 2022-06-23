#include "stdio.h"
#include "string.h"

#include "tos_hal.h"
#include "esp8266_tencent_firmware.h"

at_agent_t esp8266_tf_agent;
static k_stack_t esp8266_tf_at_parse_task_stk[AT_PARSER_TASK_STACK_SIZE];

#define AT_AGENT    ((at_agent_t *)&esp8266_tf_agent)

// ----------------------------------------------------------------------------
// tencent firmware init
// ----------------------------------------------------------------------------

static int esp8266_tencent_firmware_restore(void)
{   
    int try = 0;
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, "ready");
    do {
        tos_at_cmd_exec_until(AT_AGENT, &echo, 5000, "AT+RESTORE\r\n");
    } while(echo.status != AT_ECHO_STATUS_EXPECT && try++ < 3);
    return !(echo.status == AT_ECHO_STATUS_EXPECT);
}

static int esp8266_tencent_firmware_echo_close(void)
{
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(AT_AGENT, &echo, 1000, "ATE0\r\n");
    return !(echo.status == AT_ECHO_STATUS_OK);
}

static int esp8266_tencent_firmware_net_mode_set(sal_net_mode_t mode)
{
    char *cmd = NULL;
    at_echo_t echo;

    switch (mode)
    {
    case SAL_NET_MODE_STA:
        cmd = "AT+CWMODE=1\r\n";
        break;
    case SAL_NET_MODE_AP:
        cmd = "AT+CWMODE=2\r\n";
        break;
    case SAL_NET_MODE_STA_AP:
        cmd = "AT+CWMODE=3\r\n";
        break;
    default:
        return -1;
    }
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(AT_AGENT, &echo, 1000, cmd);
    return !(echo.status == AT_ECHO_STATUS_OK);
}

static int esp8266_tencent_firmware_send_mode_set(sal_send_mode_t mode)
{
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(AT_AGENT, &echo, 1000, "AT+CIPMODE=%d\r\n", mode == SAL_SEND_MODE_NORMAL ? 0 : 1);
    return !(echo.status == AT_ECHO_STATUS_OK);
}

static int esp8266_tencent_firmware_init(void)
{
    printf("Init ESP8266 with tencent firmware ...\n");

    if (esp8266_tencent_firmware_restore())
    {
        printf("esp8266 restore FAILED\n");
        return -1;
    }

    if (esp8266_tencent_firmware_echo_close())
    {
        printf("esp8266 echo close FAILED\n");
        return -1;
    }

    if (esp8266_tencent_firmware_net_mode_set(SAL_NET_MODE_STA))
    {
        printf("esp8266 net mode set FAILED\n");
        return -1;
    }

    if (esp8266_tencent_firmware_send_mode_set(SAL_SEND_MODE_NORMAL))
    {
        printf("esp8266 send mode set FAILED\n");
        return -1;
    }

    printf("Init ESP8266 with tencent firmware Done\n");

    return 0;
}


static k_chr_fifo_t ota_fw_data_chr_fifo;
static k_sem_t ota_fw_data_sem;

static void esp8266_tencent_firmware_recvfwdata(void)
{
    /*
    +TCREADFWDATA:256,<HEX Data>
    */
    uint8_t data;
    uint16_t data_len = 0, read_len = 0;
    static uint8_t buffer[128];
    
    while (1) {
        if (tos_at_uart_read(AT_AGENT, &data, 1) != 1) {
            return;
        }
        if (data == ',') {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }

    do {
#define MIN(a, b)   ((a) < (b) ? (a) : (b))
        read_len = MIN(data_len, sizeof(buffer));
        if (tos_at_uart_read(AT_AGENT, buffer, read_len) != read_len) {
            return;
        }

        if (tos_chr_fifo_push_stream(&ota_fw_data_chr_fifo, buffer, read_len) <= 0) {
            return;
        }

        data_len -= read_len;
    } while (data_len > 0);
    
    tos_sem_post(&ota_fw_data_sem);
    
    return;
}

static at_event_t esp8266_tencent_firmware_at_event[] = {
    { "+TCREADFWDATA:", esp8266_tencent_firmware_recvfwdata },
};


// ----------------------------------------------------------------------------
// api
// ----------------------------------------------------------------------------

static at_urc_t at_urc_table[10];
static size_t at_urc_table_count;

int esp8266_tencent_firmware_sal_init(hal_uart_port_t uart_port)
{
        if (tos_at_init(AT_AGENT, "esp8266_tf_at", esp8266_tf_at_parse_task_stk,
                    uart_port, esp8266_tencent_firmware_at_event, sizeof(esp8266_tencent_firmware_at_event)/sizeof(at_event_t)) != 0) {
        return -1;
    }

    return esp8266_tencent_firmware_init();
}

void esp8266_tencent_firmware_sal_deinit(void)
{
    tos_at_deinit(AT_AGENT);
}

int esp8266_tencent_firmware_join_ap(const char *ssid, const char *pwd)
{
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, "WIFI GOT IP");
    tos_at_cmd_exec_until(AT_AGENT, &echo, 10000, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    return !(echo.status == AT_ECHO_STATUS_OK);
}

int esp8266_tencent_firmware_set_urc(const char *urc, at_urc_callback_t urc_handle)
{
    if (at_urc_table_count > sizeof(at_urc_table) / sizeof(at_urc_t))
    {
        return -1;
    }

    at_urc_table[at_urc_table_count].urc = urc;
    at_urc_table[at_urc_table_count].urc_callback = urc_handle;
    at_urc_table_count++;
    at_urc_table_set(AT_AGENT, at_urc_table, at_urc_table_count);
    return 0;
}


int esp8266_tencent_firmware_raw_data_send(const void *data, int data_len)
{
    return tos_at_raw_data_send(AT_AGENT, NULL, 0, data, data_len);
}

int esp8266_tencent_firmware_at_cmd_exec_until(const char *at_cmd, const char *at_expect, uint32_t timeout_ms)
{
    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, (char*)at_expect);
    at_expect ? tos_at_cmd_exec_until(AT_AGENT, &echo, timeout_ms, at_cmd) : tos_at_cmd_exec(AT_AGENT, &echo, timeout_ms, at_cmd);
    return at_expect ? !(echo.status == AT_ECHO_STATUS_EXPECT) : !(echo.status == AT_ECHO_STATUS_OK);
}

int esp8266_tencent_firmware_at_cmd_exec_recv(const char *at_cmd, const char *at_expect, void *recv_buffer, uint32_t *read_len, uint32_t timeout_ms)
{
    at_echo_t echo;

    if(K_ERR_NONE != tos_chr_fifo_create(&ota_fw_data_chr_fifo, recv_buffer, *read_len)) {
        return -1;
    }
    
    if(K_ERR_NONE != tos_sem_create_max(&ota_fw_data_sem, 0, 1)) {
        return -1;
    }
    
    tos_at_echo_create(&echo, NULL, 0, (char*)at_expect);

    tos_at_cmd_exec(AT_AGENT, &echo, timeout_ms, at_cmd);
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    tos_sem_pend(&ota_fw_data_sem, TOS_TIME_FOREVER);
    int len = tos_chr_fifo_pop_stream(&ota_fw_data_chr_fifo, recv_buffer, *read_len);
    *read_len = len > 0 ? len : 0;
    return !(*read_len > 0);
}
