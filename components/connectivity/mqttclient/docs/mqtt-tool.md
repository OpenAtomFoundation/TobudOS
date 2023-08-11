
# mqttclient 代码生产工具介绍

mqttclient 代码生产工具主要是用于配置 MQTT 的参数，并且生成相应的代码，因为是可视化的配置，极易使用。

地址：[https://jiejietop.gitee.io/mqtt/index.html](https://jiejietop.gitee.io/mqtt/index.html)

![在线代码生成工具](http://qiniu.jiejie01.top/mqtt-tool.png)

与此同时改工具页面还包含了 mqttclient 的 API 接口介绍及示例，通过它们就能知道这个工具应该配置了什么内容。

![mqtt-tool-api](http://qiniu.jiejie01.top/mqtt-tool-api.png)

# 连接参数配置

首先连接参数代表着 MQTT 客户端应该如何连接到服务器，建立 MQTT 会话应该是如何配置，这些就是连接参数

- 与服务器相关的参数有：
  
  - 服务器地址。

  - 服务器端口号。

  - 服务器 CA 证书（如果有 TLS 加密的话）。

- 建立 MQTT 连接需要的参数有：
  
  - MQTT 协议版本。
  
  - 用户名。
  
  - 密码。
  
  - 客户端 ID。
  
  - 配置是否清除会话，默认清除会话。
 
  - 心跳时间间隔，默认是 50 秒。
  
  - 是否使用遗嘱。
  
  - 是否保留遗嘱消息。
  
  - 设置遗嘱主题。
  
  - 设置遗嘱消息的服务质量等级。
  
  - 设置遗嘱消息的内容。

- 与与客户端资源相关的配置：
  
  - 设置命令的超时，它主要是用于 socket 读写超时，默认是 5000 毫秒。
  
  - 读缓冲区大小，默认是 1024。
  
  - 写缓冲区大小，默认是 1024。

![mqtt-tool-connect](http://qiniu.jiejie01.top/mqtt-tool-connect.png)

# 订阅主题相关的代码配置

此配置用于配置 MQTT 客户端订阅的主题信息，指定订阅主题名字，服务质量等级以及当收到来自这个主题消息时候的回调处理函数（可以为 NULL），mqttclient 代码生成工具支持动态添加多个主题，满足绝大部分的日常需求。

![mqtt-tool-sub](http://qiniu.jiejie01.top/mqtt-tool-sub.png)

# 发布消息相关的代码配置

mqttclient 代码生成工具支持动态添向多个主题发布消息，满足绝大部分的日常需求。只需指定要发布消息的主题名字、服务质量等级、以及发布的消息内容即可。

![mqtt-tool-pub](http://qiniu.jiejie01.top/mqtt-tool-pub.png)

# 生成代码

在配置完成后，点击下方的生成代码按钮，即可生成对应的配置代码。当然，你也可以点击导入模板，使用模板默认的配置来生成代码。

![mqtt-tool-code](http://qiniu.jiejie01.top/mqtt-tool-code.png)

直接将生成的代码复制到你的 main.c 文件即可编译运行。


**上一篇**：[MQTT 通信过程](./mqtt-communication.md)

**下一篇**：[mqttclient 配置及裁剪工具](./mqtt-config.md)
