
# mqttclient配置及裁剪工具

- MQTT_TOPIC_LEN_MAX

    配置客户端支持最大的主题名长度，主题是支持通配符的，如果主题太长则会被截断，默认值为64。

    ```c
        #define     MQTT_TOPIC_LEN_MAX                  64
    ```

- MQTT_ACK_HANDLER_NUM_MAX

    配置mqtt等待应答列表的最大记录个数，对于qos1、qos2服务质量有要求的可以将其设置大一点，当然也必须资源跟得上，它主要是保证qos1、qos2的mqtt报文能准确到达服务器，默认值为64。

    ```c
        #define     MQTT_ACK_HANDLER_NUM_MAX            64
    ```

- MQTT_DEFAULT_BUF_SIZE

    默认的读写数据缓冲区的大小，根据要收发数据量修改即可，默认值为1024。

    ```c
        #define     MQTT_DEFAULT_BUF_SIZE               1024
    ```

- MQTT_DEFAULT_CMD_TIMEOUT

    默认的命令超时，它主要是用于socket读写超时（还包括等待响应的时间、重连等待时间等），默认值为5000。

    ```c
        #define     MQTT_DEFAULT_CMD_TIMEOUT            5000
    ```

- MQTT_MAX_CMD_TIMEOUT

    设置最大的命令超时时间，默认值是20000。

    ```c
        #define     MQTT_MAX_CMD_TIMEOUT                20000
    ```

- MQTT_MIN_CMD_TIMEOUT

    设置最小的命令超时时间，默认值是1000。

    ```c
        #define     MQTT_MIN_CMD_TIMEOUT                1000
    ```

- MQTT_KEEP_ALIVE_INTERVAL

    设置默认的保活时间，它主要是保证MQTT客户端与服务器的保持活性连接，单位为**秒**，默认值为50。比如MQTT客户端与服务器在指定时间间隔内没有发送数据，有没有接收到数据，此时MQTT客户端会发送一个心跳包，确认一下这个连接是否存在，如果收到服务器的应答，那么说明这个连接还是存在的，可以随时收发数据，而如果不存在了，就尝试重连或者清除会话。

    ```c
        #define     MQTT_KEEP_ALIVE_INTERVAL            50         // unit: second
    ```

- MQTT_VERSION

    选择MQTT协议的版本，默认为4，表示使用MQTT 3.1.1版本，而3则表示使用MQTT 3.1版本。

    ```c
        #define     MQTT_VERSION                        4           // 4 is mqtt 3.1.1
    ```

- MQTT_RECONNECT_DEFAULT_DURATION

    设置默认的重连时间间隔，当发生掉线时，会以这个时间间隔尝试重连，默认值为1000。

    ```c
        #define     MQTT_RECONNECT_DEFAULT_DURATION     1000
    ```

- MQTT_THREAD_STACK_SIZE

    MQTT内部维护一个线程，需要设置默认的线程属性信息，MQTT_THREAD_STACK_SIZE表示线程栈的大小，默认值是2048。在linux环境下可以是不需要理会这些参数的，而在RTOS平台则需要配置，如果不使用mbedtls，线程栈2048字节已足够，而使用mbedtls加密后，需要配置4096字节以上

    ```c
        #define     MQTT_THREAD_STACK_SIZE              2048    // 线程栈
    ```

- MQTT_THREAD_PRIO

    设置线程优先级，默认为5。

    ```c
        #define     MQTT_THREAD_PRIO                    5       // 线程优先级
    ```

- MQTT_THREAD_TICK

    设置线程优时间片，默认为50。

    ```c
        #define     MQTT_THREAD_TICK                    50      // 线程时间片
    ```

- MQTT_NETWORK_TYPE_NO_TLS

    设置是否需要支持TLS加密传输，如果定义了该宏，则表示不支持TLS加密传输，否则默认将支持TLS加密传输。

    ```c
        #define     MQTT_NETWORK_TYPE_NO_TLS
    ```

# salof相关的配置

