# mqtt连接到百度天工物接入

## 百度天工物接入简介

物接入（IoT Hub）是面向物联网领域开发者的全托管云服务，通过主流的物联网协议（如MQTT）进行通信，可以在智能设备与云端之间建立安全的双向连接，快速实现物联网项目。支持亿级并发连接和消息数，支持海量设备与云端安全可靠的双向连接，无缝对接天工平台和百度云的各项产品和服务。

物接入分为设备型和数据型两种项目类型，我们在这里使用数据型项目类型，传输一些温湿度数据，同时可以无缝对接时序数据库TSDB、物可视等，将数据实时显示出来，同时我们采用MQTT协议与云端进行通讯。

物接入主要采用后付费方式，根据你的实际使用量实时计费，即你只需为已使用的传输消息条数付费。

在使用物接入服务前，必须要创建一个百度云账号用于管理我们的设备，所有的设备信息都是在云端后台进行管理的，地址：https://console.bce.baidu.com/iot2/hub/。

使用IoT Hub之前应先了解一下计费套餐，目前IoT Hub的计费是很便宜的，每个月的前1000000条消息是免费的，更多的消息数量请自行参考IoT Hub的收费说明，因此我们用它来做实验早已足够，计算一下，假设我们的开发板在一个月内不间断以3秒的频率发送一次消息到IoT Hub，那么一个月只能发送30*24*60*(60/3) = 864000，还不到1000000条呢，因此这个流量用来做实验早就绰绰有余，即使这个月用完了流量，下个月还有1000000条，只要当月没超出这个流量都是可以免费使用，具体见

| 月用量：消息条数N | 价格(元/百万条) |
| -- | -- |
| N < 100万 | 免费 |
| 100万 < N < 1亿条 | 1.2 |
| 1亿条 < N < 10亿条 | 1.0 |
| 10亿条以上 | 0.8 |

当然也可以按消息数计算，以下计价单位中的条数代表发布消息（PUB）和订阅消息（SUB）之和。例如：有5台设备订阅了同一个主题（topic），第6台设备向该主题发布1条消息，则总条数的计算方法为：1（PUB）+5（SUB）=6。 在计费上，消息长度在512Bytes内的记为一条消息，超出部分将被算作是一条或多条新的消息，也就是“实际消息长度/512Bytes”的计算结果向上取整。（在实际使用中，用户上传的单条消息大小限制是32KB，超过32KB的消息会被丢弃）

## 使用百度天工物接入

### 创建项目

IoT Hub是用于物联网设备的后台管理的，将设备接入IoT Hub的第1步就是创建一个项目，每个项目会对应一个接入点（endpoint），一个项目就表示一个完整的物接入服务，首先登录物接入控制台页面：https://console.bce.baidu.com/iot2/hub/project/list，点击“创建项目”，填写需要创建 IoT Hub 服务的项目名称、选择项目类型为数据型，并提交即可，创建的项目，默认具有多种通信方式，我们暂时只关注TCP连接方式即可，因为开发板是采用MQTT协议与云端交互的，端口号是1883。

