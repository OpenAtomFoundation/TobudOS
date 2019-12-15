## 简介
本文介绍基于物联开发平台 IoT Explorer 创建的数据模板如何生成模板代码。

## 数据模板创建
参阅[产品定义](https://cloud.tencent.com/document/product/1081/34739?!preview&!editLang=zh#.E6.95.B0.E6.8D.AE.E6.A8.A1.E6.9D.BF)创建数据模板

## 数据模板描述文件导出
数据模板描述文件是一个 JSON 格式的文件，描述了产品定义的属性、事件及其他信息，在平台导出此json文件，如下图示:
![](https://main.qcloudimg.com/raw/0951d7c3f540ca716442e08651a0efa5.jpg)

## 数据模板模板代码生成
将下载的json文件拷贝到tools目录，执行./codegen.py -c xx/config.json -d  ../targetdir/ 命令,则会根据json文件在target目录生成所定义产品的数据模板及事件的配置文件,将这个生成的配置文件拷贝到data_template_sample.c的同级目录，data_template_sample.c示例了通用的数据模板处理框架，可以基于此框架添加业务逻辑。智能灯示例light_data_template_sample.c即是基于此框架的场景示例。

```
./codegen.py -c light.json -d ../samples/data_template/
加载 light.json 文件成功
文件 ../samples/data_template/data_config.c 生成成功
文件 ../samples/data_template/events_config.c 生成成功
```