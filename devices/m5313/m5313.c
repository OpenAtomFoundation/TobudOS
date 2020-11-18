#include "m5313.h"
#include "tos_k.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"

#define M5313_USE_MUX 1	//使用多路链接

static int m5313_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
     return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int m5313_recv(int id, void *buf, size_t len)
{
    return m5313_recv_timeout(id, buf, len, (uint32_t)10000);
}

static int m5313_connect(const char *ip, const char *port, sal_proto_t proto)
{
	int id;
    at_echo_t echo;

	char buffer[100];

	id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }

	sprintf(buffer,"%d,CONNECT OK\r\n",id);
	
	tos_at_echo_create(&echo, NULL, 0, buffer);
	#if defined(M5313_USE_MUX)
	tos_at_cmd_exec_until(&echo, 10000, "AT+IPSTART=%d,\"%s\",\"%s\",%s\r\n",id,
						proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP",
						ip,port);
	#else
	tos_at_cmd_exec_until(&echo, 10000, "AT+IPSTART=\"%s\",\"%s\",%s\r\n",
						proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP",
						ip,port);
	#endif
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
		return id;
	}
	tos_at_channel_free(id);
	return -1;
}


static int m5313_send(int id, const void *buf, size_t len)
{
	at_echo_t echo;
	
    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }
	
    tos_at_echo_create(&echo, NULL, 0, ">");
	#if defined(M5313_USE_MUX)
	tos_at_cmd_exec_until(&echo, 3000, "AT+IPSEND=%d,%d\r\n",id,len);
	#else
	tos_at_cmd_exec_until(&echo, 3000, "AT+IPSEND=%d\r\n",len);
	#endif
	if (echo.status == AT_ECHO_STATUS_EXPECT) 
	{
		printf(buf);
		tos_at_echo_create(&echo, NULL, 0, "SEND OK");
		tos_at_raw_data_send_until(&echo, 10000, buf,len);
		if (echo.status == AT_ECHO_STATUS_EXPECT)
		{
			tos_at_global_lock_post();
			return len;
		}
	}
	tos_at_global_lock_post();
	return -1;
}


static int m5313_close(int id)
{
#if defined(M5313_USE_MUX)

    tos_at_cmd_exec(NULL, 1000, "AT+IPCLOSE=%d\r\n",id);

    tos_at_channel_free(id);

	printf("close %d called\n", id);
#else
	tos_at_cmd_exec(NULL, 1000, "AT+IPCLOSE\r\n");

    tos_at_channel_free(0);

	printf("close %d called\n", 0);
#endif

    return 0;
}

//多路链接
static int m5313_set_cmmux(void)
{
    tos_at_cmd_exec(NULL, 1000, "AT+CMMUX=1\r\n");

    return 0;
}

//单路链接
static int m5313_reset_cmmux(void)
{
    tos_at_cmd_exec(NULL, 1000, "AT+CMMUX=0\r\n");

    return 0;
}

static int m5313_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len) {
    char *str;
    at_echo_t echo;
    char echo_buffer[100];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "+CMDNSGIP:");
    tos_at_cmd_exec_until(&echo, 10000, "AT+CMDNSGIP=\"%s\"\r\n", host_name);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }
//	printf(echo.buffer);
    /* +CMDNSGIP:"name","1.2.3.4"\r\n */
    str = strstr((const char *)echo.buffer, "+CMDNSGIP:");
    if (!str) {
        return -1;
    }
    sscanf(str, "+CMDNSGIP:\"%*[^\"]\",\"%[^\"]",host_ip);
    host_ip[host_ip_len - 1] = '\0';

    int iplen = strlen(host_ip);
    if (iplen > 1 && host_ip[iplen-1] == '"') {
        host_ip[iplen-1] = 0;
    }

    printf("GOT IP: %s\n", host_ip);
    return 0;
}

static int m5313_reset(void)
{
    at_echo_t echo;

	HAL_GPIO_WritePin(NB_RST_GPIO_Port, NB_RST_Pin, GPIO_PIN_RESET);
	tos_task_delay(200);
	HAL_GPIO_WritePin(NB_RST_GPIO_Port, NB_RST_Pin, GPIO_PIN_SET);
	tos_task_delay(200);

    tos_at_echo_create(&echo, NULL, 0, "+CTZV:");
    tos_at_cmd_exec_until(&echo, 60000, "AT+TRB\r\n");
	if (echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }

    return 0;
}

