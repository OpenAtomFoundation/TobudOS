
# mqttclient设计与实现方式

# 设计思想
- 整体采用分层式设计，代码实现采用异步设计方式，降低耦合。

- 消息的处理使用回调的方式处理：用户指定**订阅的主题**与指定**消息的处理函数**。

- 不依赖外部任何文件。

# API接口

**mqttclient**拥有非常简洁的**API**接口，参数都是非常简单的。

| API | 说明 | 示例 |
| -- | -- | -- |
| mqtt_lease() | 申请一个mqtt客户端 | mqtt_client_t *client = mqtt_lease(); |
| mqtt_release() | 释放已申请的mqtt客户端 | mqtt_release(client); |
| mqtt_connect() | 与服务器建立连接 | mqtt_connect(client); |
| mqtt_disconnect() | 与服务器断开连接 | mqtt_disconnect(client); |
| mqtt_subscribe() | 订阅主题，参数：主题名字、服务质量、指定当收到主题数据时的处理函数。| mqtt_subscribe(client, "topic", QOS0, sub_topic_handle); |
| mqtt_unsubscribe() | 取消订阅指定主题，参数：主题名字 | mqtt_unsubscribe(client, | "topic"); |
| mqtt_publish() | 向指定主题发布数据，参数：主题名字，mqtt_message_t类型的数据内容 | mqtt_publish(client, "topic", &msg); |
| mqtt_list_subscribe_topic() | 列出客户端已订阅的主题 | mqtt_list_subscribe_topic(client); |
| mqtt_set_host() | 设置要连接的MQTT服务器地址，参数：域名 / 点分十进制的IP地址 | mqtt_set_host(client, "www.jiejie01.top"); |
| mqtt_set_port() | 设置要连接的MQTT服务器端口号 | mqtt_set_port(client, "1883"); |
| mqtt_set_ca() | 设置要连接的MQTT服务器ca证书 | mqtt_set_ca(client, "ca ..."); |
| mqtt_set_user_name() | 设置客户端的用户名 | mqtt_set_user_name(client, "any"); |
| mqtt_set_password() | 设置客户端的密码 | mqtt_set_password(client, "any"); |
| mqtt_set_client_id() | 设置客户端的ID | mqtt_set_client_id(client, "any"); |
| mqtt_set_clean_session() | 设置在断开连接后清除会话 | mqtt_set_clean_session(client, 1); |
| mqtt_set_keep_alive_interval() | 设置心跳间隔时间（秒） | mqtt_set_keep_alive_interval(client, 50); |
| mqtt_set_cmd_timeout() | 设置命令超时时间（毫秒），主要用于socket读写超时 | mqtt_set_cmd_timeout(client, 5000); |
| mqtt_set_reconnect_try_duration() | 设置重连的时间间隔（毫秒） | mqtt_set_reconnect_try_duration(client, 1024); |
| mqtt_set_read_buf_size() | 设置读数据缓冲区的大小 | mqtt_set_read_buf_size(client, 1024); |
| mqtt_set_write_buf_size() | 设置写数据缓冲区的大小 | mqtt_set_write_buf_size(client, 1024); |
| mqtt_set_will_flag() | 设置遗嘱标记 | mqtt_set_will_flag(client, 1); |
| mqtt_set_will_options() | 设置遗嘱的配置信息，指定遗嘱主题，服务质量，遗嘱保留标记，遗嘱内容 | mqtt_set_will_options(client, "will_topic", QOS0, 0, "will_message"); |
| mqtt_set_version() | 设置MQTT协议的版本，默认值是4，MQTT版本为3.1.1 | mqtt_set_version(client, 4); |
| mqtt_set_reconnect_handler() | 设置重连时的回调函数 | mqtt_set_reconnect_handler(client, reconnect_handler); |
| mqtt_set_interceptor_handler() | 设置拦截器处理函数，将所有底层数据上报给用户 | mqtt_set_interceptor_handler(client, interceptor_handler); |

