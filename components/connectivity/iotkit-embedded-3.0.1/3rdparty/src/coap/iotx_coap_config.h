#ifndef __IOTX_COAP_CONFIG__
#define __IOTX_COAP_CONFIG__

#define COAP_MSG_MAX_TOKEN_LEN    8
#define COAP_MSG_MAX_OPTION_NUM   12
#define COAP_MSG_MAX_PATH_LEN     128
#ifndef COAP_LARGE_MEMORY_SUPPORT
#define COAP_MSG_MAX_PDU_LEN      1280
#else
#define COAP_MSG_MAX_PDU_LEN      4096
#endif

#ifndef CONFIG_COAP_AUTH_TIMEOUT
    #define CONFIG_COAP_AUTH_TIMEOUT        (3 * 1000)
#endif

#endif
