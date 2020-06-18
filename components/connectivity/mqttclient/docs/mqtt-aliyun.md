# 连接到阿里云物联

既然懂得专门连接百度天工物接入，那么连接阿里云物联其实也是一样的，因为都是基于MQTT协议进行通信的，首先打开阿里云物联：https://iot.console.aliyun.com/product，创建账号并登陆，而且阿里云物联是需要通过实名认证才能使用的。

相对于百度天工物接入，阿里云物联的安全性更好，因为即使是客户端连接服务器都需要各种验证，还使用哈希加密算法进行加密。


## 使用阿里云物联

下面开始使用阿里云物联，首先在“产品”信息中创建一个产品，每个产品下允许有多个设备，产品就是设备的集合，通常是一组具有相同功能定义的设备集合。例如：产品指同一个型号的产品，设备就是该型号下的某个设备。操作步骤如下：

### 创建产品

![mqtt-ali001](http://qiniu.jiejie01.top/mqtt-ali001.png)

- 填写产品信息，如产品名称，所属类品选择自定义类品、节点类型选择直连设备、联网方式选择wifi、数据格式选择透传的方式、认证方式选择设备秘钥，最后点击保存，这样子一个产品就创建完成。

![mqtt-ali002](http://qiniu.jiejie01.top/mqtt-ali002.png)

### 添加设备

创建完产品后选择添加设备，并填写设备的名字。

![mqtt-ali003](http://qiniu.jiejie01.top/mqtt-ali003.png)

![mqtt-ali004](http://qiniu.jiejie01.top/mqtt-ali004.png)

- 在设备添加完成后，要保存设备的证书，后续会使用到，比如此时的设备证书是:

    ```json
    { 
        "ProductKey": "a1w7XupONEX",
        "DeviceName": "test1", 
        "DeviceSecret": "2H1FiqEvyovF8bdckg6RjBcO2LAgGhwu" 
    }
    ```

![mqtt-ali005](http://qiniu.jiejie01.top/mqtt-ali005.png)

- 当然，我们也能在设备下查看这些信息，以及其他的一些信息。

![mqtt-ali006](http://qiniu.jiejie01.top/mqtt-ali006.png)

![mqtt-ali007](http://qiniu.jiejie01.top/mqtt-ali007.png)

### 自定义主题

最后我们定义一个主题（Topic类列表），让设备能对这个主题进行订阅或者发布操作，回到“产品”选项，选择“Topic类列表”，定义“Topic类列表”，再填写“Topic类列表”的信息即可，注意选择设备的操作权限“发布和订阅”，这点很重要，如果没有权限，设备是无法对这个主题进行操作的，具体过程如图所示，至此，一个产品与设备就创建完成了。

![mqtt-ali008](http://qiniu.jiejie01.top/mqtt-ali008.png)

![mqtt-ali009](http://qiniu.jiejie01.top/mqtt-ali009.png)

![mqtt-ali010](http://qiniu.jiejie01.top/mqtt-ali010.png)

- 回到设备页面，可以看到设备的主题已经有了刚刚自定义定义的topic，权限是可订阅与可发布。

![mqtt-ali011](http://qiniu.jiejie01.top/mqtt-ali011.png)

## 测试连接

在创建完成后，可以通过MQTT软件来测试一下能否正常连接，在这一步之前必须已在物联网平台控制台中创建产品和设备，并获取设备证书信息（ProductKey、DeviceName和DeviceSerect）。

其实连接是与百度天工差不多的，只不过这里的配置连接的信息比百度云天工物接入麻烦很多，因为考虑了安全的问题，需要进行加密验证。

- 首先回到设备页面，查看设备相关的信息，比如区域（这很重要，后续使用到的）、设备的证书等内容。

![mqtt-ali012](http://qiniu.jiejie01.top/mqtt-ali012.png)

![mqtt-ali013](http://qiniu.jiejie01.top/mqtt-ali013.png)

- 通过阿里云的在线签名工具[https://jiejietop.gitee.io/aliyun/index.html](https://jiejietop.gitee.io/aliyun/index.html) 。 生成秘钥，在这里必须输入**productKey、deviceName、deviceSecret、以及clientId**，前三个都是设备证书的内容，clientId则是可以随意输入，但是要记住输入的是什么，生成的秘钥要保存起来，后续使用。

![mqtt-ali014](http://qiniu.jiejie01.top/mqtt-ali014.png)

- 打开MQTTX软件，在软件中填写名称、Client ID，选择MQTT协议，填写项目的地址信息、端口号、用户名以及密码，然后点击连接。

![mqtt-ali015](http://qiniu.jiejie01.top/mqtt-ali015.png)

这个参数看起来很简单，但是它的组成却不简单，下面具体介绍一下参数是怎么来的，具体见：

- 名称：自定义名称

- Client ID：格式：`${clientId}|securemode=3,signmethod=hmacsha1|`。${clientId}为设备的ID信息，就是上一步我们随意填写的值，可取任意值，长度在64字符以内即可，securemode为安全模式，TCP直连模式设置为securemode=3，TLS直连为securemode=2，signmethod为算法类型，支持hmacmd5 和 hmacsha1，常用的是哈希加密，根据加密工具自己选择就好了。

- 服务器地址：填写阿里云的服务器地址，是有格式的，格式如下：`${YourProductKey}.iot-as-mqtt.${region}.aliyuncs.com`。其中：`${YourProductKey}`是在创建设备时候保存的**ProductKey**，`${region}`是你物联网平台服务所在地域的代码，一般在控制台左上角就有显示，根据 https://help.aliyun.com/document_detail/40654.html? 替换物联网平台服务所在地域的代码即可，当然也可以参考下表：

1. 国内地区及Region ID

    | 地域名称 | 所在城市 | Region ID | 可用区数量 |
    | -- | -- | -- | -- |
    | 华北 1 | 青岛 | cn-qingdao | 2 |
    | 华北 2 | 北京 | cn-beijing | 8 |
    | 华北 3 | 张家口 | cn-zhangjiakou | 3 |
    | 华北 5 | 呼和浩特 | cn-huhehaote | 2 |
    | 华东 1 | 杭州 | cn-hangzhou | 8 |
    | 华东 2 | 上海 | cn-shanghai | 7 |
    | 华南 1 | 深圳 | cn-shenzhen | 5 |
    | 华南 2 | 河源 | cn-heyuan | 2 |
    | 西南 1 | 成都 | cn-chengdu | 2 |

2. 国外地区及Region ID

    | 地域名称 | 所在城市 | Region ID | 可用区数量 |
    | -- | -- | -- | -- |
    | 中国香港 | 香港 | cn-hongkong | 2 |
    | 亚太东南 1 | 新加坡 | ap-southeast-1 | 3 |
    | 亚太东南 2 | 悉尼 | ap-southeast-2 | 2 |
    | 亚太东南 3 | 吉隆坡 | ap-southeast-3 | 2 |
    | 亚太东南 5 | 雅加达 | ap-southeast-5 | 2 |
    | 亚太南部 1 | 孟买 | ap-south-1 | 2 |
    | 亚太东北 1 | 东京 | ap-northeast-1 | 2 |
    | 美国西部 1 | 硅谷 | us-west-1 | 2 |
    | 美国东部 1 | 弗吉尼亚 | us-east-1 | 2 |
    | 欧洲中部 1 | 法兰克福 | eu-central-1 | 2 |
    | 英国（伦敦） | 伦敦 | eu-west-1 | 2 |
    | 中东东部 1 | 迪拜 | me-east-1 | 1 |

- 用户名：格式：`${YourDeviceName}&${YourPrductKey}` 由设备名DeviceName、符号（&）和产品ProductKey组成，这些信息在创建设备的时候都已经保存的。

- 密码：在线签名工具生成的密码。注意的是clientId是填写前面自己定义的clientId即可。


在连接成功后，可以添加订阅的主题名字，向指定的主题发送内容：

![mqtt-ali016](http://qiniu.jiejie01.top/mqtt-ali016.png)


## 更新

为了确保我们安装的软件包的版本是最新版本，让我们使用apt命令更新本地apt包索引和升级系统：

```bash
sudo apt-get update
sudo apt-get -y upgrade
```

## 手动安装相关的依赖包

这些依赖包是使用mqttclient库去连接阿里云时必须要安装的。

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

接着到github拉取这个仓库，仓库非常小，下载下来是很快的，注意，这里也需要拉取到本地。

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

- test文件夹：一些测试的代码，比如我们连接阿里云的时候，就会使用test文件夹的阿里云平台的测试代码。

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

当编译完成后，在`./build/bin`目录下会出现多个可执行文件，在`./build/lib`目录下会出现相关的动态库文件，具体如下：

```bash
➜  mqttclient git:(master) ls build/bin
ali  baidu  emqx  onenet

➜  mqttclient git:(master) ls build/lib
libarch.a  libcommon.a  libmbedtls.a  libmqtt.a  libmqttclient.a  libnetwork.a  libplatform.a  libsalof.a  libwrapper.a
```

我们直接运行`./build/bin/ali`这个可执行文件：

```bash
➜  mqttclient git:(master) ./build/bin/ali 

welcome to mqttclient test...

[I] >> [TS: 1590464316] /home/jiejie/github/mqttclient/mqttclient/mqttclient.c:948 mqtt_connect_with_results()... mqtt connect success...
[I] >> [TS: 1590464318] /home/jiejie/github/mqttclient/mqttclient/mqttclient.c:1337 mqtt_list_subscribe_topic()...[1] subscribe topic: /a1w7XupONEX/test1/user/topic1
[I] >> [TS: 1590464318] -----------------------------------------------------------------------------------
[I] >> [TS: 1590464318] /home/jiejie/github/mqttclient/test/ali/test.c:24 topic1_handler()...
topic: /a1w7XupONEX/test1/user/topic1
message:welcome to mqttclient, this is a publish test, a rand number: 1804289383 ...
[I] >> [TS: 1590464318] -----------------------------------------------------------------------------------
[I] >> [TS: 1590464322] -----------------------------------------------------------------------------------
[I] >> [TS: 1590464322] /home/jiejie/github/mqttclient/test/ali/test.c:24 topic1_handler()...
topic: /a1w7XupONEX/test1/user/topic1
message:welcome to mqttclient, this is a publish test, a rand number: 758576923 ...
[I] >> [TS: 1590464322] -----------------------------------------------------------------------------------
```

此时表示已经连接上阿里云物联了，并且实现了数据的收发，你可以到阿里云平台的日志服务中观看，可以看到设备的日志信息是正常的。

![mqtt-ali017](http://qiniu.jiejie01.top/mqtt-ali017.png)

## 代码

那么这个测试的例程代码如下，位于`./test/ali/test.c`：

```c
/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime: 2020-06-08 20:40:47
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "mqttclient.h"

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
        mqtt_publish(client, "/a1w7XupONEX/test1/user/topic1", &msg);
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
    
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "a1w7XupONEX.iot-as-mqtt.cn-shanghai.aliyuncs.com");
    mqtt_set_client_id(client, "123456|securemode=3,signmethod=hmacsha1|");
    mqtt_set_user_name(client, "test1&a1w7XupONEX");
    mqtt_set_password(client, "A9EFF34CCA05EABAE560373CBED3E43AC88956CF");
    mqtt_set_clean_session(client, 1);

    mqtt_connect(client);
    
    mqtt_subscribe(client, "/a1w7XupONEX/test1/user/topic1", QOS0, topic1_handler);
    
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
    mqtt_set_host(client, "a1w7XupONEX.iot-as-mqtt.cn-shanghai.aliyuncs.com");
    mqtt_set_client_id(client, "123456|securemode=3,signmethod=hmacsha1|");
    mqtt_set_user_name(client, "test1&a1w7XupONEX");
    mqtt_set_password(client, "A9EFF34CCA05EABAE560373CBED3E43AC88956CF");
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

**上一篇**：[mqttclient连接到OneNET云平台](./mqtt-onenet.md)

**下一篇**：待完善