# MQTT客户端的核心结构

**mqtt_client_t 结构**

```c
typedef struct mqtt_client {
    char                        *mqtt_client_id;
    char                        *mqtt_user_name;
    char                        *mqtt_password;
    char                        *mqtt_read_buf;
    char                        *mqtt_write_buf;
    char                        *mqtt_host;
    char                        *mqtt_port;
    char                        *mqtt_ca;
    void                        *mqtt_reconnect_data;
    uint16_t                    mqtt_keep_alive_interval;
    uint16_t                    mqtt_packet_id;
    uint32_t                    mqtt_will_flag          : 1;
    uint32_t                    mqtt_clean_session      : 1;
    uint32_t                    mqtt_ping_outstanding   : 2;
    uint32_t                    mqtt_version            : 4;
    uint32_t                    mqtt_ack_handler_number : 24;
    uint32_t                    mqtt_cmd_timeout;
    uint32_t                    mqtt_read_buf_size;
    uint32_t                    mqtt_write_buf_size;
    uint32_t                    mqtt_reconnect_try_duration;
    size_t                      mqtt_client_id_len;
    size_t                      mqtt_user_name_len;
    size_t                      mqtt_password_len;
    mqtt_will_options_t         *mqtt_will_options;
    client_state_t              mqtt_client_state;
    platform_mutex_t            mqtt_write_lock;
    platform_mutex_t            mqtt_global_lock;
    mqtt_list_t                 mqtt_msg_handler_list;
    mqtt_list_t                 mqtt_ack_handler_list;
    network_t                   *mqtt_network;
    platform_thread_t           *mqtt_thread;
    platform_timer_t            mqtt_reconnect_timer;
    platform_timer_t            mqtt_last_sent;
    platform_timer_t            mqtt_last_received;
    reconnect_handler_t         mqtt_reconnect_handler;
    interceptor_handler_t       mqtt_interceptor_handler;
} mqtt_client_t;
```

该结构主要维护以下内容：

1. MQTT客户端连接服务器必要的参数，如**客户端ID mqtt_client_id、用户名mqtt_user_name、密码mqtt_password**以及**客户端ID长度mqtt_client_id_len、用户名长度mqtt_user_name_len、密码长度mqtt_password_len**等。

2. 读写数据缓冲区**mqtt_read_buf、mqtt_write_buf**及其大小的配置**mqtt_read_buf_size、mqtt_write_buf_size**。

3. 服务器相关的配置信息，如**服务器地址mqtt_host、服务器端口号mqtt_port、服务器CA证书mqtt_ca**。

4. 一些MQTT客户端的配置信息：如**心跳时间间隔mqtt_keep_alive_interval、MQTT报文标识符mqtt_packet_id、遗嘱标记位mqtt_will_flag、清除会话标记mqtt_clean_session、MQTT协议版本mqtt_version、等待应答列表的最大记录个数mqtt_ack_handler_number**等。

5. 一些其他的配置，如**遗嘱消息相关的配置mqtt_will_options、客户端的状态mqtt_client_state、写缓冲区的互斥锁mqtt_write_lock、全局的互斥锁mqtt_global_lock**等。

6. 命令超时时间**mqtt_cmd_timeout**（主要是读写阻塞时间、等待响应的时间、重连等待时间等）。

7. 维护消息处理列表**mqtt_msg_handler_list**，这是**mqtt**协议必须实现的内容，所有来自服务器的**publish**报文都会被处理（前提是订阅了对应的消息，或者设置了拦截器）。

8. 维护**ack**链表**mqtt_ack_handler_list**，这是异步实现的核心，所有等待响应的报文都会被挂载到这个链表上。

9.  维护一个网络组件层**mqtt_network**，它可以自动选择数据通道。

10. 维护一个内部线程**mqtt_thread**，所有来自服务器的mqtt包都会在内部线程这里被处理！

