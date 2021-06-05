#include "stdio.h"
#include "string.h"

#include "tos_at.h"
#include "tos_hal.h"
#include "tencent_firmware_module_wrapper.h"
#include "l610_tencent_firmware.h"

static int l610_check_ready(void)
{
    at_echo_t echo;
    int try = 0;
    
    while (++try) {
        tos_sleep_ms(2000);
        
        tos_at_echo_create(&echo, NULL, 0, NULL);
        
        tos_at_cmd_exec(&echo, 1000, "AT\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }

    return -1;
}

static int l610_echo_close(void)
{
    at_echo_t echo;
    int try = 0;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
        tos_sleep_ms(1000);
    }

    return -1;
}
static int l610_sim_card_check(void)
{
    at_echo_t echo;
    int try = 0;
    char echo_buffer[32];
	
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CPIN?\r\n");
        if (strstr(echo_buffer, "READY")) {
            return 0;
        }
        tos_sleep_ms(2000);
    }
    
    return -1;
}

static int l610_signal_quality_check(void)
{
    int rssi, ber;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;
	
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CSQ\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
            return -1;
        }

        str = strstr(echo.buffer, "+CSQ:");
        if (!str) {
            return -1;
        }
        sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
        if (rssi != 99) {
            return 0;
        }
        tos_sleep_ms(2000);
    }

    return -1;
}
static int l610_gsm_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;
	
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CREG?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
            return -1;
        }

        str = strstr(echo.buffer, "+CREG:");
        if (!str) {
            return -1;
        }
        sscanf(str, "+CREG:%d,%d", &n, &stat);
        if (stat == 1) {
            return 0;
        }
        tos_sleep_ms(2000);
    }
    return -1;	
}

static int l610_gprs_network_check(void)
{
    int n, stat;
    at_echo_t echo;
    char echo_buffer[32], *str;
    int try = 0;

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    while (try++ < 10) {
       
        tos_at_cmd_exec(&echo, 1000, "AT+CGREG?\r\n");
        if (echo.status != AT_ECHO_STATUS_OK) {
            return -1;
        }

        str = strstr(echo.buffer, "+CGREG:");
        if (!str) {
            return -1;
        }
        sscanf(str, "+CGREG:%d,%d", &n, &stat);
        if (stat == 1) {
            return 0;
        }
        tos_sleep_ms(2000);
    }
		
    return -1;
}

static int l610_set_data_format(void)
{
    at_echo_t echo;
    
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+GTSET=\"IPRFMT\",0\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    
    return 0;
}

