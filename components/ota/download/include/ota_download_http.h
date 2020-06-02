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

#ifndef _OTA_DOWNLOAD_HTTP_H_
#define  _OTA_DOWNLOAD_HTTP_H_

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

__API__ int ota_download_http(const char *url);

#endif /* _OTA_DOWNLOAD_HTTP_H_ */

