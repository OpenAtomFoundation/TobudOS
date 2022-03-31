# 腾讯 AI SIG 早期计划（第一版）

Author: derekduke

Date:   2022/3/31
## 1. 目标效果
命令行：python AI_deploy.py paramA... paramB...

可视化：QT界面或Web选择（拖拽）文件

input参数：
- model name 
- bsp filepath

output结果:
-  一个可直接编译的定制化AI组件包（包含应用模板）
## 2. 开发任务分类
### 基于tos的 AI 框架开发
- 上位机模型验证框架开发（tflite格式）
- 上位机模型自动部署工具开发
- 下位机AI标准库开发
### 基于tos的 AI demo 开发
#### 平台
- stm32L4、STM32H7
- NXP RT1062
- K210
- imx6ull
- stm32mp157
- ...
#### 模型
- 行人识别
- 行人探测
- 人脸识别
- 口罩检测
- 语音检测
- 流量预测
- ...
### 基于tos的 AI 后端组件开发
- tensorflow lite micro
- cubeai
- nnom
- tengine lite
- mindspore lite
- ...
### 宣传演示
- demo视频、博客、推文 
- ...

## 3. 阶段目标
### 第一个完整demo（暂定行人识别和手写数字识别）
产出：
- [ ] stm32L496 BSP
- [ ] AI_verify.py
- [ ] AI_deploy.py
- [ ] TFLite-micro组件
- [ ] 环境配置材料
- [ ] 一篇用户体验教程