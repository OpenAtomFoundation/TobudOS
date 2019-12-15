#ifndef _INFRA_COMPAT_H_
#define _INFRA_COMPAT_H_

#include "infra_defs.h"
#include "infra_list.h"

#undef  being_deprecated
#define being_deprecated

typedef enum _IOT_LogLevel {
    IOT_LOG_NONE = 0,
    IOT_LOG_CRIT,
    IOT_LOG_ERROR,
    IOT_LOG_WARNING,
    IOT_LOG_INFO,
    IOT_LOG_DEBUG,
} IOT_LogLevel;

void IOT_SetLogLevel(IOT_LogLevel level);
void IOT_DumpMemoryStats(IOT_LogLevel level);

/**
 * @brief event list used for iotx_regist_event_monitor_cb
 */
enum iotx_event_t {
    IOTX_AWSS_START = 0x1000,       /* AWSS start without enbale, just supports device discover */
    IOTX_AWSS_ENABLE,               /* AWSS enable */
    IOTX_AWSS_LOCK_CHAN,            /* AWSS lock channel(Got AWSS sync packet) */
    IOTX_AWSS_CS_ERR,               /* AWSS AWSS checksum is error */
    IOTX_AWSS_PASSWD_ERR,           /* AWSS decrypt passwd error */
    IOTX_AWSS_GOT_SSID_PASSWD,      /* AWSS parse ssid and passwd successfully */
    IOTX_AWSS_CONNECT_ADHA,         /* AWSS try to connnect adha (device discover, router solution) */
    IOTX_AWSS_CONNECT_ADHA_FAIL,    /* AWSS fails to connect adha */
    IOTX_AWSS_CONNECT_AHA,          /* AWSS try to connect aha (AP solution) */
    IOTX_AWSS_CONNECT_AHA_FAIL,     /* AWSS fails to connect aha */
    IOTX_AWSS_SETUP_NOTIFY,         /* AWSS sends out device setup information (AP and router solution) */
    IOTX_AWSS_CONNECT_ROUTER,       /* AWSS try to connect destination router */
    IOTX_AWSS_CONNECT_ROUTER_FAIL,  /* AWSS fails to connect destination router. */
    IOTX_AWSS_GOT_IP,               /* AWSS connects destination successfully and got ip address */
    IOTX_AWSS_SUC_NOTIFY,           /* AWSS sends out success notify (AWSS sucess) */
    IOTX_AWSS_BIND_NOTIFY,          /* AWSS sends out bind notify information to support bind between user and device */
    IOTX_AWSS_ENABLE_TIMEOUT,       /* AWSS enable timeout(user needs to call awss_config_press again to enable awss) */
    IOTX_CONN_CLOUD = 0x2000,       /* Device try to connect cloud */
    IOTX_CONN_CLOUD_FAIL,           /* Device fails to connect cloud, refer to net_sockets.h for error code */
    IOTX_CONN_CLOUD_SUC,            /* Device connects cloud successfully */
    IOTX_RESET = 0x3000,            /* Linkkit reset success (just got reset response from cloud without any other operation) */
};

/**
 * @brief register callback to monitor all event from system.
 *
 * @param callback, when some event occurs, the system will trigger callback to user.
 *                  refer to enum iotx_event_t for event list supported.
 *
 * @return 0 when success, -1 when fail.
 * @note: user should make sure that callback is not block and runs to complete fast.
 */
int iotx_event_regist_cb(void (*monitor_cb)(int event));

/**
 * @brief post event to trigger callback resitered by iotx_event_regist_cb
 *
 * @param event, event id, refer to iotx_event_t
 *
 * @return 0 when success, -1 when fail.
 */
int iotx_event_post(int event);

#ifndef BUILD_AOS

#ifndef VERSION_NUM_SIZE
    #define VERSION_NUM_SIZE    4
#endif

#ifndef RANDOM_NUM_SIZE
    #define RANDOM_NUM_SIZE     4
#endif

#ifndef MAC_ADDRESS_SIZE
    #define MAC_ADDRESS_SIZE    8
#endif

#ifndef CHIP_CODE_SIZE
    #define CHIP_CODE_SIZE      4
#endif

#define AOS_ACTIVE_INFO_LEN (81)
unsigned int aos_get_version_info(unsigned char version_num[VERSION_NUM_SIZE],
                                  unsigned char random_num[RANDOM_NUM_SIZE],
                                  unsigned char mac_address[MAC_ADDRESS_SIZE],
                                  unsigned char chip_code[CHIP_CODE_SIZE],
                                  unsigned char *output_buffer,
                                  unsigned int output_buffer_size);
#endif