11. 两个定时器，分别是掉线重连定时器与保活定时器**mqtt_reconnect_timer、mqtt_last_sent、mqtt_last_received**

12. 设置掉线重连后告知应用层的回调函数**mqtt_reconnect_handler**与参数**mqtt_reconnect_data**。

13. 设置底层的拦截器的回调函数**mqtt_interceptor_handler**，将所有底层数据上报给应用层。


# mqttclient实现

以下是整个框架的实现方式，方便大家更容易理解mqttclient的代码与设计思想，让大家能够修改源码与使用，还可以提交pr或者issues，开源的世界期待各位大神的参与，感谢！

除此之外以下代码的**记录机制**与**超时处理机制**是非常好的编程思想，大家有兴趣一定要看源代码！

## 申请一个mqtt客户端

```c
mqtt_client_t *mqtt_lease(void);
```

1. 这个函数的内部通过动态申请内存的方式申请了一个MQTT客户端结构**mqtt_client_t**。

2. 调用**_mqtt_init()**函数将其内部的进行了默认的初始化，如申请网络组件的内存空间、初始化相关的互斥锁、链表等。


## 释放已申请的mqtt客户端

```c
mqtt_release()
```

回收MQTT客户端结构**mqtt_client_t**的内存空间、网络组件的内存空间、与服务器断开连接。

## 设置MQTT客户端的信息

通过宏定义去统一设置MQTT客户端结构**mqtt_client_t**的信息，定义如下：

```c
#define MQTT_CLIENT_SET_DEFINE(name, type, res)         \
    type mqtt_set_##name(mqtt_client_t *c, type t) {    \
        MQTT_ROBUSTNESS_CHECK((c), res);                \
        c->mqtt_##name = t;                             \
        return c->mqtt_##name;                          \
    }
```

由编译器预处理得到相关的函数：**mqtt_set_xxx()**。

```c
MQTT_CLIENT_SET_DEFINE(client_id, char*, NULL)
MQTT_CLIENT_SET_DEFINE(user_name, char*, NULL)
MQTT_CLIENT_SET_DEFINE(password, char*, NULL)
MQTT_CLIENT_SET_DEFINE(host, char*, NULL)
MQTT_CLIENT_SET_DEFINE(port, char*, NULL)
MQTT_CLIENT_SET_DEFINE(ca, char*, NULL)
MQTT_CLIENT_SET_DEFINE(reconnect_data, void*, NULL)
MQTT_CLIENT_SET_DEFINE(keep_alive_interval, uint16_t, 0)
MQTT_CLIENT_SET_DEFINE(will_flag, uint32_t, 0)
MQTT_CLIENT_SET_DEFINE(clean_session, uint32_t, 0)
MQTT_CLIENT_SET_DEFINE(version, uint32_t, 0)
MQTT_CLIENT_SET_DEFINE(cmd_timeout, uint32_t, 0)
MQTT_CLIENT_SET_DEFINE(read_buf_size, uint32_t, 0)
MQTT_CLIENT_SET_DEFINE(write_buf_size, uint32_t, 0)
MQTT_CLIENT_SET_DEFINE(reconnect_try_duration, uint32_t, 0)
MQTT_CLIENT_SET_DEFINE(reconnect_handler, reconnect_handler_t, NULL)
MQTT_CLIENT_SET_DEFINE(interceptor_handler, interceptor_handler_t, NULL)
```

## 连接服务器

```c
int mqtt_connect(mqtt_client_t* c);
```

参数只有 **mqtt_client_t** 类型的指针，连接服务器则是使用非异步的方式设计，因为必须等待连接上服务器才能进行下一步操作。

过程如下：

1. 调用底层的连接函数连接上服务器：

    ```c
    network_connect(c->network);
    ```

2. 序列化**mqtt**的**CONNECT**报文并且发送。

    ```c
    MQTTSerialize_connect(c->write_buf, c->write_buf_size, &connect_data)
    mqtt_send_packet(c, len, &connect_timer)
    ```

