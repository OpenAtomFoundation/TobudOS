/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ota_fetch.h"

#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#include "qcloud_iot_ca.h"
#include "utils_httpc.h"



#define OTA_HTTP_HEAD_CONTENT_LEN    256

/* ofc, OTA fetch channel */

typedef struct {

    const char *url;
    HTTPClient http;            /* http client */
    HTTPClientData http_data;   /* http client data */

} OTAHTTPStruct;

#ifdef OTA_USE_HTTPS
static int is_begin_with(const char * str1,char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    char *p = str2;
    int i = 0;
    while(*p != '\0')
    {
        if(*p != str1[i])
        return 0;
        p++;
        i++;
    }
    return 1;
}
#endif

static char sg_head_content[OTA_HTTP_HEAD_CONTENT_LEN];
void *ofc_Init(const char *url, uint32_t offset, uint32_t size)
{    
    OTAHTTPStruct *h_odc;

    if (NULL == (h_odc = HAL_Malloc(sizeof(OTAHTTPStruct)))) {
        Log_e("allocate for h_odc failed");
        return NULL;
    }

    memset(h_odc, 0, sizeof(OTAHTTPStruct));
	memset(sg_head_content, 0, OTA_HTTP_HEAD_CONTENT_LEN);
	HAL_Snprintf(sg_head_content, OTA_HTTP_HEAD_CONTENT_LEN,\
					"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"\
					"Accept-Encoding: gzip, deflate\r\n"\
					"Range: bytes=%d-%d\r\n",
					offset, size);

	Log_d("head_content:%s", sg_head_content);
    /* set http request-header parameter */
    h_odc->http.header = sg_head_content;
    h_odc->url = url;

    return h_odc;
}

int32_t qcloud_ofc_connect(void *handle)
{
    IOT_FUNC_ENTRY;

    OTAHTTPStruct * h_odc = (OTAHTTPStruct *)handle;

    int port = 80;
    const char *ca_crt = NULL;

#ifdef OTA_USE_HTTPS
    if (is_begin_with(h_odc->url, "https")) 
    {
        port = 443;
        ca_crt = iot_https_ca_get();
    }
#endif

    int32_t rc = qcloud_http_client_common(&h_odc->http, h_odc->url, port, ca_crt, HTTP_GET, &h_odc->http_data);

    IOT_FUNC_EXIT_RC(rc);
}


int32_t qcloud_ofc_fetch(void *handle, char *buf, uint32_t bufLen, uint32_t timeout_s)
{
    IOT_FUNC_ENTRY;

    int diff;
    OTAHTTPStruct * h_odc = (OTAHTTPStruct *)handle;

    h_odc->http_data.response_buf = buf;
    h_odc->http_data.response_buf_len = bufLen;
    diff = h_odc->http_data.response_content_len - h_odc->http_data.retrieve_len;
    
    int rc = qcloud_http_recv_data(&h_odc->http, timeout_s * 1000, &h_odc->http_data);
    if (QCLOUD_RET_SUCCESS != rc) {
        if (rc == QCLOUD_ERR_HTTP_NOT_FOUND)
            IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FETCH_NOT_EXIST);
        
        if (rc == QCLOUD_ERR_HTTP_AUTH)
            IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FETCH_AUTH_FAIL);
        
        if (rc == QCLOUD_ERR_HTTP_TIMEOUT)
            IOT_FUNC_EXIT_RC(IOT_OTA_ERR_FETCH_TIMEOUT);
        
        IOT_FUNC_EXIT_RC(rc);
    }

    IOT_FUNC_EXIT_RC(h_odc->http_data.response_content_len - h_odc->http_data.retrieve_len - diff);
}


int qcloud_ofc_deinit(void *handle)
{
    IOT_FUNC_ENTRY;
    if (NULL != handle) {
        HAL_Free(handle);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

#ifdef __cplusplus
}
#endif