typedef enum {
    ITE_AWSS_STATUS,
    ITE_CONNECT_SUCC,
    ITE_CONNECT_FAIL,
    ITE_DISCONNECTED,
    ITE_RAWDATA_ARRIVED,
    ITE_SERVICE_REQUEST,
    ITE_PROPERTY_SET,
    ITE_PROPERTY_GET,
#ifdef DEVICE_MODEL_SHADOW
    ITE_PROPERTY_DESIRED_GET_REPLY,
#endif
    ITE_REPORT_REPLY,
    ITE_TRIGGER_EVENT_REPLY,
    ITE_TIMESTAMP_REPLY,
    ITE_TOPOLIST_REPLY,
    ITE_PERMIT_JOIN,
    ITE_INITIALIZE_COMPLETED,
    ITE_FOTA,
    ITE_COTA,
    ITE_MQTT_CONNECT_SUCC
} iotx_ioctl_event_t;

#define IOT_RegisterCallback(evt, cb)           iotx_register_for_##evt(cb);
#define DECLARE_EVENT_CALLBACK(evt, cb)         int iotx_register_for_##evt(cb);
#define DEFINE_EVENT_CALLBACK(evt, cb)          int iotx_register_for_##evt(cb) { \
        if (evt < 0 || evt >= sizeof(g_impl_event_map)/sizeof(impl_event_map_t)) {return -1;} \
        g_impl_event_map[evt].callback = (void *)callback;return 0;}

DECLARE_EVENT_CALLBACK(ITE_AWSS_STATUS,          int (*cb)(int))
DECLARE_EVENT_CALLBACK(ITE_CONNECT_SUCC,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_CONNECT_FAIL,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_DISCONNECTED,         int (*cb)(void))
DECLARE_EVENT_CALLBACK(ITE_RAWDATA_ARRIVED,      int (*cb)(const int, const unsigned char *, const int))
DECLARE_EVENT_CALLBACK(ITE_SERVICE_REQUEST,       int (*cb)(const int, const char *, const int, const char *, const int,
                       char **, int *))
