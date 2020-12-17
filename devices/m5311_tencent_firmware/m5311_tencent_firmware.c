#include "stdio.h"
#include "string.h"

#include "tos_at.h"
#include "tos_hal.h"
#include "tencent_firmware_module_wrapper.h"
#include "m5311_tencent_firmware.h"

/*
 * config dev info to module, do this operate only once in factroy is suggested
 */
int m5311_tencent_firmware_module_info_set(device_info_t *info, tls_mode_t mode)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+TCDEVINFOSET:OK");

    tos_at_cmd_exec(&echo, 2000, "AT+TCDEVINFOSET=%d,\"%s\",\"%s\",\"%s\"\r\n",
                        mode, info->product_id, info->device_name, info->device_serc);
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

/* mqtt setup connect */
int m5311_tencent_firmware_module_mqtt_conn(mqtt_param_t init_params)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTCONN:OK");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 2000, "AT+TCMQTTCONN=%d,%d,%d,%d,%d\r\n", init_params.tls_mode,
            init_params.command_timeout, init_params.keep_alive_interval_ms,
            init_params.clean_session, init_params.auto_connect_enable);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

/* mqtt disconn */
int m5311_tencent_firmware_module_mqtt_discon(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);

    tos_at_cmd_exec(&echo, 1000, "AT+TCMQTTDISCONN\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

int m5311_tencent_firmware_module_mqtt_publ(const char *topic, qos_t qos, char *payload)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, ">");

    tos_at_cmd_exec(&echo, 1000, "AT+TCMQTTPUBL=\"%s\",%d,%d\r\n", topic, qos, strlen(payload)-2);
    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTPUB:OK");
    tos_at_raw_data_send(&echo, 1000, (uint8_t *)payload, strlen(payload));
    if (echo.status != AT_ECHO_STATUS_OK  && echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }

    return 0;
}

/* mqtt pub msg */
int m5311_tencent_firmware_module_mqtt_pub(const char *topic, qos_t qos, char *payload)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTPUB:OK");

    tos_at_cmd_exec(&echo, 1000, "AT+TCMQTTPUB=\"%s\",%d,\"%s\"\r\n", topic, qos, payload);
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

int m5311_tencent_firmware_module_mqtt_sub(char *topic, qos_t qos)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTSUB:OK");

    tos_at_cmd_exec(&echo, 2000, "AT+TCMQTTSUB=\"%s\",%d\r\n", topic, qos);
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

int m5311_tencent_firmware_module_mqtt_unsub(const char *topic)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTUNSUB:OK");

    tos_at_cmd_exec(&echo, 2000, "AT+TCMQTTUNSUB=\"%s\"\r\n", topic);
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

int m5311_tencent_firmware_module_mqtt_state_get(mqtt_state_t *state)
{
    char *str;
    int ret_state;
    at_echo_t echo;
    char echo_buffer[64];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);

    tos_at_cmd_exec(&echo, 1000, "AT+TCMQTTSTATE?\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    str = strstr(echo.buffer, "+TCMQTTSTATE:");
    if (!str) 
    {
        return -1;
    }
    sscanf(str, "+TCMQTTSTATE:%d", &ret_state);
    if (ret_state == 0) {
        *state = MQTT_STATE_DISCONNECTED;
        return 0;
    } else if (ret_state == 1) {
        *state = MQTT_STATE_CONNECTED;
        return 0;
    }

    return -1;
}

int m5311_tencent_firmware_module_debug_level_set(int log_level)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);

    tos_at_cmd_exec(&echo, 1000, "AT+TCTEMLOG=%d\r\n", log_level);
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}


static int m5311_tencent_firmware_join_net(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "OK");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 5000, "AT+TCREGNET?\r\n");
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int m5311_tencent_firmware_init(void)
{
    printf("Init m5311 with tencent firmware ...\n");

    if (m5311_tencent_firmware_join_net() != 0) {
        printf("m5311 restore FAILED\n");
        return -1;
    }

    printf("Init m5311 with tencent firmware Done\n" );

    return 0;
}

int m5311_tencent_firmware_ota_set(ota_mode_t mode, char *version)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+TCOTASET:OK");

    tos_at_cmd_exec(&echo, 2000, "AT+TCOTASET=%d,\"%s\"\r\n", mode, version);
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}
k_sem_t ota_fw_info_sem;

int m5311_tencent_firmware_ota_read_fwinfo(ota_fw_info_t *ota_fw_info)
{
    at_echo_t echo;
    char echo_buffer[64];
    uint8_t FileSize[10] = {0};
    uint32_t updateFileSize = 0;
    
    /* wait update command frome cloud forever */
    if(K_ERR_NONE != tos_sem_create_max(&ota_fw_info_sem, 0, 1)) {
        return -1;
    }
    tos_sem_pend(&ota_fw_info_sem, TOS_TIME_FOREVER);

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "+TCFWINFO:");

    tos_at_cmd_exec(&echo, 2000, "AT+TCFWINFO?\r\n");
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        
        sscanf(echo_buffer, "%*[^\"]%*c%[^\"]%*[^,]%*c%[^,]%*[^\"]%*c%[^\"]", ota_fw_info->fw_version, FileSize, ota_fw_info->fw_md5);
        for(int i = 0; i<10; i++)
        {
            if(FileSize[i] == 0)
            {
                break;
            }
            updateFileSize = updateFileSize*10 + (FileSize[i] - 0x30);
        }
        ota_fw_info->fw_size = updateFileSize;
    }

    return 0;
}

