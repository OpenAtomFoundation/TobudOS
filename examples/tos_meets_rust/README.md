# TencentOS Tiny meets Rust (甲醛检测器)

## 目录介绍

项目目录和各自的作用如下所示

    +-- app/: Rust 应用程序
    +-- BSP/: borad support package, 嵌入式板子支持工具包
        +-- Hardware/
            +-- CH20/
            +-- OLED/
            +-- PM2D5/
        +-- Inc/: BSP 头文件
        +-- Src/
            +-- main.c: 板子启动后, 从这里的 main 函数开始执行
    +-- docs/: 相关文档
    +-- libs/: Rust 插桩文件
    +-- toolchain/: 工具链编译配置
    +-- TOS_CONFIG/
        +-- tos_config.h: tos 配置
        +-- _user_config: 用户信息配置
    +-- CMakeLists.txt: 整个项目的 cmake 文件
    +-- README.md: 项目概览文档
    +-- tosglue.c: Rust 和 TencentOS 的胶水文件

板子启动后会自动执行 BSP/Src/main.c 中的 main 函数, 在 main 函数中会调用用户定义的如下函数

    void application_entry_rust();

这个函数原来是 `application_entry`, 集成 Rust 后修改为 `application_entry_rust` 了.

除了入口函数, 需要有底层的中断服务实现, 位于 `BSP/Src/stm32g0xx_it.c` 中.

## 编译运行

- 配置腾讯云

    到腾讯云物联网开发平台 https://cloud.tencent.com/product/iotexplorer 注册一个新产品.

    新建产品后导入模板数据如下

        {
            "version": "1.0",
            "profile": {
                "ProductId": "BDDSF87WEA",
                "CategoryId": "1"
            },
            "properties": [
                {
                    "id": "ch20_ppm_value",
                    "name": "甲醛浓度值",
                    "desc": "",
                    "mode": "r",
                    "define": {
                        "type": "float",
                        "min": "0",
                        "max": "2",
                        "start": "0",
                        "step": "0.001",
                        "unit": "ppm(mg/m3)"
                    },
                    "required": false
                }
            ],
            "events": [],
            "actions": []
        }

    然后新建设备, 新建之后能够得到三个信息: 设备名称, 设备密钥, 产品 ID,
    这三个信息需要写入到 TencentOS 固件中.

    配置完成后, 将甲醛传感器与底板连接, 根据板子和底板的 5v 和 GND 接口对应关系, 将底板连接到板子上,
    然后根据传感器的 Rx 和 Tx 口将传感器和板子底板连接起来.

- 下载源码

    下载 TencentOS 源码

        git clone https://github.com/Tencent/TencentOS-tiny.git

    进入 `examples/tos_meets_rust` 目录, 将 `TOS_CONFIG/_user_config.h` 复制一份为 `TOS_CONFIG/user_config.h`,
    写入设备信息和 WiFI 信息.

    新建系统环境变量 TOS_SRC_ROOT, 其值为 TencentOS Tiny 源码的绝对路径.

- 基础依赖安装

    - ST-LINK 驱动安装, 参考 [stlink](./docs/stlink.md)

    - CH34X 驱动安装, 参考 [CH34X](./docs/ch34x.md)

    - kermit 串口工具安装, 参考 [kermit](./docs/kermit.md)

- 工具链安装

    编译需要的 arm-none-eabi 工具链, eabi 的含义是 Embedded Application Binary Interface, 
    不同发行版安装方式不一样

    - ubuntu

            sudo apt-get install -y gcc-arm-none-eabi

    - archlinux:

            sudo pacman -S arm-none-eabi-gcc arm-none-eabi-newlib

    这里默认你已经安装了 rust 工具链, 如果你的 rustc 版本低于 1.47.0 则切换到 nightly 版本,
    切换命令如下

        rustup default nightly

    如果版本大于等于 1.47.0, 则不用切换, 接着安装支持板子的 rust 工具链

        rustup target add thumbv6m-none-eabi

- 刷入 WiFi 固件, 参见 [esp](./docs/flash-esp.md)

- 刷入系统

    连接 STLINK 后, 执行如下命令

        bash build.sh

    刷入完成后, 登录 kermit, 然后按下板子上的 reset 按键, 程序就运行了, 串口会输出 WIFI CONNECTED 之类的,
    腾讯云上显示设备上线.

## Rust 集成

Rust 集成原理参见 [rust](./docs/rust.md)

## 其他

1. 如果你在 mac 平台上开发, 你可以参考文档 [setup.mac.md](./docs/setup.mac.md) 来搭建开发环境.

2. 有一份简短的关于本 demo 的介绍 PPT, 位于[这里](./docs/presentation.pdf), 有需要可以打开查看.