3. 等待来自服务器的**CONNACK**报文

    ```c
    mqtt_wait_packet(c, CONNACK, &connect_timer)
    ```

4. 连接成功后创建一个内部线程**mqtt_yield_thread**，并在合适的时候启动它：

    ```c
    /* connect success, and need init mqtt thread */
    c->mqtt_thread= platform_thread_init("mqtt_yield_thread", mqtt_yield_thread, c, MQTT_THREAD_STACK_SIZE, MQTT_THREAD_PRIO, MQTT_THREAD_TICK);

    if (NULL != c->mqtt_thread) {
        mqtt_set_client_state(c, CLIENT_STATE_CONNECTED);
        platform_thread_startup(c->mqtt_thread);
        platform_thread_start(c->mqtt_thread);       /* start run mqtt thread */
    }
    ```

5. 而对于重连来说则不会重新创建线程，直接改变客户端状态为连接状态即可:

    ```c
    mqtt_set_client_state(c, CLIENT_STATE_CONNECTED);
    ```

## 订阅报文

```c
int mqtt_subscribe(mqtt_client_t* c, const char* topic_filter, mqtt_qos_t qos, message_handler_t handler)
```

订阅报文使用异步设计来实现的，参数有字符串类型的**主题**（支持通配符"#" "+"），主题的**服务质量**，以及收到报文的**处理函数`**，如不指定则有默认处理函数。

过程如下：

1. 序列化订阅报文并且发送给服务器

    ```c
    MQTTSerialize_subscribe(c->write_buf, c->write_buf_size, 0, mqtt_get_next_packet_id(c), 1, &topic, (int*)&qos)
    mqtt_send_packet(c, len, &timer)
    ```

2. 创建对应的消息处理节点，这个消息节点在收到服务器的**SUBACK**订阅应答报文后会挂载到消息处理列表**mqtt_msg_handler_list**上

    ```c
    mqtt_msg_handler_create(topic_filter, qos, handler)
    ```

3. 在发送了报文给服务器那就要等待服务器的响应了，先记录这个等待**SUBACK**

    ```c
    mqtt_ack_list_record(c, SUBACK, mqtt_get_next_packet_id(c), len, msg_handler)
    ```

## 取消订阅

```c
int mqtt_unsubscribe(mqtt_client_t* c, const char* topic_filter);
```

与订阅报文的逻辑基本差不多的，指定了取消订阅的主题。

实现过程如下：

1. 序列化订阅报文并且发送给服务器

    ```c
    MQTTSerialize_unsubscribe(c->write_buf, c->write_buf_size, 0, packet_id, 1, &topic)
    mqtt_send_packet(c, len, &timer)
    ```

2. 创建对应的消息处理节点，这个消息节点在收到服务器的**UNSUBACK**取消订阅应答报文后将消息处理列表**mqtt_msg_handler_list**上的已经订阅的主题消息节点销毁

    ```c
    mqtt_msg_handler_create((const char*)topic_filter, QOS0, NULL)
    ```

3. 在发送了报文给服务器那就要等待服务器的响应了，先记录这个等待**UNSUBACK**

    ```c
    mqtt_ack_list_record(c, UNSUBACK, packet_id, len, msg_handler)
    ```

## 发布报文

```c
int mqtt_publish(mqtt_client_t* c, const char* topic_filter, mqtt_message_t* msg)
```

向指定主题发布一个MQTT报文。参数只有**mqtt_client_t** 类型的指针，字符串类型的**主题**（支持通配符），要发布的消息（包括**服务质量**、**消息主体**）。

使用如下：

```c
mqtt_message_t msg;

msg.qos = 2;
msg.payload = (void *) buf;