DECLARE_EVENT_CALLBACK(ITE_PROPERTY_SET,         int (*cb)(const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_PROPERTY_DESIRED_GET_REPLY,         int (*cb)(const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_PROPERTY_GET,         int (*cb)(const int, const char *, const int, char **, int *))
DECLARE_EVENT_CALLBACK(ITE_REPORT_REPLY,         int (*cb)(const int, const int, const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_TRIGGER_EVENT_REPLY,  int (*cb)(const int, const int, const int, const char *, const int,
                       const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_TIMESTAMP_REPLY,      int (*cb)(const char *))
DECLARE_EVENT_CALLBACK(ITE_TOPOLIST_REPLY,       int (*cb)(const int, const int, const int, const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_PERMIT_JOIN,          int (*cb)(const char *, const int))
DECLARE_EVENT_CALLBACK(ITE_INITIALIZE_COMPLETED, int (*cb)(const int))
DECLARE_EVENT_CALLBACK(ITE_FOTA,                 int (*cb)(const int, const char *))
DECLARE_EVENT_CALLBACK(ITE_COTA,                 int (*cb)(const int, const char *, int, const char *, const char *,
                       const char *, const char *))
DECLARE_EVENT_CALLBACK(ITE_MQTT_CONNECT_SUCC,    int (*cb)(void))

void *iotx_event_callback(int evt);

#ifndef offset_of
    #define offset_of aos_offsetof
#endif
#ifndef container_of
    #define container_of aos_container_of
#endif

#define LIST_HEAD                       AOS_DLIST_HEAD
#define LIST_HEAD_INIT                  AOS_DLIST_INIT
#define INIT_LIST_HEAD                  INIT_AOS_DLIST_HEAD
#define LIST_INIT                       AOS_DLIST_INIT

#define list_head                       dlist_s
#define list_head_t                     dlist_t

#define list_add                        dlist_add
#define list_add_tail                   dlist_add_tail
#define list_del                        dlist_del
#define list_empty                      dlist_empty
#define list_entry_number               dlist_entry_number
#define list_first_entry                dlist_first_entry
#define list_for_each                   dlist_for_each
#define list_for_each_entry_reverse     dlist_for_each_entry_reverse
#define list_for_each_safe              dlist_for_each_safe
#define list_init                       dlist_init

#define list_for_each_entry(pos, head, member, type)             \
    dlist_for_each_entry(head, pos, type, member)

#define list_for_each_entry_safe(pos, n, head, member, type)         \
    for (pos = list_entry((head)->next, type, member), \
         n = list_entry(pos->member.next, type, member);    \
         &pos->member != (head);                    \
         pos = n, n = list_entry(n->member.next, type, member))

#define list_next_entry(pos, member, type) \
    list_entry((pos)->member.next, type, member)

static inline void list_del_init(struct list_head *entry)
{
    list_del(entry);
    INIT_LIST_HEAD(entry);
}

static inline int list_is_last(const struct list_head *list,
                               const struct list_head *head)
{
    return list->next == head;
}

typedef struct {
    uint16_t        port;
    uint8_t         init;
    char            *host_name;
    char            *client_id;
    char            *username;
    char            *password;
    const char      *pub_key;
} iotx_conn_info_t, *iotx_conn_info_pt;

int IOT_SetupConnInfo(const char *product_key,
                      const char *device_name,
                      const char *device_secret,
                      void **info_ptr);


typedef enum {
    IOTX_IOCTL_SET_REGION,              /* value(int*): iotx_cloud_region_types_t */
    IOTX_IOCTL_GET_REGION,              /* value(int*) */
    IOTX_IOCTL_SET_MQTT_DOMAIN,         /* value(const char*): point to mqtt domain string */
    IOTX_IOCTL_SET_HTTP_DOMAIN,         /* value(const char*): point to http domain string */
    IOTX_IOCTL_SET_DYNAMIC_REGISTER,    /* value(int*): 0 - Disable Dynamic Register, 1 - Enable Dynamic Register */
    IOTX_IOCTL_GET_DYNAMIC_REGISTER,    /* value(int*) */
    IOTX_IOCTL_RECV_PROP_REPLY,         /* value(int*): 0 - Disable property post reply by cloud; 1 - Enable property post reply by cloud */
    IOTX_IOCTL_RECV_EVENT_REPLY,        /* value(int*): 0 - Disable event post reply by cloud; 1 - Enable event post reply by cloud */
    IOTX_IOCTL_SEND_PROP_SET_REPLY,     /* value(int*): 0 - Disable send post set reply by devid; 1 - Enable property set reply by devid */
    IOTX_IOCTL_SET_SUBDEV_SIGN,         /* value(const char*): only for slave device, set signature of subdevice */
    IOTX_IOCTL_GET_SUBDEV_LOGIN,        /* value(int*): 0 - SubDev is logout; 1 - SubDev is login */
    IOTX_IOCTL_SET_OTA_DEV_ID,          /* value(int*):     select the device to do OTA according to devid */
    IOTX_IOCTL_QUERY_DEVID,             /* value(iotx_dev_meta_info_t*): device meta info, only productKey and deviceName is required, ret value is subdev_id or -1 */
    IOTX_IOCTL_SET_CUSTOMIZE_INFO,      /* value(char*): set mqtt clientID customize information */
} iotx_ioctl_option_t;

typedef enum {
    IMPL_LINKKIT_IOCTL_SWITCH_PROPERTY_POST_REPLY,           /* only for master device, choose whether you need receive property post reply message */
    IMPL_LINKKIT_IOCTL_SWITCH_EVENT_POST_REPLY,              /* only for master device, choose whether you need receive event post reply message */
    IMPL_LINKKIT_IOCTL_SWITCH_PROPERTY_SET_REPLY,            /* only for master device, choose whether you need send property set reply message */
    IMPL_LINKKIT_IOCTL_MAX
} impl_linkkit_ioctl_cmd_t;

/**
 * @brief Setup Demain type, should be called before MQTT connection.
 *
 * @param [in] option: see iotx_ioctl_option_t.
 *
 * @return None.
 * @see None.
 */
int IOT_Ioctl(int option, void *data);

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
#endif

/* compatible for V2.3.0 */
#define IOTX_CLOUD_DOMAIN_SH        IOTX_CLOUD_REGION_SHANGHAI
#define IOTX_CLOUD_DOMAIN_SG        IOTX_CLOUD_REGION_SINGAPORE
#define IOTX_CLOUD_DOMAIN_JP        IOTX_CLOUD_REGION_JAPAN
#define IOTX_CLOUD_DOMAIN_US        IOTX_CLOUD_REGION_USA_WEST
#define IOTX_CLOUD_DOMAIN_GER       IOTX_CLOUD_REGION_GERMANY
#define IOTX_IOCTL_SET_DOMAIN       IOTX_IOCTL_SET_REGION
#define IOTX_IOCTL_GET_DOMAIN       IOTX_IOCTL_GET_REGION

#define IOT_OpenLog(arg)
#define IOT_CloseLog()              IOT_SetLogLevel(IOT_LOG_NONE)
#define IOT_LOG_EMERG               IOT_LOG_NONE

#define IOT_Linkkit_Post            IOT_Linkkit_Report
/* compatible for V2.3.0 end */

typedef enum {
    HAL_AES_ENCRYPTION = 0,
    HAL_AES_DECRYPTION = 1,
} AES_DIR_t;

typedef void *p_HAL_Aes128_t;

#define NETWORK_ADDR_LEN        (16)

typedef struct _network_addr_t {
    unsigned char
    addr[NETWORK_ADDR_LEN];
    unsigned short  port;
} NetworkAddr;

#endif  /* _INFRA_COMPAT_H_ */
