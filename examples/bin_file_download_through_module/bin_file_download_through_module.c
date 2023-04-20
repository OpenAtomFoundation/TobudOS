#include "mcu_init.h"
#include "sal_module_wrapper.h"

#define USE_ESP8266

#ifdef USE_ESP8266
#include "esp8266.h"
#endif

#define HTTP_REQUEST_TEMPLATE       \
"GET /%s HTTP/1.1\r\nAccept:*/*\r\n\
User-Agent: Mozilla/5.0\r\n\
Cache-Control: no-cache\r\n\
Connection: close\r\n\
Host:%s:%d\r\n\r\n"

#define HOST_MAX            32
#define FILE_MAX            32
#define PORT_MAX            5
#define HTTP_REQUEST_MAX    (sizeof(HTTP_REQUEST_TEMPLATE) + HOST_MAX + FILE_MAX + PORT_MAX)

#define HTTP_RESPONSE_STATUS_CODE_OK        200

typedef enum http_parse_status_en {
    HTTP_PARSE_STATUS_NONE,
    HTTP_PARSE_STATUS_BLANK_LINE,
    HTTP_PARSE_STATUS_HEADER,
    HTTP_PARSE_STATUS_OVERFLOW,
    HTTP_PARSE_STATUS_TIMEOUT,
} http_parse_status_t;

static uint16_t ota_str2uint16(char *str)
{
    uint16_t r = 0;

    while (*str) {
        r = r * 10 + *str++ - '0';
    }

    return r;
}

static int ota_url_parse(char *url, char *host, uint16_t *port, char *file)
{
    /* http://192.168.1.107:80/test_2048.bin */
    char *p;
    int len;
    static char the_port[PORT_MAX + 1];

    /* 1. skip http url prefix */
#define HTTP_URL_PREFIX         "http://"
    if (strncmp(url, HTTP_URL_PREFIX, strlen(HTTP_URL_PREFIX)) != 0) {
        return -1;
    }

    url += strlen(HTTP_URL_PREFIX);

    /* 2. parse host */
    p = strstr(url, ":");
    if (!p || (len = p - url) > HOST_MAX) { /* ":" not found or host name too long */
        return -1;
    }

    strncpy(host, url, len);
    host[len] = '\0';

    /* 3. parse port */
    url = p + 1;
    p = strstr(url, "/");
    if (!p || (len = p - url) > PORT_MAX) { /* ":" not found or port too long */
        return -1;
    }

    strncpy(the_port, url, len);
    the_port[len] = '\0';
    *port = ota_str2uint16(the_port);

    /* 4. parse file */
    url = p + 1;
    if (strlen(url) > FILE_MAX) {
        return -1;
    }
    strcpy(file, url);

    return 0;
}

static void ota_http_request_construct(char *http_request, char *host, uint16_t port, char *file)
{
    snprintf(http_request, HTTP_REQUEST_MAX, HTTP_REQUEST_TEMPLATE, file, host, port);
}

static http_parse_status_t ota_http_response_line_parse(int fd, char *line_buf, size_t buf_len)
{
    size_t curr_len = 0;
    k_stopwatch_t stopwatch;
    uint8_t data, last_data = 0;

    memset(line_buf, 0, buf_len);

    tos_stopwatch_create(&stopwatch);
    tos_stopwatch_countdown_ms(&stopwatch, 8000);
    while (K_TRUE) {
        if (tos_stopwatch_is_expired(&stopwatch)) {
            return HTTP_PARSE_STATUS_TIMEOUT;
        }

        if (curr_len >= buf_len) {
             return HTTP_PARSE_STATUS_OVERFLOW;
        }

        if (tos_sal_module_recv_timeout(fd, &data, 1, 1000) <= 0) {
            continue;
        }

        if (data == '\n' && last_data == '\r') {
            curr_len -= 1;
            line_buf[curr_len] = '\0';

            if (curr_len == 0) {
                return HTTP_PARSE_STATUS_BLANK_LINE;
            }

            return HTTP_PARSE_STATUS_HEADER;
        }

        line_buf[curr_len++] = data;
        last_data = data;
    }
}

static void hex_dump(uint8_t *buf, size_t len)
{
    uint16_t i;
    
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if ((i+1) % 16 == 0) {
            printf("\r\n");
        }
    }
}