![mqtt-baidu000](http://qiniu.jiejie01.top/mqtt-baidu000.png)

![mqtt-baidu001](http://qiniu.jiejie01.top/mqtt-baidu001.png)


此处补充说明一点：通过项目可以将不同项目的设备进行隔离和管理，一个项目下允许有多个用户（设备），用户与身份进行绑定，而每个身份需要绑定一个策略，而策略下可以创建多个不同的主题，因此一个设备可以订阅多个不同的主题，并且同一个项目下的主题是共享的，所有的设备均可进行订阅。

具体见：

![mqtt-baidu0011](http://qiniu.jiejie01.top/mqtt-baidu0011.png)


### 创建策略

首先我们点击刚刚创建的“mqtt-client”项目，进入项目里面，首先创建一个策略（简单来说就是主题），输入对应的名称与主题，选择发布与订阅权限，当创建完成后，项目下的设备就可以订阅这个主题，具体见


![mqtt-baidu002](http://qiniu.jiejie01.top/mqtt-baidu002.png)


### 创建身份

身份是开发板连接IoT Hub的重要途径，这里面包含了秘钥，标识着开发板能否通过IoT Hub的验证，这也是安全保障的主要操作，首先点击“身份列表”，再点击“创建身份”，然后根据自己的信息进行填写，在这里要注意的是需要勾选密码验证，再选择之前我们创建的策略进行绑定（如果没有则必须创建），当身份创建完成时候生成的秘钥是用于开发板连接IoT Hub的必要条件，记住不能丢失，如果丢失了就找不回来了，在创建的时候就把它保存好。

![mqtt-baidu003](http://qiniu.jiejie01.top/mqtt-baidu003.png)

![mqtt-baidu004](http://qiniu.jiejie01.top/mqtt-baidu004.png)

![mqtt-baidu005](http://qiniu.jiejie01.top/mqtt-baidu005.png)


### 创建用户

在创建用户的时候，只需要把用户名设置好，然后再绑定身份即可，这样子一个设备就对应到云端了，并且可以向策略中设置的主题发布消息和订阅主题，具体见:

![mqtt-baidu006](http://qiniu.jiejie01.top/mqtt-baidu006.png)

![mqtt-baidu007](http://qiniu.jiejie01.top/mqtt-baidu007.png)

![mqtt-baidu008](http://qiniu.jiejie01.top/mqtt-baidu008.png)

### 测试连接

当所有的东西都创建完成，我们点击身份操作中的“测试连接”，在网页上测试一下我们能不能正常进行MQTT通信、发布与定阅主题，在页面中输入刚刚保存的秘钥，点击“connect”进行连接测试，如果前面的步骤全部做完，并且秘钥是正确的，就可以发现我们的连接是正常的，然后我们订阅一下之前创建策略时候的主题名字“topic1”，再向这个主题发送一个消息，消息的内容由自己定义，

![mqtt-baidu009](http://qiniu.jiejie01.top/mqtt-baidu009.png)

![mqtt-baidu010](http://qiniu.jiejie01.top/mqtt-baidu010.png)

![mqtt-baidu011](http://qiniu.jiejie01.top/mqtt-baidu011.png)

![mqtt-baidu012](http://qiniu.jiejie01.top/mqtt-baidu012.png)


最后回到项目下，可以看到项目的地址信息，等内容。

![mqtt-baidu013](http://qiniu.jiejie01.top/mqtt-baidu013.png)


## MQTT软件测试连接

仔细观察的同学可能会发现，这个连接的端口是8884，因为这是在网页上测试连接的，所以端口不一样是正常的，那么我们也可以使用MQTT客户端软件进行连接测试，这个软件的界面看起来很漂亮，并且它是开源的，我们直接用它就好了，可以从github下载：https://github.com/emqx/MQTTX/releases。

此处演示MQTTX软件去连接百度云：

首先获取到用户名与项目的地址信息：

![mqtt-baidu014](http://qiniu.jiejie01.top/mqtt-baidu014.png)

![mqtt-baidu013](http://qiniu.jiejie01.top/mqtt-baidu013.png)

在软件中填写名称、Client ID，选择MQTT协议，填写项目的地址信息、端口号、用户名以及密码，然后点击连接。

![mqtt-baidu015](http://qiniu.jiejie01.top/mqtt-baidu015.png)

在连接成功后，可以添加订阅的主题名字，向指定的主题发送内容：

![mqtt-baidu016](http://qiniu.jiejie01.top/mqtt-baidu016.png)


## 手动安装相关的依赖包

这些依赖包是使用mqttclient库去连接百度云时必须要安装的。

```bash
sudo apt-get -y install git
```

```bash
sudo apt-get -y install mkae
```

```bash
sudo apt-get -y install gcc
```

```bash
sudo apt-get -y install g++
```

```bash
sudo apt-get -y install cmake
```

## 拉取mqttclient仓库

接着到github拉取这个仓库，仓库非常小，下载下来是很快的，注意，这里也需要拉取到两个开发板上。

```bash
git clone https://github.com/jiejieTop/mqttclient.git
```

当然也可以从gitee仓库下载。

```bash
git clone https://gitee.com/jiejieTop/mqttclient.git
```

## 简单介绍mqttclient仓库文件夹

- common文件夹：是一些通用的文件内容，比如链表的处理，错误代码的处理、随机数生成器、日志库等内容。

- mqtt文件夹：著名的paho mqtt库。

- mqttclient文件夹：实现mqttclient的主要文件，并且包含了一个默认的配置文件。

- network文件夹：网络抽象层，封装了mbedtls加密库、网络数据的通道类型，自动选择tls加密传输或者是tcp直连。

- platform文件夹：平台抽象层，此处封装了各种平台的内存管理、互斥锁、线程管理、时间管理等内容，如linux平台，freertos平台、rt-thread平台、TencentOS tiny平台等。

- test文件夹：一些测试的代码，比如我们连接百度云的时候，就会使用test文件夹的百度云平台的测试代码。

## 编译

拉取下来后看到本地有mqttclient文件夹，我们进去mqttclient目录下，运行它提供的编译脚本，它主要是通过cmake去自动构建整个代码工程。

```bash
./build.sh

# 产生的信息如下:

-- Configuring done
-- Generating done
-- Build files have been written to: /home/jiejie/github/mqttclient/build
[  9%] Built target mqtt
[ 12%] Built target common
[ 17%] Built target platform
[ 21%] Built target wrapper
[ 85%] Built target mbedtls
[ 88%] Built target network
[ 90%] Built target mqttclient
[ 92%] Built target arch
[ 95%] Built target salof
[ 97%] Built target emqx
[ 98%] Built target onenet
[ 99%] Built target baidu
[100%] Built target ali
```

## 运行

当编译完成后，在`./build/bin`目录下会出现多个可执行文件，在`./build/lib`目录下会出现相关的动态库文件，具体如下：

```bash
➜  mqttclient git:(master) ls build/bin
ali  baidu  emqx  onenet

➜  mqttclient git:(master) ls build/lib
libarch.a  libcommon.a  libmbedtls.a  libmqtt.a  libmqttclient.a  libnetwork.a  libplatform.a  libsalof.a  libwrapper.a
```

我们直接运行`./build/bin/baidu`这个可执行文件：

```bash
➜  mqttclient git:(master) ./build/bin/baidu 

welcome to mqttclient test...

[I] >> [TS: 1590399555] /home/jiejie/github/mqttclient/mqttclient/mqttclient.c:948 mqtt_connect_with_results()... mqtt connect success...
[I] >> [TS: 1590399558] /home/jiejie/github/mqttclient/mqttclient/mqttclient.c:1337 mqtt_list_subscribe_topic()...[1] subscribe topic: topic1
[I] >> [TS: 1590399558] -----------------------------------------------------------------------------------
[I] >> [TS: 1590399558] /home/jiejie/github/mqttclient/test/baidu/test.c:48 topic1_handler()...
topic: topic1
message:welcome to mqttclient, this is a publish test, a rand number: 2108237578 ...
[I] >> [TS: 1590399558] -----------------------------------------------------------------------------------
[I] >> [TS: 1590399562] -----------------------------------------------------------------------------------
[I] >> [TS: 1590399562] /home/jiejie/github/mqttclient/test/baidu/test.c:48 topic1_handler()...
topic: topic1
message:welcome to mqttclient, this is a publish test, a rand number: 1675963111 ...
[I] >> [TS: 1590399562] -----------------------------------------------------------------------------------
```

此时表示已经连接上百度云天工了，并且实现了数据的收发。

## 测试代码

那么这个测试的例程代码如下，位于`./test/baidu/test.c`：

```c
/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime: 2020-06-08 20:38:41
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "mqttclient.h"

// #define TEST_USEING_TLS  

static const char *test_baidu_ca_crt = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDXzCCAkegAwIBAgILBAAAAAABIVhTCKIwDQYJKoZIhvcNAQELBQAwTDEgMB4G\r\n"
    "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjMxEzARBgNVBAoTCkdsb2JhbFNp\r\n"
    "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDkwMzE4MTAwMDAwWhcNMjkwMzE4\r\n"
    "MTAwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMzETMBEG\r\n"
    "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\r\n"
    "hvcNAQEBBQADggEPADCCAQoCggEBAMwldpB5BngiFvXAg7aEyiie/QV2EcWtiHL8\r\n"
    "RgJDx7KKnQRfJMsuS+FggkbhUqsMgUdwbN1k0ev1LKMPgj0MK66X17YUhhB5uzsT\r\n"
    "gHeMCOFJ0mpiLx9e+pZo34knlTifBtc+ycsmWQ1z3rDI6SYOgxXG71uL0gRgykmm\r\n"
    "KPZpO/bLyCiR5Z2KYVc3rHQU3HTgOu5yLy6c+9C7v/U9AOEGM+iCK65TpjoWc4zd\r\n"
    "QQ4gOsC0p6Hpsk+QLjJg6VfLuQSSaGjlOCZgdbKfd/+RFO+uIEn8rUAVSNECMWEZ\r\n"
    "XriX7613t2Saer9fwRPvm2L7DWzgVGkWqQPabumDk3F2xmmFghcCAwEAAaNCMEAw\r\n"
    "DgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFI/wS3+o\r\n"
    "LkUkrk1Q+mOai97i3Ru8MA0GCSqGSIb3DQEBCwUAA4IBAQBLQNvAUKr+yAzv95ZU\r\n"
    "RUm7lgAJQayzE4aGKAczymvmdLm6AC2upArT9fHxD4q/c2dKg8dEe3jgr25sbwMp\r\n"
    "jjM5RcOO5LlXbKr8EpbsU8Yt5CRsuZRj+9xTaGdWPoO4zzUhw8lo/s7awlOqzJCK\r\n"
    "6fBdRoyV3XpYKBovHd7NADdBj+1EbddTKJd+82cEHhXXipa0095MJ6RMG3NzdvQX\r\n"
    "mcIfeg7jLQitChws/zyrVQ4PkX4268NXSb7hLi18YIvDQVETI53O9zJrlAGomecs\r\n"
    "Mx86OyXShkDOOyyGeMlhLxS67ttVb9+E7gUJTb0o2HLO02JQZR7rkpeDMdmztcpH\r\n"
    "WD9f\r\n"
    "-----END CERTIFICATE-----"
};

static void topic1_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

void *mqtt_publish_thread(void *arg)
{
    mqtt_client_t *client = (mqtt_client_t *)arg;

    char buf[100] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");

    sleep(2);

    mqtt_list_subscribe_topic(client);

    msg.payload = (void *) buf;
    msg.qos = 0;
    while(1) {
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        mqtt_publish(client, "topic1", &msg);
        sleep(4);
    }
}

int main(void)
{
    int res;
    pthread_t thread1;
    mqtt_client_t *client = NULL;
    
    printf("\nwelcome to mqttclient test...\n");

    mqtt_log_init();

    client = mqtt_lease();

#ifdef TEST_USEING_TLS
    mqtt_set_port(client, "1884");
    mqtt_set_ca(client, (char*)test_baidu_ca_crt);
#else
    mqtt_set_port(client, "1883");
#endif

    mqtt_set_host(client, "j6npr4w.mqtt.iot.gz.baidubce.com");
    mqtt_set_client_id(client, random_string(10));
    mqtt_set_user_name(client, "j6npr4w/mqtt-client-dev");
    mqtt_set_password(client, "lcUhUs5VYLMSbrnB");
    mqtt_set_clean_session(client, 1);

    mqtt_connect(client);
    
    mqtt_subscribe(client, "topic1", QOS0, topic1_handler);
    
    res = pthread_create(&thread1, NULL, mqtt_publish_thread, client);
    if(res != 0) {
        MQTT_LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    while (1) {
        sleep(100);
    }
}
```

## 使用到的API

- 申请一个MQTT客户端

    ```c
    mqtt_client_t *client = NULL;
    client = mqtt_lease();
    ```


- mqtt客户端配置，主要是配置**mqtt_client_t**结构的相关信息，如果没有指定初始化参数，则系统会提供默认的参数。但连接部分的参数则必须指定，比如连接的端口号、云服务器的地址或者域名、用户名、密码，这些信息都是百度云平台得到的。

    ```c
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "j6npr4w.mqtt.iot.gz.baidubce.com");
    mqtt_set_client_id(client, random_string(10));
    mqtt_set_user_name(client, "j6npr4w/mqtt-client-dev");
    mqtt_set_password(client, "lcUhUs5VYLMSbrnB");
    mqtt_set_clean_session(client, 1);
    ```

- 连接服务器并建立mqtt会话。

    ```c
    mqtt_connect(&client);
    ```

- 订阅主题，字符串类型的**主题**（支持通配符"#" "+"），主题的**服务质量**，以及收到报文的**回调处理函数**，如不指定则有默认处理函数，订阅主题的处理方式是异步处理的，topic1_handler则是当收到服务器下发的数据内容时调用的回调函数。

    ```c
    mqtt_subscribe(&client, "topic1", QOS0, topic1_handler);
    ```

- 创建一个发布主题的线程，并且发布主题数据，指定字符串类型的**主题**（支持通配符），要发布的消息（包括**服务质量**、**消息主体**）。

    ```c
    mqtt_message_t msg;
    msg.payload = (void *) buf;
    msg.qos = 0;

    mqtt_publish(&client, "topic1", &msg);
    ```

**上一篇**：[mqttclient设计与实现方式](./mqtt-design.md)

**下一篇**：[mqttclient连接到OneNET云平台](./mqtt-onenet.md)
