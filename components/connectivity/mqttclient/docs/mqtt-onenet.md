# mqttclient连接到OneNET云平台

有了前面连接百度云的经验，废话不多说，直接使用OneNET，OneNET平台应该是最开放，对开发者最友好的平台了。

## 使用OneNET
首先注册与登陆OneNET，然后进入开发者中心：https://open.iot.10086.cn/develop/global/product/#/public?protocol=3&other=1， 选择公有协议产品，点击“添加产品”，填写产品相关的信息，联网方式选择wifi（其实我们是以太网，但是没有这个选项，那就选择wifi，没啥影响的），接入协议必须选择MQTT，操作系统选择“linux”，运营商这个随意选择，具体见：

![mqtt-onenet001](http://qiniu.jiejie01.top/mqtt-onenet001.png)


在添加产品完毕，继续添加设备，填写设备的相关信息，鉴权信息就是登陆密码，不过需要注意的是，这个鉴权信息在产品内是唯一的，一般推荐使用产品序列号，可作为设备登录参数之一，不同协议设备可能鉴权信息的参数不一致，不过现在是做实验，选择一个好记的即可，数据保密性要选择公有，除此之外还剩下一些设备相关的信息，就不过多赘述，具体见图：

![mqtt-onenet002](http://qiniu.jiejie01.top/mqtt-onenet002.png)

![mqtt-onenet003](http://qiniu.jiejie01.top/mqtt-onenet003.png)

![mqtt-onenet004](http://qiniu.jiejie01.top/mqtt-onenet004.png)

经过上面的步骤，我们就创建完成一个产品了，此时可以去测试连接它。

## 测试连接

OneNET的数据交互做的很好，它支持动态创建主题（除系统主题外），即不用我们在平台上创建任何的主题，只需要随意订阅某个主题即可，同一个产品下，即使是不同设备之间的主题之间的消息是共享的，简单来说，我们在开发板上可以随意向某个主题发起订阅请求，也可以向这个主题发布消息，而同一产品的其他设备如果订阅了这个主题，那么将收到开发板发布的消息数据，这样子更加方便嵌入式的开发者，只不过这样子的信息安全就没有阿里云物联那么好。

我们可以在这个网页中去查看服务器的IP地址：https://open.iot.10086.cn/doc/multiprotocol/book/problem/platformaddress.html， 对于服务器的地址，onenet分为了不同的地区平台，不过我们一般正常使用的都是中心平台。

![mqtt-onenet005](http://qiniu.jiejie01.top/mqtt-onenet005.png)

现在我们打开MQTT软件，进行连接测试，与前面的实验操作是一样的，配置好相关信息，即可，这些信息都可以在平台上找到，需要注意的是服务器地址是183.230.40.39；端口号是6002，这与我们常见的1883是不一样的；Client ID是设备ID，在设备列表中是可以找到的；用户名是产品ID，在产品概况页面中可以找到；密码就是创建设备时候的鉴权信息，具体见：

![mqtt-onenet006](http://qiniu.jiejie01.top/mqtt-onenet006.png)

![mqtt-onenet007](http://qiniu.jiejie01.top/mqtt-onenet007.png)

![mqtt-onenet008](http://qiniu.jiejie01.top/mqtt-onenet008.png)


接下来我们可以通过MQTT软件来测试一下能否正常连接，在这一步之前必须已在物联网平台控制台中创建产品和设备，并获取设备相关的信息。

其实连接是与百度天工差不多的，直接填写相关的内容即可：

![mqtt-onenet009](http://qiniu.jiejie01.top/mqtt-onenet009.png)

配置好就可以连接，然后随便订阅一个主题，因为OneNet平台支持动态创建主题（除系统主题外），所以对我们来说是非常方便的，直接订阅就好了，然后再用客户端进行发布消息，如果收到消息，表明通信成功，

![mqtt-onenet010](http://qiniu.jiejie01.top/mqtt-onenet010.png)

![mqtt-onenet011](http://qiniu.jiejie01.top/mqtt-onenet011.png)

回到OneNet的设备列表界面，可以看到刚刚创建的设备是处于在线状态的：

![mqtt-onenet012](http://qiniu.jiejie01.top/mqtt-onenet012.png)


## 手动安装相关的依赖包

这些依赖包是使用mqttclient库去连接OneNet云时必须要安装的。

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

接着到github拉取这个仓库，仓库非常小，下载下来是很快的，注意，这里也需要拉取下来。

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

- test文件夹：一些测试的代码，比如我们连接OneNet云的时候，就会使用test文件夹的OneNet云平台的测试代码。

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

我们直接运行`./build/bin/onenet`这个可执行文件：

```bash
➜  mqttclient git:(master) ✗ ./build/bin/onenet 

welcome to mqttclient test...

[I] >> [TS: 1590547112] /home/jiejie/github/mqttclient/mqttclient/mqttclient.c:948 mqtt_connect_with_results()... mqtt connect success...
[I] >> [TS: 1590547113] /home/jiejie/github/mqttclient/mqttclient/mqttclient.c:16 default_msg_handler()...
topic: temp_hum, qos: 0, 
message:welcome to mqttclient, this is a publish test, a rand number: 1804289383 ...
[I] >> [TS: 1590547116] /home/jiejie/github/mqttclient/mqttclient/mqttclient.c:16 default_msg_handler()...
topic: temp_hum, qos: 0, 
message:welcome to mqttclient, this is a publish test, a rand number: 1837236902 ...
```

此时表示已经连接上OneNet云了，并且实现了数据的收发。

## 代码

那么这个测试的例程代码如下，位于`./test/onenet/test.c`：

```c
/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-04-18 12:37:34
 * @LastEditTime: 2020-06-08 20:32:33
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "mqttclient.h"

extern const char *test_ca_get();


static void interceptor_handler(void* client, message_data_t* msg)
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
    
    msg.qos = 0;
    msg.payload = (void *) buf;
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

    mqtt_set_port(client, "6002");
    mqtt_set_host(client, "183.230.40.39");
    mqtt_set_client_id(client, "599908192");
    mqtt_set_user_name(client, "348547");
    mqtt_set_password(client, "mqttclienttest1");
    mqtt_set_clean_session(client, 1);
    
    mqtt_connect(client);
    
    mqtt_subscribe(client, "topic1", QOS0, NULL);

    mqtt_set_interceptor_handler(client, interceptor_handler);     // set interceptor handler
    
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
    mqtt_set_port(client, "6002");
    mqtt_set_host(client, "183.230.40.39");
    mqtt_set_client_id(client, "599908192");
    mqtt_set_user_name(client, "348547");
    mqtt_set_password(client, "mqttclienttest1");
    mqtt_set_clean_session(client, 1);
    ```

- 连接服务器并建立mqtt会话。

    ```c
    mqtt_connect(&client);
    ```

- 订阅主题，字符串类型的**主题**（支持通配符"#" "+"），主题的**服务质量**，以及收到报文的**回调处理函数**，如不指定则有默认处理函数，订阅主题的处理方式是异步处理的，此处设置为NULL则表示使用默认的回调处理函数。

    ```c
    mqtt_subscribe(client, "topic1", QOS0, NULL);
    ```

- 创建一个发布主题的线程，并且发布主题数据，指定字符串类型的**主题**（支持通配符），要发布的消息（包括**服务质量**、**消息主体**）。

    ```c
    mqtt_message_t msg;
    msg.payload = (void *) buf;
    msg.qos = 0;

    mqtt_publish(&client, "topic1", &msg);
    ```

- 由于onenet的设备会自动订阅系统主题，而用户是不能直接订阅的，比如我们在线调试的时候，他会下发一些系统主题，那么我们需要接收这些主题，就需要设置拦截器的处理函数，去拦截它们并且通过回调函数上报到应用层，设置如下：

    ```c
    mqtt_set_interceptor_handler(&client, interceptor_handler); 
    ```

我们在设备列表页面，点击下方命令，下发一些数据到开发板上：

![mqtt-onenet013](http://qiniu.jiejie01.top/mqtt-onenet013.png)

![mqtt-onenet014](http://qiniu.jiejie01.top/mqtt-onenet014.png)

![mqtt-onenet015](http://qiniu.jiejie01.top/mqtt-onenet015.png)

在开发板上可以看到这些数据内容，可以看到他的主题是`$creq/`开头的，而且接下来的内容应该是经过平台加密生成的，是随机的，我们也无法主动去订阅，只能通过拦截器去接收这些数据内容。

```bash
[I] >> [TS: 1590548249] -----------------------------------------------------------------------------------
[I] >> [TS: 1590548249] /home/jiejie/github/mqttclient/test/onenet/test.c:26 interceptor_handler()...
topic: $creq/f6132232-bbcd-5b1e-9b9a-806da959ce80
message:hello world !
[I] >> [TS: 1590548249] -----------------------------------------------------------------------------------

[I] >> [TS: 1590548023] -----------------------------------------------------------------------------------
[I] >> [TS: 1590548023] /home/jiejie/github/mqttclient/test/onenet/test.c:24 interceptor_handler()...
topic: $creq/5d2670bb-a9a4-5bc9-93d5-107246135af8
message:hello world !
[I] >> [TS: 1590548023] -----------------------------------------------------------------------------------
```


**上一篇**：[mqttclient连接到百度天工物接入](./mqtt-baidu.md)

**下一篇**：[mqttclient连接到阿里云物联网平台](./mqtt-aliyun.md)
