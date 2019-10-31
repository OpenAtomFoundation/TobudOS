/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_ota_internal.h"

/* ofc, OTA fetch channel */

typedef struct {

    const char *url;
    httpclient_t http;              /* http client */
    httpclient_data_t http_data;    /* http client data */

} otahttp_Struct_t, *otahttp_Struct_pt;

extern int httpclient_common(httpclient_t *client,
                             const char *url,
                             int port,
                             const char *ca_crt,
                             HTTPCLIENT_REQUEST_TYPE method,
                             uint32_t timeout_ms,
                             httpclient_data_t *client_data);

void *ofc_Init(char *url)
{
    otahttp_Struct_pt h_odc;

    if (NULL == (h_odc = OTA_MALLOC(sizeof(otahttp_Struct_t)))) {
        OTA_LOG_ERROR("allocate for h_odc failed");
        return NULL;
    }

    memset(h_odc, 0, sizeof(otahttp_Struct_t));

    /* set http request-header parameter */
    h_odc->http.header = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" \
                         "Accept-Encoding: gzip, deflate\r\n";
#if defined(SUPPORT_ITLS)
    char *s_ptr = strstr(url, "://");
    if (strlen("https") == (s_ptr - url) && (0 == strncmp(url, "https", strlen("https")))) {
        strncpy(url + 1, url, strlen("http"));
        url++;
    }
#endif
    h_odc->url = url;

    return h_odc;
}


extern const char *iotx_ca_crt;

int32_t ofc_Fetch(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_s)
{
    int                 diff;
    otahttp_Struct_pt   h_odc = (otahttp_Struct_pt)handle;

    h_odc->http_data.response_buf = buf;
    h_odc->http_data.response_buf_len = buf_len;
    diff = h_odc->http_data.response_content_len - h_odc->http_data.retrieve_len;

#if !defined(SUPPORT_TLS)
    if (0 != httpclient_common(&h_odc->http, h_odc->url, 80, 0, HTTPCLIENT_GET, timeout_s * 1000,
                               &h_odc->http_data)) {
#else
    if (0 != httpclient_common(&h_odc->http, h_odc->url, 443, iotx_ca_crt, HTTPCLIENT_GET, timeout_s * 1000,
                               &h_odc->http_data)) {
#endif
        OTA_LOG_ERROR("fetch firmware failed");
        return -1;
    }

    return h_odc->http_data.response_content_len - h_odc->http_data.retrieve_len - diff;
}


int ofc_Deinit(void *handle)
{
    if (NULL != handle) {
        OTA_FREE(handle);
    }

    return 0;
}



