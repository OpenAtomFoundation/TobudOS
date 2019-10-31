#ifndef _INFRA_DEFS_H_
#define _INFRA_DEFS_H_

#include "infra_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IOTX_SDK_VERSION                "3.0.1"
#define IOTX_ALINK_VERSION              "20"
#define IOTX_FIRMWARE_VERSION_LEN       (32)
#define IOTX_PRODUCT_KEY_LEN            (20)
#define IOTX_DEVICE_NAME_LEN            (32)
#define IOTX_DEVICE_SECRET_LEN          (64)
#define IOTX_DEVICE_ID_LEN              (64)
#define IOTX_PRODUCT_SECRET_LEN         (64)
#define IOTX_PARTNER_ID_LEN             (64)
#define IOTX_MODULE_ID_LEN              (64)
#define IOTX_NETWORK_IF_LEN             (160)
#define IOTX_FIRMWARE_VER_LEN           (32)
#define IOTX_URI_MAX_LEN                (135)
#define IOTX_DOMAIN_MAX_LEN             (64)
#define IOTX_CUSTOMIZE_INFO_LEN         (80)

#define DEV_SIGN_SOURCE_MAXLEN          (200)
#define DEV_SIGN_HOSTNAME_MAXLEN        (64)
#define DEV_SIGN_CLIENT_ID_MAXLEN       (200)
#define DEV_SIGN_USERNAME_MAXLEN        (64)
#define DEV_SIGN_PASSWORD_MAXLEN        (65)

#ifndef _IN_
    #define _IN_
#endif

#ifndef _OU_
    #define _OU_
#endif

#ifndef _IN_OPT_
    #define _IN_OPT_
#endif

#define NETWORK_ADDR_LEN        (16)
#define HAL_MAC_LEN             (17 + 1)    /* MAC地址的长度 */
#define STR_SHORT_LEN                   (32)
#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif
#define HAL_MAX_SSID_LEN                (32 + 1)    /* ssid: 32 octets at most, include the NULL-terminated */
#define HAL_MAX_PASSWD_LEN              (64 + 1)    /* password: 8-63 ascii */
#define WLAN_CONNECTION_TIMEOUT_MS      (30 * 1000)

