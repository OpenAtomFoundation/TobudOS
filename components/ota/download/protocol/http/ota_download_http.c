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

#include "tos_kv.h"
#include "ota_download.h"

__STATIC__ uint16_t ota_str2uint16(char *str)
{
    uint16_t r = 0;

    while (*str) {
        r = r * 10 + *str++ - '0';
    }

    return r;
}

__STATIC__ int ota_url_parse(char *url, char *host, uint16_t *port, char *file)
{
    /* http://39.108.190.129:8080/firmware.bin */
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

__STATIC_INLINE__ void ota_http_request_construct(char *http_request, char *host, uint16_t port, char *file)
{
    snprintf(http_request, HTTP_REQUEST_MAX, HTTP_REQUEST_TEMPLATE, file, host, port);
}

__STATIC__ http_parse_status_t ota_http_response_line_parse(int fd, char *line_buf, size_t buf_len)
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

        if (hal_tcp_read(fd, &data, 1, 1000) <= 0) {
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

__STATIC__ int ota_http_body_write2flash(int fd, int file_len, unsigned char *buf, size_t buf_len)
{
    uint8_t crc = 0, the_crc;
    ota_img_vs_t new_version;
    int remain_len = file_len, read_len;
    uint32_t flash_addr = ota_partition_start(OTA_PARTITION_OTA);

    /* is the file too large? */
    if (file_len > ota_partition_size(OTA_PARTITION_OTA)) {
        return -1;
    }

    /* we need the buf_len to be flash write-align aligned */
    if (!ota_flash_size_is_aligned(buf_len)) {
        return -1;
    }

    /* 1. make flash ready */
    if (ota_flash_erase_blocks(flash_addr, file_len) < 0) {
        return -1;
    }

    /* 2. deal with image header */
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
    read_len = MIN(remain_len, buf_len);
    if (hal_tcp_read(fd, buf, read_len, 2000) != read_len) {
        return -1;
    }

    if (read_len < sizeof(ota_img_hdr_t)) {
        /* read_len at least larger than ota_img_hdr_t */
        return -1;
    }

    if (ota_flash_write(flash_addr, buf, ota_flash_write_size_aligned_get(read_len)) < 0) {
        return -1;
    }

    /* get original image header */
    the_crc = ((ota_img_hdr_t *)buf)->patch_crc;
    /* patch version */
    new_version = ((ota_img_hdr_t *)buf)->new_version;

    /* calculate image header crc */
    crc = ota_img_hdr_crc((ota_img_hdr_t *)buf);
    /* calculate remain image body crc */
    crc = crc8(crc, buf + sizeof(ota_img_hdr_t), read_len - sizeof(ota_img_hdr_t));

    flash_addr += read_len;
    remain_len -= read_len;

    /* 3. read file content and write to flash */
    while (remain_len > 0) {
        read_len = MIN(remain_len, buf_len);
        if (hal_tcp_read(fd, buf, read_len, 2000) != read_len) {
            return -1;
        }

        if (ota_flash_write(flash_addr, buf, ota_flash_write_size_aligned_get(read_len)) < 0) {
            return -1;
        }

        crc = crc8(crc, buf, read_len);

        flash_addr += read_len;
        remain_len -= read_len;
    }

    /* 4. crc check */
    if (the_crc != crc) {
        return -1;
    }

    if (tos_kv_set("new_version", &new_version, sizeof(ota_img_vs_t)) != KV_ERR_NONE) {
        return -1;
    }

    return 0;
}

__STATIC__ int ota_http_response_parse(int fd)
{
#define HTTP_HEADER_MAX         128
#define HEADER_FIELD_MAX        30
    static char line_buf[HTTP_HEADER_MAX];
    static char header_field[HEADER_FIELD_MAX];

    http_parse_status_t status;
    int header_value, file_len;
    int is_response_status_ok = K_FALSE;

    do {
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
    if (ota_http_body_write2flash(fd, file_len, (unsigned char *)line_buf, sizeof(line_buf)) < 0) {
        return -1;
    }

    return 0;
}

__API__ int ota_download_http(const char *url)
{
    int fd, rc = 0;
    uint16_t port;
    static char host[HOST_MAX + 1], file[FILE_MAX + 1];
    static char http_request[HTTP_REQUEST_MAX];

    if (!url) {
        return -1;
    }

    if (ota_url_parse((char *)url, host, &port, file) != 0) {
        return -1;
    }

    ota_http_request_construct(http_request, host, port, file);

    /* 1. connect to host */
    fd = hal_tcp_connect(host, port);
    if (fd < 0) {
        return -1;
    }

    /* 2. send http request */
    if (hal_tcp_write(fd, (unsigned char *)http_request, strlen(http_request), 0) < 0) {
        hal_tcp_disconnect(fd);
        return -1;
    }

    /* 3. parse http response */
    rc = ota_http_response_parse(fd);

    /* 4. free socket */
    hal_tcp_disconnect(fd);

    return rc;
}