static int m5313_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }

    return -1;
}

static int m5313_pin_ready(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+CPIN:READY");
    tos_at_cmd_exec_until(&echo, 5000, "AT+CPIN?\r\n");
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }

    return -1;
}


static int m5313_signal_quality_check(void)
{
    int rssi, ber;
    at_echo_t echo;
    char echo_buffer[32], *str;

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "+CSQ:");
    tos_at_cmd_exec_until(&echo, 5000, "AT+CSQ\r\n");
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }
	printf(echo.buffer);
    str = strstr(echo.buffer, "+CSQ:");
    sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
    if (rssi == 99) {
        return -1;
    }

    return 0;
}


//Network switching mode ---Automatic mode
static int m5313_Net_switch_mode(void)
{
	at_echo_t echo;
	tos_at_echo_create(&echo, NULL, 0, "+CFGDUALMODE: 1,0\r\n");
	tos_at_cmd_exec_until(&echo, 5000,  "AT+CFGDUALMODE?\r\n");				
	if (echo.status == AT_ECHO_STATUS_EXPECT){	
		return 0;		
	}else{
		tos_at_echo_create(&echo, NULL, 0, NULL);
		tos_at_cmd_exec(&echo, 2000,  "AT+CFGDUALMODE=1,0\r\n");				
		if(echo.status == AT_ECHO_STATUS_OK)
		{	
			if (m5313_reset() != 0) {
		        return -1;
		    }
			return 0;
		}		
	}
	return -1;
}
//
static int m5313_Check_net_type(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "+COPS: 0,2,\"46000\", 0");
    tos_at_cmd_exec_until(&echo, 8000,  "AT+COPS?\r\n");				
	if (echo.status != AT_ECHO_STATUS_EXPECT)
	{	
		tos_at_echo_create(&echo, NULL, 0, "+CTZV:");
		tos_at_cmd_exec_until(&echo, 25000,  "AT+CFGRATPRIO=2\r\n");				
		if (echo.status == AT_ECHO_STATUS_EXPECT)
		{					
			return 0;										
		}		
	}else{
		return 0;
	}
    return -1;
}
//
static int m5313_Check_sim_numb(void)
{
	at_echo_t echo;
	tos_at_echo_create(&echo, NULL, 0, NULL);
	tos_at_cmd_exec_until(&echo, 2000,  "AT+CIMI\r\n");				
	if (echo.status == AT_ECHO_STATUS_OK)
	{					
		return 0;													
	}	
	return -1;
}

static int m5313_cm_head(void)
{
	at_echo_t echo;
	tos_at_echo_create(&echo, NULL, 0, NULL);
	tos_at_cmd_exec(&echo, 1000,  "AT+CMHEAD=1\r\n");				
	if (echo.status == AT_ECHO_STATUS_OK)
	{					
		return 0;
	}
	return -1;
}

static int m5313_cm_showpt(void)
{
	at_echo_t echo;
	tos_at_echo_create(&echo, NULL, 0, NULL);
	tos_at_cmd_exec(&echo, 1000,  "AT+CMSHOWPT=1\r\n");				
	if (echo.status == AT_ECHO_STATUS_OK)
	{					
		return 0;												
	}	
	return -1;
}

static int m5313_cm_showra0(void)
{
	at_echo_t echo;
	tos_at_echo_create(&echo, NULL, 0, NULL);
	tos_at_cmd_exec(&echo, 1000,  "AT+CMSHOWRA=0\r\n");				
	if (echo.status == AT_ECHO_STATUS_OK)
	{					
		return 0;												
	}	
	return -1;
}


static int m5313_activate(void) {
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 5) {
        tos_at_cmd_exec(&echo, 3000, "AT+CGACT=1,1\r\n");
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }

    return -1;
}