mqtt_publish(&client, "testtopic1", &msg);
```

代码的实现核心思想都差不多，过程如下：

1. 先序列化发布报文，然后发送到服务器

    ```c
    MQTTSerialize_publish(c->write_buf, c->write_buf_size, 0, msg->qos, msg->retained, msg->id,topic, (unsigned char*)msg->payload, msg->payloadlen);
    mqtt_send_packet(c, len, &timer)
    ```

2. 对于QOS0的逻辑，不做任何处理，对于QOS1和QOS2的报文则需要记录下来，在没收到服务器应答的时候进行重发

    ```c
    if (QOS1 == msg->qos) {
        rc = mqtt_ack_list_record(c, PUBACK, mqtt_get_next_packet_id(c), len, NULL);
    } else if (QOS2 == msg->qos) {
        rc = mqtt_ack_list_record(c, PUBREC, mqtt_get_next_packet_id(c), len, NULL);
    }
    ```

3. 还有非常重要的一点，重发报文的MQTT报文头部需要设置DUP标志位，这是MQTT协议的标准，因此，在重发的时候作者直接操作了报文的DUP标志位，因为修改DUP标志位的函数我没有从MQTT库中找到，所以我封装了一个函数，这与LwIP中的交叉存取思想是一个道理，它假设我知道MQTT报文的所有操作，所以我可以操作它，这样子可以提高很多效率：

    ```c
    mqtt_set_publish_dup(c,1);  /* may resend this data, set the udp flag in advance */
    ```

## 内部线程

```c
static void mqtt_yield_thread(void *arg)
```

主要是对**mqtt_yield**函数的返回值做处理，比如在**disconnect**的时候销毁这个线程。

## 核心的处理函数

1. 数据包的处理**mqtt_packet_handle**

    ```c
    static int mqtt_packet_handle(mqtt_client_t* c, platform_timer_t* timer)
    ```

    对不同的包使用不一样的处理：

    ```c
    switch (packet_type) {
        case 0: /* timed out reading packet */
            break;

        case CONNACK:
            break;

        case PUBACK:
        case PUBCOMP:
            rc = mqtt_puback_and_pubcomp_packet_handle(c, timer);
            break;

        case SUBACK:
            rc = mqtt_suback_packet_handle(c, timer);
            break;
            
        case UNSUBACK:
            rc = mqtt_unsuback_packet_handle(c, timer);
            break;

        case PUBLISH:
            rc = mqtt_publish_packet_handle(c, timer);
            break;

        case PUBREC:
        case PUBREL:
            rc = mqtt_pubrec_and_pubrel_packet_handle(c, timer);
            break;

        case PINGRESP:
            c->ping_outstanding = 0;
            break;

        default:
            goto exit;
    }
    ```

    并且做保活的处理：

    ```c
    mqtt_keep_alive(c)
    ```

    当发生超时后的处理：

    ```c
    if (platform_timer_is_expired(&c->last_sent) || platform_timer_is_expired(&c->last_received)) 
    ```

    序列化一个心跳包并且发送给服务器

    ```c
    MQTTSerialize_pingreq(c->write_buf, c->write_buf_size);
    mqtt_send_packet(c, len, &timer);
    ```

    当再次发生超时后，表示与服务器的连接已断开，需要重连的操作，设置客户端状态为断开连接

    ```c
    mqtt_set_client_state(c, CLIENT_STATE_DISCONNECTED);
    ```

2. `ack`链表的扫描，当收到服务器的报文时，对ack列表进行扫描操作

    ```c
    mqtt_ack_list_scan(c);
    ```

    当超时后就销毁ack链表节点：

    ```c
    mqtt_ack_handler_destroy(ack_handler);
    ```

    当然下面这几种报文则需要重发操作：（**PUBACK 、PUBREC、 PUBREL 、PUBCOMP**，保证QOS1 QOS2的服务质量）

    ```c
    if ((ack_handler->type ==  PUBACK) || (ack_handler->type ==  PUBREC) || (ack_handler->type ==  PUBREL) || (ack_handler->type ==  PUBCOMP))
        mqtt_ack_handler_resend(c, ack_handler);
    ```

3. 保持活性的时间过去了，可能掉线了，需要重连操作

    ```c
    mqtt_try_reconnect(c);
    ```

    重连成功后尝试重新订阅报文，保证恢复原始状态~

    ```c
    mqtt_try_resubscribe(c)
    ```

## 发布应答与发布完成报文的处理

```c
static int mqtt_puback_and_pubcomp_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
```

1. 反序列化报文

    ```c
    MQTTDeserialize_ack(&packet_type, &dup, &packet_id, c->read_buf, c->read_buf_size)
    ```

2. 取消对应的ack记录

    ```c
    mqtt_ack_list_unrecord(c, packet_type, packet_id, NULL);
    ```

## 订阅应答报文的处理

    ```c
    static int mqtt_suback_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
    ```

1. 反序列化报文

    ```c
    MQTTDeserialize_suback(&packet_id, 1, &count, (int*)&granted_qos, c->read_buf, c->read_buf_size)
    ```

2. 取消对应的ack记录

    ```c
    mqtt_ack_list_unrecord(c, packet_type, packet_id, NULL);
    ```

3. 安装对应的订阅消息处理函数，如果是已存在的则不会安装

    ```c
    mqtt_msg_handlers_install(c, msg_handler);
    ```

## 取消订阅应答报文的处理

    ```c
    static int mqtt_unsuback_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
    ```

1. 反序列化报文

    ```c
    MQTTDeserialize_unsuback(&packet_id, c->read_buf, c->read_buf_size)
    ```

2. 取消对应的ack记录，并且获取到已经订阅的消息处理节点

    ```c
    mqtt_ack_list_unrecord(c, UNSUBACK, packet_id, &msg_handler)
    ```

3. 销毁对应的订阅消息处理函数

    ```c
    mqtt_msg_handler_destory(msg_handler);
    ```

## 来自服务器的发布报文的处理

    ```c
    static int mqtt_publish_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
    ```

1. 反序列化报文

    ```c
    MQTTDeserialize_publish(&msg.dup, &qos, &msg.retained, &msg.id, &topic_name,
            (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->read_buf, c->read_buf_size)
    ```

2. 对于QOS0、QOS1的报文，直接去处理消息

    ```c
    mqtt_deliver_message(c, &topic_name, &msg);
    ```

3. 对于QOS1的报文，还需要发送一个**PUBACK**应答报文给服务器

    ```c
    MQTTSerialize_ack(c->write_buf, c->write_buf_size, PUBACK, 0, msg.id);
    ```

4. 而对于QOS2的报文则需要发送**PUBREC**报文给服务器，除此之外还需要记录**PUBREL**到ack链表上，等待服务器的发布释放报文，最后再去处理这个消息

    ```c
    MQTTSerialize_ack(c->write_buf, c->write_buf_size, PUBREC, 0, msg.id);
    mqtt_ack_list_record(c, PUBREL, msg.id + 1, len, NULL)
    mqtt_deliver_message(c, &topic_name, &msg);
    ```

说明：一旦注册到ack列表上的报文，当具有重复的报文是不会重新被注册的，它会通过**mqtt_ack_list_node_is_exist()**函数判断这个节点是否存在，主要是依赖等待响应的消息类型与msgid。

## 发布收到与发布释放报文的处理

```c
static int mqtt_pubrec_and_pubrel_packet_handle(mqtt_client_t *c, platform_timer_t *timer)
```

1. 反序列化报文

    ```c
    MQTTDeserialize_ack(&packet_type, &dup, &packet_id, c->read_buf, c->read_buf_size)
    ```

2. 产生一个对应的应答报文

    ```c
    mqtt_publish_ack_packet(c, packet_id, packet_type);
    ```

3. 取消对应的ack记录

    ```c
    mqtt_ack_list_unrecord(c, UNSUBACK, packet_id, &msg_handler)
    ```

**上一篇**：[mqttclient配置及裁剪工具](./mqtt-config.md)

**下一篇**：[mqttclient连接到百度天工物接入](./mqtt-baidu.md)