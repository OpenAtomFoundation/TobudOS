#ifndef _DM_SERVER_ADAPTER_H_
#define _DM_SERVER_ADAPTER_H_

#ifdef ALCS_ENABLED

#define DM_SERVER_ALCS_ADDR          "224.0.1.187"
#define DM_SERVER_ALCS_PORT          (5863)
#define DM_SERVER_ALCS_SEND_MAXCOUNT (16)
#define DM_SERVER_ALCS_WAITTIME      (200)
#define DM_SERVER_ALCS_OBS_MAXCOUNT  (16)
#define DM_SERVER_ALCS_RES_MAXCOUNT  (255)

typedef struct {
    void *conn_handle;
} dm_server_ctx_t;

typedef struct {
    char *ip;
    uint16_t port;
    char *token;
    int token_len;
} dm_server_alcs_context_t;

int dm_server_open(void);
int dm_server_connect(void);
int dm_server_close(void);
int dm_server_send(char *uri, unsigned char *payload, int payload_len, void *context);
int dm_server_subscribe(char *uri, CoAPRecvMsgHandler callback, int auth_type);
int dm_server_add_device(char product_key[IOTX_PRODUCT_KEY_LEN + 1], char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int dm_server_del_device(char product_key[IOTX_PRODUCT_KEY_LEN + 1], char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int dm_server_yield(void);

#endif
#endif