typedef enum IOT_RETURN_CODES {
    ERROR_DEVICE_NOT_EXIST = -311,
    ERROR_NET_TIMEOUT = -310,
    ERROR_CERT_VERIFY_FAIL  = -309,
    ERROR_NET_SETOPT_TIMEOUT = -308,
    ERROR_NET_SOCKET = -307,
    ERROR_NET_CONNECT = -306,
    ERROR_NET_BIND = -305,
    ERROR_NET_LISTEN = -304,
    ERROR_NET_RECV = -303,
    ERROR_NET_SEND = -302,
    ERROR_NET_CONN = -301,
    ERROR_NET_UNKNOWN_HOST = -300,

    MQTT_SUBHANDLE_LIST_LEN_TOO_SHORT = -47,
    MQTT_OFFLINE_LIST_LEN_TOO_SHORT = -46,
    MQTT_TOPIC_LEN_TOO_SHORT = -45,
    MQTT_CONNECT_BLOCK = -44,
    MQTT_SUB_INFO_NOT_FOUND_ERROR = -43,
    MQTT_PUSH_TO_LIST_ERROR = -42,
    MQTT_TOPIC_FORMAT_ERROR = -41,
    NETWORK_RECONNECT_TIMED_OUT_ERROR = -40,/** Returned when the Network is disconnected and the reconnect attempt has timed out */
    MQTT_CONNACK_UNKNOWN_ERROR = -39,/** Connect request failed with the server returning an unknown error */
    MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = -38,/** Connect request failed with the server returning an unacceptable protocol version error */
    MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR = -37,/** Connect request failed with the server returning an identifier rejected error */
    MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR = -36,/** Connect request failed with the server returning an unavailable error */
    MQTT_CONNACK_BAD_USERDATA_ERROR = -35,/** Connect request failed with the server returning a bad userdata error */
    MQTT_CONNACK_NOT_AUTHORIZED_ERROR = -34,/** Connect request failed with the server failing to authenticate the request */
    MQTT_CONNECT_ERROR = -33,
    MQTT_CREATE_THREAD_ERROR = -32,
    MQTT_PING_PACKET_ERROR = -31,
    MQTT_CONNECT_PACKET_ERROR = -30,
    MQTT_CONNECT_ACK_PACKET_ERROR = -29,
    MQTT_NETWORK_CONNECT_ERROR = -28,
    MQTT_STATE_ERROR = -27,
    MQTT_SUBSCRIBE_PACKET_ERROR = -26,
    MQTT_SUBSCRIBE_ACK_PACKET_ERROR = -25,
    MQTT_SUBSCRIBE_ACK_FAILURE = -24,
    MQTT_SUBSCRIBE_QOS_ERROR = -23,
    MQTT_UNSUBSCRIBE_PACKET_ERROR = -22,
    MQTT_PUBLISH_PACKET_ERROR = -21,
    MQTT_PUBLISH_QOS_ERROR = -20,
    MQTT_PUBLISH_ACK_PACKET_ERROR = -19,
    MQTT_PUBLISH_COMP_PACKET_ERROR = -18,
    MQTT_PUBLISH_REC_PACKET_ERROR = -17,
    MQTT_PUBLISH_REL_PACKET_ERROR = -16,
    MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR = -15,
    MQTT_NETWORK_ERROR = -14,
    MQTT_PUBLISH_ACK_TYPE_ERROR = -13,

    ERROR_SHADOW_NO_METHOD = -2008,
    ERROR_SHADOW_UNDEF_TYPE = -2007,
    ERROR_SHADOW_UPDATE_TIMEOUT = -2006,
    ERROR_SHADOW_UPDATE_NACK = -2005,         /**< Negative ACK  */
    ERROR_SHADOW_NO_ATTRIBUTE = -2004,
    ERROR_SHADOW_ATTR_NO_EXIST = -2003,       /**< NO such attribute */
    ERROR_SHADOW_ATTR_EXIST = -2002,          /**< attribute already exists   */
    ERROR_SHADOW_WAIT_LIST_OVERFLOW = -2001,
    ERROR_SHADOW_INVALID_STATE = -2000,

    ERROR_SUBDEV_NULL_VALUE = -1501, /**< Indicating NULL value*/
    ERROR_SUBDEV_NOT_NULL_VALUE = -1500, /**< Indicating value not NULL*/
    ERROR_SUBDEV_STRING_NULL_VALUE = -1499, /**< Indicating NULL value or empty string */
    ERROR_SUBDEV_INVALID_GATEWAY_HANDLE = -1498, /**< Indicating gateway handle is null or invalid*/
    ERROR_SUBDEV_SESSION_NOT_FOUND = -1497, /**< Cannot find device session*/
    ERROR_SUBDEV_RRPC_CB_NOT_NULL = -1496, /**< RRPC callback function has been set,needn't to set again*/
    ERROR_SUBDEV_REPLY_TYPE_NOT_DEF = -1495, /**< Reply type not defined*/
    ERROR_SUBDEV_GET_JSON_VAL = -1494, /**< Get value from reply payload fail*/
    ERROR_SUBDEV_DATA_LEN_OVERFLOW = -1493, /**< Length of 'data' value from reply palyoad is large than limit(1024)*/
    ERROR_SUBDEV_MSG_LEN = -1492, /**< Indicating msg len is not correct*/
    ERROR_SUBDEV_REPLY_PROC = -1491, /**< Error occur when process publish reply */
    ERROR_SUBDEV_REPLY_TOPIC_NOT_MATCH = -1490, /**< Indicating that topic received is unknown*/
    ERROR_SUBDEV_REPLY_VAL_CHECK = -1489, /**< Indicating that value get from reply checked fail with local*/
    ERROR_SUBDEV_REGISTER_TYPE_NOT_DEF = -1488, /**< Register type not support*/
    ERROR_SUBDEV_PACKET_SPLICE_FAIL = -1487, /**< Splice packet error*/
    ERROR_SUBDEV_MQTT_PUBLISH_FAIL = -1486, /**< MQTT publish fail*/
    ERROR_SUBDEV_REPLY_PARSE_FAIL = -1485, /**< Parse reply fail*/
    ERROR_SUBDEV_CREATE_SESSION_FAIL = -1484, /**< Create session fail*/
    ERROR_SUBDEV_INVALID_CLEAN_SESSION_TYPE = -1483, /**< Clean session not support*/
    ERROR_SUBDEV_HAS_BEEN_LOGIN = -1482, /**< Device has been login*/
    ERROR_SUBDEV_SUB_UNSUB_FAIL = -1481, /**< subscribe or unsubscribe fail*/
    ERROR_SUBDEV_SESSION_STATE_FAIL = -1480, /**< Session state is error,may not login*/
    ERROR_SUBDEV_MEMORY_NOT_ENOUGH = -1479, /**< Set memory too small*/

    ERROR_REPLY_TIMEOUT = -6099, /**< recieve reply timeout*/
    ERROR_DEVICE_NOT_FOUND = -6100, /**< device not found*/
    ERROR_TOO_LARGE_PAGE_SIZE = -6101, /**< page size must less than 200*/
    ERROR_DEVICE_COUNT_FAULT = -6102, /**< device count query service fault*/
    ERROR_DEVICE_DETAIL_FAULT = -6103, /**< device detail query service fault*/
    ERROR_TOO_LARGE_LIST_SIZE = -6104, /**< list size must less than 200*/
    ERROR_LIST_SIZE_CANNOT_BE_ZERO = -6105, /**< list size must greater than 0*/
    ERROR_TOO_LARGE_MAP_SIZE = -6106, /**< map size must less than 200*/
    ERROR_MAP_SIZE_CANNOT_BE_ZERO = -6107, /**< map size must greater than 0*/
    ERROR_DEVICE_STATUS_FAULT = -6108, /**< device status query service fault*/
    ERROR_DEVICE_INFO_FAULT = -6109, /**< device info query service fault*/
    ERROR_SET_THING_PROPERTIES_ERROR = -6150, /**< set thing properties error*/
    ERROR_INVOKE_THING_SERVICE_ERROR = -6151, /**< invoke thing service error*/
    ERROR_SCRIPT_REL_NOT_EXIST = -6200, /**< script relation not exist*/
    ERROR_SCRIPT_CONVERT_DATA_IS_NULL = -6201, /**< script convert data is null*/
    ERROR_DEVICE_PRODUCT_NOT_EXIST = -6202, /**< product not exist*/
    ERROR_TOPIC_NOT_EXIST = -6203, /**< topic not exist*/
    ERROR_DEVICE_IS_DISABLED = -6204, /**< device is disabled*/
    ERROR_IOT_MESSAGE_ERROR = -6205, /**< iot message service error*/
    ERROR_PRODUCT_PROPERTY_NOT_EXIST = -6206, /**< product property not exist*/
    ERROR_DATA_FORMAT_ERROR = -6207, /**< device data format is error*/
    ERROR_THING_STATUS_PROHIBITED = -6208, /**< thing status is prohibited*/
    ERROR_THING_STATUS_NOT_ACTIVE = -6209, /**< thing status not active*/
    /**
     *
     * -6250 ~ -6299
     */
    ERROR_PRODUCT_NOT_FOUND = -6250, /**< product not found*/
    ERROR_DEVICE_EXISTS = -6251, /**< device has existed*/
    ERROR_JUDGE_DEVICE_EXISTS_ERROR = -6252, /**< judge device exists error*/
    ERROR_ADD_DEVICE_FAILED = -6253, /**< add device failed*/
    ERROR_UPDATE_DEVICE_FAILED = -6254, /**< update device failed*/
    ERROR_INSERT_DGR_FAILED = -6255, /**< insert device group relation failed*/
    ERROR_SYN_DEVICE_FAILED = -6256, /**< device synchronization failed*/
    ERROR_PRODUCT_DOMAIN_ILLEGAL = -6257, /**< product domain illegal*/
    ERROR_TENANID_ILLEGAL = -6258, /**< tenantId illegal*/
    ERROR_PRODUCT_REGION_ILLEGAL = -6259, /**< product region illegal*/
    ERROR_PRODUCT_NETTYPE_ILLEGAL = -6260, /**< product nettype illegal*/
    ERROR_INSERT_DEVICE_APPLY_DETAIL_FAILED = -6261, /**< insert device apply detail failed*/
    ERROR_UPDATE_DEVICE_APPLY_STATUS_FAILED = -6262, /**< update device apply status failed*/
    ERROR_DELERE_DGR_FAILED = -6263, /**< delete device group relation status*/
    ERROR_DELETE_DEVICE_FAILED = -6264, /**< delete device failed*/
    ERROR_QUERY_DEVICE_DETAIL_FAILED = -6265, /**< query device detail failed*/
    ERROR_QUERY_DEVICE_COUNT_FAILED = -6266, /**< query device count failed*/
    ERROR_QUERY_ACTIVE_DEVICE_COUNT_FAILED = -6267, /**< query active device count failed*/
    ERROR_INSERT_AGR_FAILED = -6268, /**< insert apply group relation failed*/
    ERROR_QUERY_DEVICE_APPLY_FAILED = -6269, /**< query device apply failed*/
    ERROR_QUERY_PRODUCT_FAILED = -6270, /**< query product failed*/
    ERROR_DEVICE_APPLY_NOT_FOUND = -6271, /**< device apply not found*/
    ERROR_RELEASE_TRIAD_FAILED = -6272, /**< release triad failed*/
    ERROR_UPDATE_DAD_STATUS_FAILED = -6273, /**< update device apply detail status failed*/
    ERROR_REG_LORA_DEVICE_FAILED = -6274, /**< register lora device failed*/
    ERROR_SYN_APPLY_DEVICE_FAILED = -6275, /**< device apply synchronization failed*/
    ERROR_QUERY_DGR_FAILED = -6276, /**< query device group relation failed*/
    ERROR_JUDGE_DGR_FAILED = -6277, /**< judge device group relation failed*/
    ERROR_QUERY_AGR_FAILED = -6278, /**< query apply group relation failed*/
    ERROR_JUDGE_AGR_FAILED = -6279, /**< judge apply group relation failed*/
    ERROR_DEVICENAME_NOT_MEET_SPECS = -6280, /**< devicename not meet specs*/
    ERROR_DELETE_APPLY_DEVICE_FAILED = -6281, /**< delete apply device failed*/
    ERROR_GEN_DEVICEID_FAILED = -6282, /**< gennerate deviceId failed*/
    ERROR_APPLY_ILLEGAL = -6283, /**< apply illegal*/
    ERROR_LORA_DEVICE_METHOD_ERROR = -6284, /**< lora device cannot created by num*/
    ERROR_APPLY_NOT_READY = -6285, /**< apply not ready*/


    /**
     * dsl
     * -6300 ~ -6349
     */
    ERROR_DSL_PARSE_METHOD_NOT_EXIST = -6300, /**< dsl parse: method not exist*/
    ERROR_DSL_PARSE_PARAMS_FORMAT_ERROR = -6301, /**< dsl parse: params format must be JSONObject/JSONArray*/
    ERROR_DSL_PARSE_PARAMS_VALUE_EMPTY = -6302, /**< dsl parse: params value empty*/
    ERROR_DSL_PARSE_PARAMS_NUMBER_ERROR = -6303, /**< dsl parse: params number error*/
    ERROR_DSL_PARSE_PARAMS_NOT_EXIST = -6304, /**< dsl parse: params not exist*/
    ERROR_DSL_PARSE_PARAMS_TYPE_ERROR = -6305, /**< dsl parse: params type error*/
    ERROR_DSL_PARSE_INT_SPECS_ERROR = -6306, /**< dsl parse: int specs error*/
    ERROR_DSL_PARSE_FLOAT_SPECS_ERROR = -6307, /**< dsl parse: float specs error*/
    ERROR_DSL_PARSE_BOOL_SPECS_ERROR = -6308, /**< dsl parse: bool specs error*/
    ERROR_DSL_PARSE_ENUM_SPECS_ERROR = -6309, /**< dsl parse: enum specs error*/
    ERROR_DSL_PARSE_STRING_SPECS_ERROR = -6310, /**< dsl parse: string specs error*/
    ERROR_DSL_PARSE_DATE_SPECS_ERROR = -6311, /**< dsl parse: date specs error*/
    ERROR_DSL_PARSE_STRUCT_SPECS_ERROR = -6312, /**< dsl parse: struct specs error*/
    ERROR_DSL_SERVICE_NOT_AVAILABLE = -6313, /**< dsl service not available*/
    ERROR_DSL_PARSE_DATA_TYPE_PARSE_ERROR = -6314, /**< dsl parse: data type parse error*/
    ERROR_DATA_NOT_SATISFY_DSL = -6315, /**< dsl parse: data not satisfy dsl*/
    ERROR_DSL_PARSE_SPECS_NUMBER_FORMAT_ERROR = -6316, /**< dsl parse: specs number format error*/
    ERROR_DSL_PARSE_TEMPLATE_ERROR = -6317, /**< dsl parse: template error*/
    ERROR_DSL_EXCEPTION = -6318, /**< dsl exception*/
    ERROR_DSL_PARSE_EVENT_CALL_TYPE_ERROR = -6319, /**< dsl parse: event call type error*/
    ERROR_DSL_PARSE_NO_PROPERTY = -6320, /**< dsl parse: no property exist in product*/
    ERROR_DSL_PARSE_IDENTIFIER_IS_NULL = -6321, /**< dsl parse: template property/params idetifier is null*/
    ERROR_DSL_DEVICE_NOT_EXIST_IN_PRODUCT = -6321, /**< dsl: device not exist in product*/
    ERROR_DSL_PARSE_DOUBLE_SPECS_ERROR = -6322, /**< dsl parse: double specs error*/

    /**
     *
     * -6350 ~ -6399
     */
    ERROR_EVENT_PUT_ERROR = -6350, /**< thing event put error*/
    ERROR_SERVICE_PUT_ERROR = -6351, /**< thing service put error*/
    ERROR_DEVICE_GET_EVENT_FAULT = -6352, /**< thing event get error*/
    ERROR_PRODUCT_KEY_ELEMENT_ALREADY_EXIST = -6353, /**< product key element already exist*/

    /**
     *
     * -6400 ~ -6449
     */
    ERROR_TOPO_RELATION_COUNT_EXCEED = -6400, /**< topo relation count exceed*/
    ERROR_TOPO_RELATION_NOT_EXIST = -6401, /**< topo relation not exist*/
    ERROR_TOPO_RELATION_CANNOT_ADD_BYSELF = -6402, /**< topo relation cannot add by self*/

    /**
     * alink
     * -6450 ~ -6469
     */
    ERROR_ALINK_METHOD_NOT_EXIST = -6450, /**< alink method not exist*/

    /**
     *
     * -6550 ~ -6599
     */
    ERROR_DEVICE_GROUP_NOT_FOUND = -6550, /**< device group not found*/

    /**
     * @brief dev_sign
     *
     * -1100 ~ -1200
     *
     */
    ERROR_DEV_SIGN_CUSTOM_DOMAIN_IS_NULL = -1105,
    ERROR_DEV_SIGN_SOURCE_TOO_SHORT = -1104,
    ERROR_DEV_SIGN_PASSWORD_TOO_SHORT = -1103,
    ERROR_DEV_SIGN_USERNAME_TOO_SHORT = -1102,
    ERROR_DEV_SIGN_CLIENT_ID_TOO_SHORT = -1101,
    ERROR_DEV_SIGN_HOST_NAME_TOO_SHORT = -1100,

    ERROR_NO_MEM = -1016,
    ERROR_CERTIFICATE_EXPIRED = -1015,
    ERROR_MALLOC = -1014,
    ERROR_NO_ENOUGH_MEM = -1013,               /**< Writes more than size value. */

    ERROR_NO_SUPPORT = -12,
    ERROR_NO_PERSISTENCE = -11,
    ERROR_HTTP_BREAK = -10,
    ERROR_NULL_VALUE = -9,
    ERROR_HTTP_CONN = -8,                    /**< Connection failed. */
    ERROR_HTTP_PARSE = -7,                   /**< A URL parse error occurred. */
    ERROR_HTTP_UNRESOLVED_DNS = -6,           /**< Could not resolve the hostname. */
    ERROR_HTTP_PRTCL = -5,                   /**< A protocol error occurred. */
    ERROR_HTTP = -4,                         /**< An unknown error occurred. */
    ERROR_HTTP_CLOSED = -3,                  /**< Connection was closed by a remote host. */
    NULL_VALUE_ERROR = -2,

    FAIL_RETURN = -1,                        /**< generic error. */
    SUCCESS_RETURN = 0,


    /* @value > 0, reserved for other usage */

} iotx_err_t;