static int l610_check_apn(void)
{
    at_echo_t echo;
    char buffer[64];
    char *str = NULL;
    int apn_state;
    
    tos_at_echo_create(&echo, buffer, 64, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+MIPCALL?\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    
    str = strstr(buffer, "+MIPCALL:");
    if (str == NULL) {
        return -1;
    }
    
    sscanf(str+strlen("+MIPCALL: "), "%d", &apn_state);
    if (apn_state == 0) {
        return 0;
    } else if (apn_state == 1) {
        return 1;
    } else {
        return -1;
    }
}

static int l610_check_close_apn(void)
{
    at_echo_t echo;
    char buffer[64];
    char *str = NULL;
    int apn_state;
    
    apn_state = l610_check_apn();
    
    if (apn_state == -1) {
        return -1;
    } else if (apn_state == 0) {
        return 0;
    }

    tos_at_echo_create(&echo, buffer, 64, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+MIPCALL=0\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    
    str = strstr(buffer, "+MIPCALL:");
    if (str == NULL) {
        return -1;
    }
    
    sscanf(str+strlen("+MIPCALL: "), "%d", &apn_state);
    if (apn_state == 0) {
        return 0;
    }
   
    return -1;
}

static int l610_set_apn(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+MIPCALL");
    tos_at_cmd_exec_until(&echo, 10000, "AT+MIPCALL=1,\"CMNET\"\r\n");
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }
    
    if (l610_check_apn() == 1) {
        return 0;
    } else {
        return -1;
    }
}

/*
 * config dev info to module, do this operate only once in factroy is suggested
 */
int l610_tencent_firmware_module_info_set(device_info_t *info, tls_mode_t mode)
{
    at_echo_t echo;
    
    if (!info) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+TCDEVINFOSET: OK");

    tos_at_cmd_exec_until(&echo, 2000, "AT+TCDEVINFOSET=%d,\"%s\",\"%s\",\"%s\"\r\n",
                        mode, info->product_id, info->device_name, info->device_serc);
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

/* mqtt setup connect */
int l610_tencent_firmware_module_mqtt_conn(mqtt_param_t init_params)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTCONN: OK");
    while (try++ < 10) {
        tos_at_cmd_exec_until(&echo, 2000, "AT+TCMQTTCONN=%d,%d,%d,%d,%d\r\n", init_params.tls_mode,
            init_params.command_timeout, init_params.keep_alive_interval_ms,
            init_params.clean_session, init_params.auto_connect_enable);
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

/* mqtt disconn */
int l610_tencent_firmware_module_mqtt_discon(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);

    tos_at_cmd_exec(&echo, 1000, "AT+TCMQTTDISCONN\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

int l610_tencent_firmware_module_mqtt_publ(const char *topic, qos_t qos, char *payload)
{
    at_echo_t echo;
    
    if (!topic || !payload) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, ">");

    tos_at_cmd_exec_until(&echo, 1000, "AT+TCMQTTPUBL=\"%s\",%d,%d\r\n", topic, qos, strlen(payload)-2);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTPUB: OK");
    tos_at_raw_data_send_until(&echo, 1000, (uint8_t *)payload, strlen(payload));
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }

    return 0;
}

/* mqtt pub msg */
int l610_tencent_firmware_module_mqtt_pub(const char *topic, qos_t qos, char *payload)
{
    at_echo_t echo;
    
    if (!topic || !payload) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTPUB: OK");

    tos_at_cmd_exec_until(&echo, 1000, "AT+TCMQTTPUB=\"%s\",%d,\"%s\"\r\n", topic, qos, payload);
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

int l610_tencent_firmware_module_mqtt_sub(char *topic, qos_t qos)
{
    at_echo_t echo;
    
    if (!topic) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTSUB: OK");

    tos_at_cmd_exec_until(&echo, 2000, "AT+TCMQTTSUB=\"%s\",%d\r\n", topic, qos);
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

int l610_tencent_firmware_module_mqtt_unsub(const char *topic)
{
    at_echo_t echo;
    
    if (!topic) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "+TCMQTTUNSUB: OK");

    tos_at_cmd_exec_until(&echo, 2000, "AT+TCMQTTUNSUB=\"%s\"\r\n", topic);
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

int l610_tencent_firmware_module_mqtt_state_get(mqtt_state_t *state)
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
    if (!str) {
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

int l610_tencent_firmware_module_debug_level_set(int log_level)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);

    tos_at_cmd_exec(&echo, 1000, "AT+TCTEMLOG=%d\r\n", log_level);
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

#if 0  // l610 tencent_firmware not support AT+TCREGNET now
static int l610_tencent_firmware_join_net(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 2) {
        tos_at_cmd_exec(&echo, 5000, "AT+TCREGNET?\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}
#endif  

static int l610_tencent_firmware_init(void)
{
    printf("Init l610 with tencent firmware ...\n");
    
    if (l610_check_ready() != 0) {
        printf("wait module ready timeout, please check your module\n");
        return -1;
    }
    
    if (l610_echo_close() != 0) {
        printf("echo close failed,please check your module\n");
        return -1;
    }
		
    if (l610_sim_card_check() != 0) {
        printf("sim card check failed,please insert your card\n");
        return -1;
    }

    if (l610_signal_quality_check() != 0) {
        printf("signal quality check status failed\n");
        return -1;
    }
		
    if (l610_gsm_network_check() != 0) {
        printf("GSM network register status check fail\n");
        return -1;
    }
    
    if (l610_gprs_network_check() != 0) {
        printf("GPRS network register status check fail\n");
        return -1;
    }
    
    if (l610_set_data_format() != 0) {
        printf("set data format fail\n");
        return -1;
    }
    
    if (l610_check_close_apn() != 0) {
        printf("close apn failed\n");
        return -1;
    }
		
    if (l610_set_apn() != 0) {
        printf("apn set FAILED\n");
        return -1;
    }

    printf("Init l610 with tencent firmware Done\n" );

    return 0;
}

int l610_tencent_firmware_ota_set(ota_mode_t mode, char *version)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+TCOTASET:OK");

    tos_at_cmd_exec_until(&echo, 2000, "AT+TCOTASET=%d,\"%s\"\r\n", mode, version);
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}
k_sem_t ota_fw_info_sem;

int l610_tencent_firmware_ota_read_fwinfo(ota_fw_info_t *ota_fw_info)
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

    tos_at_cmd_exec_until(&echo, 2000, "AT+TCFWINFO?\r\n");
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        
        sscanf(echo_buffer, "%*[^\"]%*c%[^\"]%*[^,]%*c%[^,]%*[^\"]%*c%[^\"]", ota_fw_info->fw_version, FileSize, ota_fw_info->fw_md5);
        for(int i = 0; i<10; i++) {
            if(FileSize[i] == 0) {
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

static int l610_tencent_firmware_ota_read_fwdata(uint8_t *ota_fw_data_buffer,uint16_t read_len)
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

void l610_tencent_firmware_recvpub(void)
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

void l610_tencent_firmware_recvcmd(void)
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

void l610_tencent_firmware_recvfwdata(void)
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

at_event_t l610_tencent_firmware_at_event[] = {
    { "+TCMQTTRCVPUB: ", l610_tencent_firmware_recvpub },
    { "+TCREADFWDATA:", l610_tencent_firmware_recvfwdata },
    { "+TCOTASTATUS:", l610_tencent_firmware_recvcmd },
};

tencent_firmware_module_t tencent_firmware_module_l610 = {
    .init               = l610_tencent_firmware_init,
    .info_set           = l610_tencent_firmware_module_info_set, 
    .mqtt_conn          = l610_tencent_firmware_module_mqtt_conn,        
    .mqtt_discon        = l610_tencent_firmware_module_mqtt_discon,
    .mqtt_pub           = l610_tencent_firmware_module_mqtt_pub,
    .mqtt_publ          = l610_tencent_firmware_module_mqtt_publ,
    .mqtt_sub           = l610_tencent_firmware_module_mqtt_sub,
    .mqtt_unsub         = l610_tencent_firmware_module_mqtt_unsub,
    .mqtt_state_get     = l610_tencent_firmware_module_mqtt_state_get,
    .debug_level_set    = l610_tencent_firmware_module_debug_level_set,
    .ota_set            = l610_tencent_firmware_ota_set,
    .ota_read_fwinfo    = l610_tencent_firmware_ota_read_fwinfo,
    .ota_read_fwdata    = l610_tencent_firmware_ota_read_fwdata,
};

int l610_tencent_firmware_sal_init(hal_uart_port_t uart_port)
{
    int ret = -1;
    
    if (tos_at_init(uart_port, l610_tencent_firmware_at_event,
                        sizeof(l610_tencent_firmware_at_event) /
                        sizeof(l610_tencent_firmware_at_event[0])) != 0) {
        return -1;
    }

    if (tos_tf_module_register(&tencent_firmware_module_l610) != 0) {
        return -1;
    }

    if ((ret = tos_tf_module_init()) != 0) {
        return ret;
    }

    return 0;
}