static int ota_http_body_read(int fd, int file_len, unsigned char *buf, size_t buf_len)
{
    int remain_len = file_len, read_len;
    int ret;
    
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
    
    printf("file_len is %d,  remain_len = %d\r\n", file_len, remain_len);
    
    while (remain_len > 0) {
        read_len = MIN(remain_len, buf_len);
        
        printf("remain_len is %d\r\n", remain_len);
        
        if ((ret = tos_sal_module_recv(fd, buf, read_len)) != read_len) {
            printf("tos_sal_module_recv fail, ret is %d\r\n", ret);
            
            return -1;
        }
        
        printf("tos_sal_module_recv success, read_len is %d\r\n", read_len);

        hex_dump(buf, read_len);

        remain_len -= read_len;
    }

    return 0;
}

static int ota_http_response_parse(int fd)
{
#define HTTP_HEADER_MAX         128
#define HEADER_FIELD_MAX        30
    static char line_buf[HTTP_HEADER_MAX];
    static char header_field[HEADER_FIELD_MAX];

    http_parse_status_t status;
    int header_value, file_len;
    int is_response_status_ok = K_FALSE;

    do {
        printf("line buf:[%s]\r\n", line_buf);
        status = ota_http_response_line_parse(fd, line_buf, sizeof(line_buf));

        if (strncmp("HTTP/", line_buf, strlen("HTTP/")) == 0) {
            sscanf(line_buf, "%s %d", header_field, &header_value);
            if (header_value != HTTP_RESPONSE_STATUS_CODE_OK) {
                return -1;
            }

            is_response_status_ok = K_TRUE;
        } else if (strncmp("Content-Length:", line_buf, strlen("Content-Length:")) == 0) {
            sscanf(line_buf, "%s %d", header_field, &header_value);
            file_len = header_value;
        }
    } while (status == HTTP_PARSE_STATUS_HEADER);

    if (!is_response_status_ok) {
        return -1;
    }

    if (status == HTTP_PARSE_STATUS_OVERFLOW ||
        status == HTTP_PARSE_STATUS_TIMEOUT ||
        file_len <= 0) {
        return -1;
    }

    /* must be HTTP_PARSE_STATUS_BLANK_LINE, means the body begins */
    if (ota_http_body_read(fd, file_len, (unsigned char *)line_buf, sizeof(line_buf)) < 0) {
        return -1;
    }

    return 0;
}

void application_entry(void *arg)
{
    int fd;
    int ret;
    char host[HOST_MAX];
    char port[PORT_MAX];
    char file[FILE_MAX];
    uint16_t port_num;
    char http_request[HTTP_REQUEST_MAX];
    
    /* The different size test bin files is supported in example folder */
    char *url = "http://192.168.1.107:80/test_2048.bin";

#if defined(USE_ESP8266)
    esp8266_sal_init(HAL_UART_PORT_0);
    esp8266_join_ap("Tencent-WIFI", "424424424");
#endif
    
    ret = ota_url_parse(url, host, &port_num, file);
    if (ret != 0) {
        printf("url parser fail, ret is %d\r\n", ret);
        return;
    } else {
        sprintf(port, "%d", port_num);
        printf("url parser success:\r\n");
        printf("  -host:%s\r\n", host);
        printf("  -port:%s\r\n", port);
        printf("  -file:%s\r\n", file);
    }
    
    ota_http_request_construct(http_request, host, port_num, file);
    
    /* 1. connect to host */
    fd = tos_sal_module_connect(host, port, TOS_SAL_PROTO_TCP); 
    if (fd == -1) {
        printf("TCP connect failed\r\n");
    } else {
        printf("TCP connect success! fd: %d\n", fd);
    }
    
    /* 2. send http request */
     if (tos_sal_module_send(fd, (void *)http_request, strlen(http_request)) < 0) {
        printf("TCP send http request fail\r\n");
        tos_sal_module_close(fd);
        return;
    } else {
        printf("TCP send http request success\r\n");
    }
    
    /* 3. parse http response */
    ret = ota_http_response_parse(fd);

    /* 4. free socket */
    tos_sal_module_close(fd);
    printf("TCP disconnect success!\r\n");
    
     while (1) {
        printf("ota http download finish!\r\n");
        tos_task_delay(1000);
    }

}