typedef struct _iotx_dev_meta_info {
    char product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char product_secret[IOTX_PRODUCT_SECRET_LEN + 1];
    char device_name[IOTX_DEVICE_NAME_LEN + 1];
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1];
} iotx_dev_meta_info_t;

typedef struct {
    const char *region;
    uint16_t port;
} iotx_region_item_t;

typedef enum {
    IOTX_CLOUD_REGION_SHANGHAI,   /* Shanghai */
    IOTX_CLOUD_REGION_SINGAPORE,  /* Singapore */
    IOTX_CLOUD_REGION_JAPAN,      /* Japan */
    IOTX_CLOUD_REGION_USA_WEST,   /* America */
    IOTX_CLOUD_REGION_GERMANY,    /* Germany */
    IOTX_CLOUD_REGION_CUSTOM,     /* Custom setting */
    IOTX_CLOUD_DOMAIN_MAX         /* Maximum number of domain */
} iotx_mqtt_region_types_t;

#define IOTX_MQTT_DOMAIN_NUMBER (6)
extern const char *g_infra_mqtt_domain[IOTX_MQTT_DOMAIN_NUMBER];

typedef enum {
    IOTX_HTTP_REGION_SHANGHAI,   /* Shanghai */
    IOTX_HTTP_REGION_SINGAPORE,  /* Singapore */
    IOTX_HTTP_REGION_JAPAN,      /* Japan */
    IOTX_HTTP_REGION_AMERICA,    /* America */
    IOTX_HTTP_REGION_GERMANY,    /* Germany */
    IOTX_HTTP_REGION_CUSTOM,     /* Custom setting */
    IOTX_HTTP_REGION_MAX         /* Maximum number of domain */
} iotx_http_region_types_t;

/**
 * @brief contains iot_ioctl context
 *
 */
typedef struct {
    int domain_type;
    int dynamic_register;
    char cloud_custom_domain[IOTX_DOMAIN_MAX_LEN + 1];
    uint16_t mqtt_port_num;
    char http_custom_domain[IOTX_DOMAIN_MAX_LEN + 1];
    char mqtt_customzie_info[IOTX_CUSTOMIZE_INFO_LEN + 1];
} sdk_impl_ctx_t;

typedef struct {
    char hostname[DEV_SIGN_HOSTNAME_MAXLEN];
    uint16_t port;
    char clientid[DEV_SIGN_CLIENT_ID_MAXLEN];
    char username[DEV_SIGN_USERNAME_MAXLEN];
    char password[DEV_SIGN_PASSWORD_MAXLEN];
} iotx_sign_mqtt_t;

#define IOTX_HTTP_DOMAIN_NUMBER (6)
extern const char *g_infra_http_domain[IOTX_HTTP_DOMAIN_NUMBER];

extern int iotx_facility_json_print(const char *str, int level, ...);
#ifdef __cplusplus
}
#endif
#endif