[salof](https://github.com/jiejieTop/salof) 全称是：**Synchronous Asynchronous Log Output Framework**（同步异步日志输出框架），它是一个同步异步日志输出框架，可以在空闲时候输出对应的日志信息，并且该库与mqttclient无缝衔接。

- LOG_LEVEL
  
    配置对应的日志输出级别，它支持以下4种级别：ERR_LEVEL、WARN_LEVEL、INFO_LEVEL、DEBUG_LEVEL。

    ```c
        #define BASE_LEVEL      (0)
        #define ERR_LEVEL       (BASE_LEVEL + 1)          /* 日志输出级别：错误级别（高优先级） */
        #define WARN_LEVEL      (ERR_LEVEL + 1)             /* 日志输出级别：警告级别（中优先级） */
        #define INFO_LEVEL      (WARN_LEVEL + 1)            /* 日志输出级别：信息级别（低优先级） */
        #define DEBUG_LEVEL     (INFO_LEVEL + 1)            /* 日志输出级别：调试级别（更低优先级） */

        #define         LOG_LEVEL                   WARN_LEVEL      /* 日志输出级别 */
    ```

- USE_SALOF

    如果该宏大于0，则表示使用salof指定的后端作为日志输出，反之则使用printf作为日志输出接口。

    ```c
        #define         USE_SALOF                   (1U)
    ```

- SALOF_OS

    设置salof日志库的操作系统，有以下选项：USE_LINUX、USE_TENCENTOS、USE_FREERTOS、USE_RTT。

    ```c
        #define         SALOF_OS                    USE_LINUX
    ```

- USE_IDLE_HOOK

    如果该宏大于0，则表示在操作系统中的空闲钩子函数中输出日志，反之则通过线程异步处理或者同步处理。

    ```c
        #define         USE_IDLE_HOOK               (0U)
    ```

- LOG_COLOR

    如果该宏大于0，则表示输出的日志是带有颜色的（需要终端的支持），反之则没有颜色。

    ```c
        #define         LOG_COLOR                   (1U)
    ```

- LOG_TS

    如果该宏大于0，则表示输出的日志是带有时间戳的，反之则没有时间戳。

    ```c
        #define         LOG_TS                   (0U)
    ```

- LOG_TAR

    如果该宏大于0，则表示输出的日志是带有标签的（任务名字），反之则没有标签。

    ```c
        #define         LOG_TAR                   (0U)
    ```

- SALOF_BUFF_SIZE

    设置日志库salof输出库的buf缓冲区大小，即最大一次性能输出多少个字节。

    ```c
        #define         SALOF_BUFF_SIZE                   (512U)
    ```

- SALOF_FIFO_SIZE

    设置日志库salof的fifo缓冲区大小，即最能缓存多少个字节的日志数据。

    ```c
            #define         SALOF_FIFO_SIZE             (1024*4U)
    ```

- SALOF_TASK_STACK_SIZE

    如果使用了空闲线程输出日志，则配置salof线程栈的大小。

    ```c
        #define         SALOF_TASK_STACK_SIZE       (2048U)
    ```

- SALOF_TASK_TICK

    如果使用了空闲线程输出日志，则配置salof线程栈的时间片大小。

    ```c
        #define         SALOF_TASK_TICK             (50U)
    ```

# 使用mqttclient裁剪配置工具

我们可以通过配置很方便地裁剪我们需要的功能，设置上述MQTT客户端的默认参数，完全不需要手动修改代码，直接使用工具裁剪配置即可。

打开在线mqttclient裁剪配置工具：[https://jiejietop.gitee.io/mqtt/mqtt-config.html](https://jiejietop.gitee.io/mqtt/mqtt-config.html)

它支持裁剪配置相关的头文件，配置客户端默认的一些参数：

![mqtt-config-tool01](http://qiniu.jiejie01.top/mqtt-config-tool01.png)

还支持配置salof同步异步日志相关的信息，以保证日志能正常运作，配置完成后生成对应的代码，然后覆盖掉原本的MQTT客户端配置即可，如果不知道如何配置，那就导入默认的配置即可。

![mqtt-config-tool02](http://qiniu.jiejie01.top/mqtt-config-tool02.png)

如果是初次接触，看不懂上面配置的信息，没关系，我很贴心地准备了配置的宏定义及其描述信息，保证你能看得懂的。

![mqtt-config-tool03](http://qiniu.jiejie01.top/mqtt-config-tool03.png)


**上一篇**：[mqttclient代码生成工具](./mqtt-tool.md)

**下一篇**：[mqttclient设计与实现方式](./mqtt-design.md)
