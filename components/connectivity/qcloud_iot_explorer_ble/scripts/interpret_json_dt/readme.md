## 介绍
使用`python`脚本将后台生成的`json`格式的数据模版文件转为`c`代码文件。
极大的减少了用户的开发工作，同时保证了终端设备，网关设备上数据定义的一致性。
请使用`python3`解释器运行。

## 目录结构
```c
interpret_json_dt
├─config                                # 配置文件目录
│  └─dt.conf                            # INI配置文件
├─src                                   # 脚本文件目录
│  ├─dt_fixed_content                   # 固定代码文件目录 
│  │   └─dt_ble_action                  # ble action部分固定代码 
│  │   └─dt_ble_event                   # ble event部分固定代码
│  │   └─dt_ble_property                # ble proerty部分固定代码 
│  │   └─dt_ble_prototype               # ble 函数原型
│  │   └─dt_gateway_action              # gateway action部分固定代码 
│  │   └─dt_gateway_event               # gateway event部分固定代码 
│  │   └─dt_gateway_property            # gateway property部分固定代码 
│  │   └─dt_gateway_prototype           # gateway 函数原型   
│  ├─interpret_dt_ble.py                # 转换json脚本生成ble sdk代码
│  ├─interpret_dt_gateway.py            # 转换json脚本生成网关代码   
│  ├─example.json                       # 示例文件           
```

## 代码生成原理
采取`动态生成 + 固定写入`的方式来生成代码。
* 动态生成：通过脚本解析`json`文件，将数据模版转换为对应的代码
* 固定写入：数据模版的操作函数是固定不变的，从静态文件读取写入，达到简化脚本的目的

### 头文件生成
1. 按照`LLSync协议`定义写入公共定义，包括数据类型定义，消息类型定义等
2. 解析`json`文件，将字符串`id`转换为枚举类型`id`
3. 将每个`id`对应的值根据其类型进行转换
   * 对于枚举类型，将其枚举值转换为枚举类型
   * 对于整数类型或浮点数类型，将其最大值，最小值，起始值，步进转换为宏定义
   * 对于字符串类型，将其最大长度，最小长度转换为宏定义
4. 写入不同数据类型的结构体定义
5. 写入不同数据类型的函数声明，函数声明从`dt_gateway_prototype`中读

### BLE C文件生成
1. 解析`json`文件，根据`id`生成其操作函数，**操作函数需要用户按照需求实现**
2. 解析`json`文件，生成数据模版的结构数组
3. 读取静态文件，写入固定操作函数

### 网关C文件生成
1. 解析`json`文件，生成数据模版的结构数组
2. 读取静态文件，写入固定操作函数

## 使用方法
### BLE SDK
1. 从物联网平台下载数据模版`json`文件
2. 执行`python3 interpret_dt_ble.py <your_json_file>`来生成对应的数据模版文件
3. 按照数据特性实现`ble_qiot_template.c`中的操作函数
4. 将生成文件拷贝到`data_template`目录编译即可

### 网关
1. 从物联网平台下载数据模版`json`文件
2. 执行`python3 interpret_dt_gateway.py <your_json_file>`来生成对应的数据模版文件
3. 将生成文件拷贝SDK编译即可。