//
static int m5313_Activate_pdp(void)
{
	at_echo_t echo;

	tos_at_echo_create(&echo, NULL, 0, "+CGACT: 1, 1\r\n");
	tos_at_cmd_exec_until(&echo, 2000,  "AT+CGACT?\r\n");				
	if (echo.status == AT_ECHO_STATUS_EXPECT)
	{			
		return 0;												
	}
	else
	{
		tos_at_echo_create(&echo, NULL, 0, NULL);
		tos_at_cmd_exec_until(&echo, 20000,  "AT+CGACT=1,1\r\n");				
		if (echo.status == AT_ECHO_STATUS_OK)
		{			
			return 0;												
		}
		
	}
	return -1;
}

static int m5313_init(void)
{
    printf("Init M5313 ...\n" );

 	if (m5313_reset() != 0) {
        printf("m5313 reset FAILED\n");
        return -1;
    }

    if (m5313_echo_close() != 0) {
        printf("m5313 echo close FAILED\n");
        return -1;
    }

	if (m5313_pin_ready() != 0) {
        printf("m5313 pin not ready\n");
        return -1;
    }

	if (m5313_Net_switch_mode() != 0) {
        printf("nb Netswitch mode err!\n");
//        return -1;
	}
	
	if (m5313_Check_net_type() != 0) {
        printf("nb net type err!\n");
	}
	
	if (m5313_signal_quality_check() != 0) {
        printf("nb signal err!\n");
	}
	
	if (m5313_activate() != 0) {
        printf("nb pdp err!\n");
	}

	
	if (m5313_cm_head() != 0) {
        printf("cm head err!\n");
	}


	if (m5313_cm_showpt() != 0) {
        printf("cm showpt err!\n");
	}
	
#if defined(M5313_USE_MUX)
	if (m5313_set_cmmux() != 0) {
        printf("cm showpt err!\n");
	}
#endif
	return 0;
}

#define M5313_INCOMING_DATA_LEN 2048
__STATIC__ uint8_t incoming_data_buffer[M5313_INCOMING_DATA_LEN];

__STATIC__ void m5313_server_ip_port(void)
{
	
}
#if defined(M5313_USE_MUX)
__STATIC__ void m5313_incoming_data(void)
{
	uint8_t data=0;
	int id = 0;
	int data_len = 0;
	int discard_suffix = 1;
	if (tos_at_uart_read(&data, 1) != 1) {
            return;
    }
	if(data >= '0' && data <= '9') {
		id = data - '0';
    }

	if (tos_at_uart_read(&data, 1) != 1) {
        return;
    }

	while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if(data >= '0' && data <= '9') {
            data_len = data_len * 10 + (data - '0');
		}else if(data == ':'){
			break;
		}else{
			return;
		}
    }

	if (data_len > sizeof(incoming_data_buffer)) {
        discard_suffix = 0;
        data_len = sizeof(incoming_data_buffer);
    }

    if (tos_at_uart_read(incoming_data_buffer, data_len) != data_len) {
        return;
    }

    tos_at_channel_write(id, incoming_data_buffer, data_len);

//	printf(incoming_data_buffer);
	
}

#else
__STATIC__ void m5313_incoming_data(void)
{
	uint8_t data=0;
	int data_len = 0;
	int discard_suffix = 1;

	while (data != ':') {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if(data >= '0' && data <= '9') {
            data_len = data_len * 10 + (data - '0');
        }
    }

	if (data_len > sizeof(incoming_data_buffer)) {
        discard_suffix = 0;
        data_len = sizeof(incoming_data_buffer);
    }

    if (tos_at_uart_read(incoming_data_buffer, data_len) != data_len) {
        return;
    }

    tos_at_channel_write(0, incoming_data_buffer, data_len);

//	printf(incoming_data_buffer);

}
#endif

at_event_t m5313_at_event[] = {
    {"+RECV FROM:",    m5313_server_ip_port},
#if defined(M5313_USE_MUX)
	{"+RECEIVE,",    m5313_incoming_data },
#else
	{"+IPD,",    m5313_incoming_data },
#endif

};

sal_module_t sal_module_m5313 = {
    .init           = m5313_init,
    .connect        = m5313_connect,
    .send           = m5313_send,
    .recv_timeout   = m5313_recv_timeout,
    .recv           = m5313_recv,
    .close          = m5313_close,
    .parse_domain   = m5313_parse_domain,
};

int m5313_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, m5313_at_event,
                        sizeof(m5313_at_event) / sizeof(m5313_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_m5313) != 0) {
        return -1;
    }

    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}

