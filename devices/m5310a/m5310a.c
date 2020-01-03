#include "m5310a.h"
#include "tos_k.h"
#include "tos_at.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"


static int m5310a_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
     return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int m5310a_recv(int id, void *buf, size_t len)
{
    return m5310a_recv_timeout(id, buf, len, (uint32_t)10000);
}

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

static int m5310a_wait_ok(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }

    return -1;
}

static int m5310a_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id, try = 0, is_connected = 0;
    at_echo_t echo;
    char echo_buffer[32];

    at_delay_ms(10000);
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);

    tos_at_cmd_exec(&echo, 4000, "AT+NSOCR=%s,%d\r\n",
                        proto == TOS_SAL_PROTO_UDP ? "DGRAM" : "STREAM",
                        proto == TOS_SAL_PROTO_UDP ? 17 : 6);
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    sscanf(echo.buffer, "%d", &id);

    id = tos_at_channel_alloc_id(id, ip, port);
    if (id == -1) {
        return -1;
    }

    m5310a_wait_ok();

    at_delay_ms(3000);

    while (is_connected == 0 && try < 10) {
        tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "CONNECT OK");
        tos_at_cmd_exec(&echo, 1000, "AT+NSOCO=%d,%s,%s\r\n", id, ip, port);
		for(int j=0; j<5; j++) {
			if(echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
				is_connected = 1;
				break;
			}
			tos_task_delay(tos_millisec2tick(1000));
		}

		try++;
    }

    if (!is_connected) {
        tos_at_channel_free(id);
        return -1;
    }

    return id;
}


static int m5310a_send(int id, const void *buf, size_t len)
{
    char *str_buf = NULL;
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    const int max_send_len = 200;
    str_buf = tos_mmheap_calloc(2 * max_send_len + 1, sizeof(char));
    if (!str_buf) {
        return -1;
    }


    int left_len = len;
    for(int i=0; left_len>0; i++) {
    	int send_len = left_len > max_send_len ? max_send_len : left_len;
    	__hex2str((uint8_t *)buf+i*max_send_len, str_buf, send_len);

    	char except[8];
    	snprintf(except, sizeof(except), "%d,%d", id, send_len);
		tos_at_echo_create(&echo, NULL, 0, except);
		tos_at_cmd_exec(&echo, 1000, "AT+NSOSD=%d,%d,%s\r\n", id, send_len, str_buf);

		for(int j=0; j<10; j++) {
			if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT) {
				break;
			}

			if(echo.status != AT_ECHO_STATUS_EXPECT) {
			    at_delay_ms(1000);
			}
		}

	    left_len -= send_len;
    }

    tos_mmheap_free(str_buf);

    tos_at_global_lock_post();

    return len - left_len;
}


static int m5310a_close(int id)
{
    tos_at_cmd_exec(NULL, 1000, "AT+NSOCL=%d\r\n", id);

    tos_at_channel_free(id);

	printf("close %d called\n", id);

    return 0;
}

static int m5310a_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    char *str;
    at_echo_t echo;
    char echo_buffer[64];
    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "+CMDNS:");
    tos_at_cmd_exec(&echo, 60000, "AT+CMDNS=\"%s\"\r\n", host_name);
    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT) {
        return -1;
    }

    str = strstr((const char *)echo.buffer, "+CMDNS:");
    if(str == NULL) {
    	return -1;
    }

    sscanf(str, "+CMDNS:%s", host_ip);
    host_ip[host_ip_len - 1] = '\0';
    printf("GOT IP: %s\n", host_ip);
    return 0;
}

static int m5310a_reset(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "M5310-A");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 8000, "AT+NRB\r\n");
        if (echo.status == AT_ECHO_STATUS_EXPECT && m5310a_wait_ok() == 0) {
            return 0;
        }
    }
    return -1;
}

