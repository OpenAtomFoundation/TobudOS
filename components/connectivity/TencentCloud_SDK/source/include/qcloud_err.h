#ifndef _QCLOUD_ERR_H_
#define _QCLOUD_ERR_H_

typedef enum qcloud_err_en {
    QCLOUD_ERR_MQTT_ALREADY_CONNECTED                        = 4,       // 表示与MQTT服务器已经建立连接
    QCLOUD_ERR_MQTT_CONNACK_CONNECTION_ACCEPTED              = 3,       // 表示服务器接受客户端MQTT连接
    QCLOUD_ERR_MQTT_MANUALLY_DISCONNECTED                    = 2,       // 表示与MQTT服务器已经手动断开
    QCLOUD_ERR_MQTT_RECONNECTED                              = 1,       // 表示与MQTT服务器重连成功

    QCLOUD_ERR_HTTP_CLOSED                                   = -3,      // 远程主机关闭连接
    QCLOUD_ERR_HTTP                                          = -4,      // HTTP未知错误
    QCLOUD_ERR_HTTP_PRTCL                                    = -5,      // 协议错误
    QCLOUD_ERR_HTTP_UNRESOLVED_DNS                           = -6,      // 域名解析失败
    QCLOUD_ERR_HTTP_PARSE                                    = -7,      // URL解析失败
    QCLOUD_ERR_HTTP_CONN                                     = -8,      // HTTP连接失败
    QCLOUD_ERR_HTTP_AUTH                                     = -9,      // HTTP鉴权问题
    QCLOUD_ERR_HTTP_NOT_FOUND                                = -10,     // HTTP 404
    QCLOUD_ERR_HTTP_TIMEOUT                                  = -11,     // HTTP 超时

    QCLOUD_ERR_SUCCESS                                       = 0,       // 表示成功返回
    QCLOUD_ERR_FAILURE                                       = -1001,   // 表示失败返回
    QCLOUD_ERR_INVAL                                         = -1002,   // 表示参数无效错误
    QCLOUD_ERR_DEV_INFO                                      = -1003,   // 获取设备信息失败


    QCLOUD_ERR_MQTT_PUSH_TO_LIST_FAILED                      = -102,    // 表示往等待 ACK 列表中添加元素失败
    QCLOUD_ERR_MQTT_NO_CONN                                  = -103,    // 表示未与MQTT服务器建立连接或已经断开连接
    QCLOUD_ERR_MQTT_UNKNOWN                                  = -104,    // 表示MQTT相关的未知错误
    QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT                     = -105,    // 表示正在与MQTT服务重新建立连接
    QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT                        = -106,    // 表示重连已经超时
    QCLOUD_ERR_MQTT_MAX_SUBSCRIPTIONS                        = -107,    // 表示超过可订阅的主题数
    QCLOUD_ERR_MQTT_SUB                                      = -108,    // 表示订阅主题失败, 即服务器拒绝
    QCLOUD_ERR_MQTT_NOTHING_TO_READ                          = -109,    // 表示无MQTT相关报文可以读取
    QCLOUD_ERR_MQTT_PACKET_READ                              = -110,    // 表示读取的MQTT报文有问题
    QCLOUD_ERR_MQTT_REQUEST_TIMEOUT                          = -111,    // 表示MQTT相关操作请求超时
    QCLOUD_ERR_MQTT_CONNACK_UNKNOWN                          = -112,    // 表示客户端MQTT连接未知错误
    QCLOUD_ERR_MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION    = -113,    // 表示客户端MQTT版本错误
    QCLOUD_ERR_MQTT_CONNACK_IDENTIFIER_REJECTED              = -114,    // 表示客户端标识符错误
    QCLOUD_ERR_MQTT_CONNACK_SERVER_UNAVAILABLE               = -115,    // 表示服务器不可用
    QCLOUD_ERR_MQTT_CONNACK_BAD_USERDATA                     = -116,    // 表示客户端连接参数中的username或password错误
    QCLOUD_ERR_MQTT_CONNACK_NOT_AUTHORIZED                   = -117,    // 表示客户端连接认证失败
    QCLOUD_ERR_RX_MESSAGE_INVAL                              = -118,    // 表示收到的消息无效
    QCLOUD_ERR_BUF_TOO_SHORT                                 = -119,    // 表示消息接收缓冲区的长度小于消息的长度
    QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT                          = -120,    // 表示该QOS级别不支持
    QCLOUD_ERR_MQTT_UNSUB_FAIL                               = -121,    // 表示取消订阅主题失败,比如该主题不存在

    QCLOUD_ERR_JSON_PARSE                                    = -132,    // 表示JSON解析错误
    QCLOUD_ERR_JSON_BUFFER_TRUNCATED                         = -133,    // 表示JSON文档会被截断
    QCLOUD_ERR_JSON_BUFFER_TOO_SHORT                         = -134,    // 表示存储JSON文档的缓冲区太小
    QCLOUD_ERR_JSON                                          = -135,    // 表示JSON文档生成错误
    QCLOUD_ERR_MAX_JSON_TOKEN                                = -136,    // 表示超过JSON文档中的最大Token数
    QCLOUD_ERR_MAX_APPENDING_REQUEST                         = -137,    // 表示超过同时最大的文档请求
    QCLOUD_ERR_MAX_TOPIC_LENGTH                              = -138,    // 表示超过规定最大的topic长度

    QCLOUD_ERR_COAP_CONNRESET                                = -150,    // COAP参数错误
    QCLOUD_ERR_COAP_NULL                                     = -151,    // 空指针
    QCLOUD_ERR_COAP_INVALID_LENGTH                           = -152,    // COAP参数长度错误
    QCLOUD_ERR_COAP_DATA_SIZE                                = -153,    // COAP数据大小超出限制
    QCLOUD_ERR_COAP_NOT_FOUND                                = -154,    // COAP查找失败
    QCLOUD_ERR_COAP_NET_INIT_FAILED                          = -155,    // COAP初始化失败
    QCLOUD_ERR_COAP_INTERNAL                                 = -156,    // COAP内部错误
    QCLOUD_ERR_COAP_WRITE_FAILED                             = -157,    // COAP写数据失败
    QCLOUD_ERR_COAP_READ_FAILED                              = -158,    // COAP读数据失败
    QCLOUD_ERR_COAP_BADMSG                                   = -159,    // COAPbad消息
    QCLOUD_ERR_COAP_TIMEOUT                                  = -160,    // COAP超时错误
    QCLOUD_ERR_DTLS_PEER_CLOSE_NOTIFY                        = -161,    // 表示DTLS通道被关闭

    QCLOUD_ERR_SHADOW_PROPERTY_EXIST                         = -201,    // 表示注册的属性已经存在
    QCLOUD_ERR_SHADOW_NOT_PROPERTY_EXIST                     = -202,    // 表示注册的属性不存在
    QCLOUD_ERR_SHADOW_UPDATE_TIMEOUT                         = -203,    // 表示更新设备影子文档超时
    QCLOUD_ERR_SHADOW_UPDATE_REJECTED                        = -204,    // 表示更新设备影子文档被拒绝
    QCLOUD_ERR_SHADOW_GET_TIMEOUT                            = -205,    // 表示拉取设备影子文档超时
    QCLOUD_ERR_SHADOW_GET_REJECTED                           = -206,    // 表示拉取设备影子文档被拒绝

    QCLOUD_ERR_GATEWAY_CREATE_SESSION_FAIL                   = -221,    //创建子设备session失败
    QCLOUD_ERR_GATEWAY_SESSION_NO_EXIST                      = -222,    //子设备session不存在
    QCLOUD_ERR_GATEWAY_SESSION_TIMEOUT                       = -223,    //子设备session超时
    QCLOUD_ERR_GATEWAY_SUBDEV_ONLINE                         = -224,    //子设备已在线
    QCLOUD_ERR_GATEWAY_SUBDEV_OFFLINE                        = -225,    //子设备已不在线

    QCLOUD_ERR_TCP_SOCKET_FAILED                             = -601,    // 表示TCP连接建立套接字失败
    QCLOUD_ERR_TCP_UNKNOWN_HOST                              = -602,    // 表示无法通过主机名获取IP地址
    QCLOUD_ERR_TCP_CONNECT                                   = -603,    // 表示建立TCP连接失败
    QCLOUD_ERR_TCP_READ_TIMEOUT                              = -604,    // 表示TCP读超时
    QCLOUD_ERR_TCP_WRITE_TIMEOUT                             = -605,    // 表示TCP写超时
    QCLOUD_ERR_TCP_READ_FAIL                                 = -606,    // 表示TCP读错误
    QCLOUD_ERR_TCP_WRITE_FAIL                                = -607,    // 表示TCP写错误
    QCLOUD_ERR_TCP_PEER_SHUTDOWN                             = -608,    // 表示TCP对端关闭了连接
    QCLOUD_ERR_TCP_NOTHING_TO_READ                           = -609,    // 表示底层没有数据可以读取

    QCLOUD_ERR_SSL_INIT                                      = -701,    // 表示SSL初始化失败
    QCLOUD_ERR_SSL_CERT                                      = -702,    // 表示SSL证书相关问题
    QCLOUD_ERR_SSL_CONNECT                                   = -703,    // 表示SSL连接失败
    QCLOUD_ERR_SSL_CONNECT_TIMEOUT                           = -704,    // 表示SSL连接超时
    QCLOUD_ERR_SSL_WRITE_TIMEOUT                             = -705,    // 表示SSL写超时
    QCLOUD_ERR_SSL_WRITE                                     = -706,    // 表示SSL写错误
    QCLOUD_ERR_SSL_READ_TIMEOUT                              = -707,    // 表示SSL读超时
    QCLOUD_ERR_SSL_READ                                      = -708,    // 表示SSL读错误
    QCLOUD_ERR_SSL_NOTHING_TO_READ                           = -709,    // 表示底层没有数据可以读取

    QCLOUD_ERR_UDP_SOCKET_FAILED                             = -601,    // 表示UDP连接建立套接字失败
    QCLOUD_ERR_UDP_UNKNOWN_HOST                              = -602,    // 表示无法通过主机名获取IP地址
    QCLOUD_ERR_UDP_CONNECT                                   = -603,    // 表示建立UDP连接失败
    QCLOUD_ERR_UDP_READ_TIMEOUT                              = -604,    // 表示UDP读超时
    QCLOUD_ERR_UDP_WRITE_TIMEOUT                             = -605,    // 表示UDP写超时
    QCLOUD_ERR_UDP_READ_FAIL                                 = -606,    // 表示UDP读错误
    QCLOUD_ERR_UDP_WRITE_FAIL                                = -607,    // 表示UDP写错误
    QCLOUD_ERR_UDP_NOTHING_TO_READ                           = -609,    // 表示底层没有数据可以读取
} qcloud_err_t;

#endif