k_chr_fifo_t ota_fw_data_chr_fifo;
k_sem_t ota_fw_data_sem;

static int m5311_tencent_firmware_ota_read_fwdata(uint8_t *ota_fw_data_buffer,uint16_t read_len)
{
    at_echo_t echo;
    
    if(K_ERR_NONE != tos_chr_fifo_create(&ota_fw_data_chr_fifo, ota_fw_data_buffer, read_len)) {
        return -1;
    }
    
    if(K_ERR_NONE != tos_sem_create_max(&ota_fw_data_sem, 0, 1)) {
        return -1;
    }
    
    tos_at_echo_create(&echo, NULL, 0, NULL);

    tos_at_cmd_exec(&echo, 300, "AT+TCREADFWDATA=%d\r\n", read_len);
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    
    tos_sem_pend(&ota_fw_data_sem, TOS_TIME_FOREVER);
    
    return tos_chr_fifo_pop_stream(&ota_fw_data_chr_fifo, ota_fw_data_buffer, read_len);
}

static mqtt_message_t mqtt_message;

void m5311_tencent_firmware_recvpub(void)
{
    /*
     +TCMQTTRCVPUB:"xxx/yyy/zzz",44,"{"type":"get", "clientToken":"123456781234"}"
     */

    uint8_t data;
    int read_len = 0, payload_len = 0;

    memset(&mqtt_message, 0, sizeof(mqtt_message));

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }
        if (data == '"') {
            continue;
        } else if (data == ',') {
            break;
        }
        if (read_len < sizeof(mqtt_message.topic)) {
            mqtt_message.topic[read_len++] = data;
        }
    }
    if (read_len == sizeof(mqtt_message.topic)) {
        mqtt_message.topic[read_len - 1] = '\0';
    } else {
        mqtt_message.topic[read_len] = '\0';
    }

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }
        if (data == ',') {
            break;
        }
        payload_len = payload_len * 10 + (data - '0');
    }

    if (payload_len > sizeof(mqtt_message.payload)) {
        payload_len = sizeof(mqtt_message.payload);
    }

    read_len = tos_at_uart_read((uint8_t*)mqtt_message.payload, payload_len + 2);
    if (read_len != payload_len + 2) {
        return;
    }

    tos_mail_q_post(&mqtt_message_mail, &mqtt_message, sizeof(mqtt_message_t));
}

void m5311_tencent_firmware_recvcmd(void)
{
    /*
    +TCOTASTATUS:UPDATEFAIL
    +TCOTASTATUS:UPDATESUCCESS
    */
    uint8_t buffer[20];
    int read_len = 13;

    if (tos_at_uart_read(buffer, read_len) != read_len) {
        return;
    }
    
    if(!strstr((char*)buffer, "UPDATESUCCESS")) {
        return;
    }
    
    tos_sem_post(&ota_fw_info_sem);
    
    return;
}

void m5311_tencent_firmware_recvfwdata(void)
{
    /*
    +TCREADFWDATA:256,<HEX Data>
    */
    uint8_t data;
    uint16_t data_len = 0, read_len = 0;
    static uint8_t buffer[128];
    
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
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
        if (tos_at_uart_read(buffer, read_len) != read_len) {
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

at_event_t m5311_tencent_firmware_at_event[] = {
    { "+TCMQTTRCVPUB:", m5311_tencent_firmware_recvpub },
    { "+TCREADFWDATA:", m5311_tencent_firmware_recvfwdata },
    { "+TCOTASTATUS:", m5311_tencent_firmware_recvcmd },
};

tencent_firmware_module_t tencent_firmware_module_m5311 = {
    .init               = m5311_tencent_firmware_init,
    .info_set           = m5311_tencent_firmware_module_info_set, 
    .mqtt_conn          = m5311_tencent_firmware_module_mqtt_conn,        
    .mqtt_discon        = m5311_tencent_firmware_module_mqtt_discon,
    .mqtt_pub           = m5311_tencent_firmware_module_mqtt_pub,
    .mqtt_publ          = m5311_tencent_firmware_module_mqtt_publ,
    .mqtt_sub           = m5311_tencent_firmware_module_mqtt_sub,
    .mqtt_unsub         = m5311_tencent_firmware_module_mqtt_unsub,
    .mqtt_state_get     = m5311_tencent_firmware_module_mqtt_state_get,
    .debug_level_set    = m5311_tencent_firmware_module_debug_level_set,
    .ota_set            = m5311_tencent_firmware_ota_set,
    .ota_read_fwinfo    = m5311_tencent_firmware_ota_read_fwinfo,
    .ota_read_fwdata    = m5311_tencent_firmware_ota_read_fwdata,
};

int m5311_tencent_firmware_sal_init(hal_uart_port_t uart_port)
{
    int ret = -1;
    
    if (tos_at_init(uart_port, m5311_tencent_firmware_at_event,
                        sizeof(m5311_tencent_firmware_at_event) /
                        sizeof(m5311_tencent_firmware_at_event[0])) != 0) {
        return -1;
    }

    if (tos_tf_module_register(&tencent_firmware_module_m5311) != 0) {
        return -1;
    }

    if ((ret = tos_tf_module_init()) != 0) {
        return ret;
    }

    return 0;
}