static int m5310a_open_cfun(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CFUN=1\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int m5310a_check_cfun(void)
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

static int m5310a_get_net(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CGATT=1\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

static int m5310a_signal_quality_check(void)
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

static int m5310a_init(void)
{
    printf("Init M5310A ...\n" );

    if (m5310a_wait_ok() != 0) {
        printf("wait ok FAILED\n");
        return -1;
    }

    if (m5310a_reset() != 0) {
        printf("reset FAILED\n");
        return -1;
    }


    if (m5310a_open_cfun() != 0) {
        printf("open_cfun FAILED\n");
        return -1;
    }

    if (m5310a_check_cfun() != 0) {
        printf("check cfun FAILED\n");
        return -1;
    }

    if (m5310a_get_net() != 0) {
        printf("get net FAILED\n");
        return -1;
    }

    if (m5310a_signal_quality_check() != 0) {
        printf("check csq FAILED\n");
        return -1;
    }

    printf("Init m5310a Done\n" );

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

__STATIC__ uint8_t incoming_data_buffer[512];

__STATIC__ void m5310a_incoming_data(void)
{
    uint8_t data=0;
    int comma_cnt = 1;
    int socket = 0;
    int data_len = 0;

    /*+NSORF:0,1.2.3.4,1234,5,313131310A,0*/
    while (data != ',') {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }
        if(data >= '0' && data <= '9') {
        	socket = socket * 10 + (data - '0');
        }
    }

    while(comma_cnt < 3) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }
        comma_cnt += (data == ',') ? 1 : 0;
    }

    // get data len
    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data < '0' || data > '9') {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }

    int bufsz = sizeof(incoming_data_buffer) / 2;
    bufsz *= 2; // keep bufsz is even number

    while(data_len > 0) {
        int read_cnt = 0;
        if(bufsz < data_len*2) {
            read_cnt = bufsz;
            data_len -= bufsz/2;
        } else {
            read_cnt = data_len*2;
            data_len = 0;
        }
        for(int i=0; i<read_cnt; i++) {
            if (tos_at_uart_read_timed(&data, 1, 1000) != 1) {
                return;
            }
            incoming_data_buffer[i] = data;
        }
        __asciistr2hex(incoming_data_buffer, incoming_data_buffer, read_cnt);
        tos_at_channel_write(socket, incoming_data_buffer, read_cnt/2);
    }

    while(tos_at_uart_read_timed(&data, 1, 1000) == 1) { }
}

__STATIC__ void m5310a_incoming_udp(void) {
    // +NSONMI:0,5
    // 0 socket
    // 5 length
	uint8_t data = 0;
    int socket = 0;
    int length = 0;
    while (data != ',') {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if(data >= '0' && data <= '9') {
        	socket = socket * 10 + (data - '0');
        }
    }

    while (data != '\n') {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if(data >= '0' && data <= '9') {
            length = length * 10 + (data - '0');
        }
    }

    at_echo_t echo;
    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1, "AT+NSORF=%d,%d\r\n", socket, length);
}

__STATIC__ void m5310a_socket_close_event(void) {
    int id = -1;
    uint8_t data;
    while (data != '\n') {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }
        if(data >= '0' && data <= '9') {
            id = id * 10 + (data - '0');
        }
    }
    tos_at_channel_free(id);
}

at_event_t m5310a_at_event[] = {
    { "+NSORF:",    m5310a_incoming_data },
    { "+NSONMI:",   m5310a_incoming_udp },
    { "+NSOCLI: ",  m5310a_socket_close_event },
};

sal_module_t sal_module_m5310a = {
    .init           = m5310a_init,
    .connect        = m5310a_connect,
    .send           = m5310a_send,
    .recv_timeout   = m5310a_recv_timeout,
    .recv           = m5310a_recv,
    .close          = m5310a_close,
    .parse_domain   = m5310a_parse_domain,
};

int m5310a_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, m5310a_at_event,
                        sizeof(m5310a_at_event) / sizeof(m5310a_at_event[0])) != 0) {
        return -1;
    }

    if (tos_sal_module_register(&sal_module_m5310a) != 0) {
        return -1;
    }

    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}

