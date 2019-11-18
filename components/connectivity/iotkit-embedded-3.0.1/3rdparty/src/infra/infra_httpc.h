/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef _INFRA_HTTPC_H_
#define _INFRA_HTTPC_H_

#include "infra_net.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup HttpClient
 * @{
 * HttpClient API implements the client-side of HTTP/1.1. It provides base interfaces to execute an HTTP request on a given URL. It also supports HTTPS (HTTP over TLS) to provide secure communication.\n
 * @section HttpClient_Usage_Chapter How to use this module
 * In this release, MediaTek provides two types of APIs: high level APIs and low level APIs.\n
 * - \b The \b high \b level \b APIs
 *  - Enables to execute a single HTTP request on a given URL.
 *  - Call #httpclient_get(), #httpclient_post(), #httpclient_put() or #httpclient_delete() to get, post, put or delete and HTTP request.\n
 * - \b The \b low \b level \b APIs
 *  - Enables to execute more than one HTTP requests during a Keep-Alive connection. Keep-alive is the idea of using a single TCP connection to send and receive multiple HTTP requests/responses, as opposed to opening a new connection for every single request/response pair.
 *  - Step1: Call #httpclient_connect() to connect to a remote server.
 *  - Step2: Call #httpclient_send_request() to send an HTTP request to the server.
 *  - Step3: Call #httpclient_recv_response() to receive an HTTP response from the server.
 *  - Step4: Repeat Steps 2 and 3 to execute more requests.
 *  - Step5: Call #httpclient_close() to close the connection.
 *  - Sample code: Please refer to the example under <sdk_root>/project/mt7687_hdk/apps/http_client/http_client_keepalive folder.
 */

/** @defgroup httpclient_define Define
 * @{
 */
/** @brief   This macro defines the HTTP port.  */
#define HTTP_PORT   80

/** @brief   This macro defines the HTTPS port.  */
#define HTTPS_PORT 443
/**
 * @}
 */

/** @defgroup httpclient_enum Enum
 *  @{
 */
/** @brief   This enumeration defines the HTTP request type.  */
typedef enum {
    HTTPCLIENT_GET,
    HTTPCLIENT_POST,
    HTTPCLIENT_PUT,
    HTTPCLIENT_DELETE,
    HTTPCLIENT_HEAD
} HTTPCLIENT_REQUEST_TYPE;


/** @defgroup httpclient_struct Struct
 * @{
 */
/** @brief   This structure defines the httpclient_t structure.  */
typedef struct {
    int                 remote_port;    /**< HTTP or HTTPS port. */
    utils_network_t     net;
    int                 response_code;  /**< Response code. */
    char               *header;         /**< Custom header. */
    char               *auth_user;      /**< Username for basic authentication. */
    char               *auth_password;  /**< Password for basic authentication. */
} httpclient_t;

/** @brief   This structure defines the HTTP data structure.  */
typedef struct {
    int     is_more;                /**< Indicates if more data needs to be retrieved. */
    int     is_chunked;             /**< Response data is encoded in portions/chunks.*/
    int     retrieve_len;           /**< Content length to be retrieved. */
    int     response_content_len;   /**< Response content length. */
    int     response_received_len;  /**< Response have received length. */
    int     post_buf_len;           /**< Post data length. */
    int     response_buf_len;       /**< Response buffer length. */
    char   *post_content_type;      /**< Content type of the post data. */
    char   *post_buf;               /**< User data to be posted. */
    char   *response_buf;           /**< Buffer to store the response data. */
} httpclient_data_t;

int iotx_post(httpclient_t *client,
              const char *url,
              int port,
              const char *ca_crt,
              httpclient_data_t *client_data);

int httpclient_recv_response(httpclient_t *client, uint32_t timeout_ms, httpclient_data_t *client_data);

int httpclient_common(httpclient_t *client, const char *url, int port, const char *ca_crt,
                      HTTPCLIENT_REQUEST_TYPE method, uint32_t timeout_ms, httpclient_data_t *client_data);

void httpclient_close(httpclient_t *client);

#ifdef __cplusplus
}
#endif

#endif /* __HTTPCLIENT_H__ */



