# STM32 甲醛检测器安装教程

本文所用系统为 Ubuntu 20.04.1, 以下教程所用到的系统固件源码以本仓库 template 中的模板代码为准,
有任何疑问欢迎提 issues.

## 教程

- 连接 ST-LINK (STv2)  和开发板

    STv2 上有一个示意图标有各个 Pin 的含义, Pin 周围有一个缺口,
    缺口对应的一排 Pin 对应与示意图外侧的一列, STv2 和开发板接口连接对应关系如下表所示.

    |STv2 接口      |开发板接口   |
    | ------------- |-------------|
    |    3.3v       |    3v3      |
    |    SWDIO      |    DIO      |
    |    SWCLK      |    CLK      |
    |    GND        |    GND      |

- 安装 STv2 驱动

    ST-LINK 的驱动源码地址为 https://github.com/stlink-org/stlink.

    安装编译依赖

        sudo apt install clang build-essential cmake libusb-1.0-0 libusb-1.0-0-dev

    下载源码并编译

        git clone https://github.com/stlink-org/stlink
        cd stlink
        make release

    安装到 $HOME/.usr/stlink 目录中

        cd build/Release && make install DESTDIR=$HOME/.usr/stlink

    安装后 $HOME/.usr/stlink 结构如下

        ➜  Release git:(develop) tree ~/.usr/stlink/
        /home/m9/.usr/stlink/
        ├── etc
        │   └── modprobe.d
        │       └── stlink_v1.conf
        ├── lib
        │   └── udev
        │       └── rules.d
        │           ├── 49-stlinkv1.rules
        │           ├── 49-stlinkv2-1.rules
        │           ├── 49-stlinkv2.rules
        │           └── 49-stlinkv3.rules
        └── usr
            └── local
                ├── bin
                │   ├── st-flash
                │   ├── st-info
                │   └── st-util
                ├── include
                │   └── stlink
                │       ├── backend.h
                │       ├── chipid.h
                │       ├── commands.h
                │       ├── flash_loader.h
                │       ├── libusb_settings.h
                │       ├── logging.h
                │       ├── md5.h
                │       ├── reg.h
                │       ├── sg.h
                │       ├── stlink.h
                │       ├── stm32.h
                │       ├── usb.h
                │       └── version.h
                ├── lib
                │   ├── libstlink.a
                │   ├── libstlink.so -> libstlink.so.1
                │   ├── libstlink.so.1 -> libstlink.so.1.6.1
                │   └── libstlink.so.1.6.1
                └── share
                    └── man
                        └── man1
                            ├── st-flash.1
                            ├── st-info.1
                            └── st-util.1

    现在需要通过 udev 设置 USB 访问权限, 进入到 stlink 源码根目录执行如下操作

        sudo cp -a config/udev/rules.d/* /etc/udev/rules.d/
        sudo udevadm control --reload-rules
        sudo udevadm trigger

    现在给电脑插上电路板, 使用 lsusb 查看 USB 设备, 能看到如下输出说明安装成功
    

        ➜  stlink git:(develop) lsusb
        ...
        Bus 002 Device 013: ID 0483:3748 STMicroelectronics ST-LINK/V2
        ...


    在 shell 配置文件如 $HOME/.zshrc 中添加如下环境变量

        STLINK=$HOME/.usr/stlink
        export PATH=${STLINK}/usr/local/bin:${PATH}
        export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${STLINK}/usr/local/lib

    更新库缓存

        sudo ldconfig

    刷新环境变量

        exec $SHELL

    然后查看板子信息

        ➜  ~ st-info --probe
        Found 1 stlink programmers
         serial:     2c5a15000b14324d434d4e00
         hla-serial: "\x2c\x5a\x15\x00\x0b\x14\x32\x4d\x43\x4d\x4e\x00"
         flash:      131072 (pagesize: 2048)
         sram:       36864
         chipid:     0x0460
         descr:      G070/G071/G081


    测试文件读取

        ➜  ~ st-flash --debug read dummy.file 0 256
        st-flash 1.6.1-96-gbf41f14
        2020-09-22T23:56:45 DEBUG common.c: *** looking up stlink version
        2020-09-22T23:56:45 DEBUG common.c: st vid         = 0x0483 (expect 0x0483)
        2020-09-22T23:56:45 DEBUG common.c: stlink pid     = 0x3748
        2020-09-22T23:56:45 DEBUG common.c: stlink version = 0x2
        2020-09-22T23:56:45 DEBUG common.c: jtag version   = 0x23
        2020-09-22T23:56:45 DEBUG common.c: swim version   = 0x7
        2020-09-22T23:56:45 DEBUG common.c: stlink current mode: debug (jtag or swd)
        2020-09-22T23:56:45 DEBUG usb.c: JTAG/SWD freq set to 0
        2020-09-22T23:56:45 DEBUG common.c: *** set_swdclk ***
        2020-09-22T23:56:45 DEBUG common.c: stlink current mode: debug (jtag or swd)
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_jtag_reset ***
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_reset ***
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_write_debug32 0x05fa0004 to 0xe000ed0c
        2020-09-22T23:56:45 DEBUG common.c: Loading device parameters....
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_core_id ***
        2020-09-22T23:56:45 DEBUG common.c: core_id = 0x0bc11477
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_read_debug32 0000000000 at 0xe0042000
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_read_debug32 0x20006460 at 0x40015800
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_read_debug32 0xffff0080 at 0x1fff75e0
        2020-09-22T23:56:45 INFO common.c: G070/G071/G081: 36 KiB SRAM, 128 KiB flash in at least 2 KiB pages.
        2020-09-22T23:56:45 DEBUG common.c: stlink current mode: debug (jtag or swd)
        2020-09-22T23:56:45 DEBUG common.c: stlink current mode: debug (jtag or swd)
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_force_debug_mode ***
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_status ***
        2020-09-22T23:56:45 DEBUG usb.c: core status: 00030003
        2020-09-22T23:56:45 DEBUG common.c:   core status: halted
        2020-09-22T23:56:45 INFO common.c: read from address 0000000000 size 256
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_read_mem32 ***
        2020-09-22T23:56:45 DEBUG common.c: data_len = 256 0x100
         68 5c 00 20 cd 00 00 08 c1 2d 00 08 c7 29 00 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 b7 2f 00 08 00 00 00 00 00 00 00 00 25 01 00 08 b9 2f 00 08 df 00 00 08 00 00 00 00 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 00 00 00 00 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 00 00 00 00 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 df 00 00 08 9d 37 00 08 ad 37 00 08 bd 37 00 08 03 48 85 46 00 f0 56 fd 00 48 00 47 39 58 00 08 68 5c 00 20 04 48 80 47 04 48 00 47 fe e7 fe e7 fe e7 fe e7 fe e7 fe e7 4d 30 00 08 b9 00 00 08 72 b6 70 47 62 b6 70 47 ef f3 10 80 72 b6 70 47 80 f3 10 88 70 47 1b 48
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_exit_debug_mode ***
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_write_debug32 0xa05f0000 to 0xe000edf0
        2020-09-22T23:56:45 DEBUG common.c: *** stlink_close ***

- CH340 串口驱动

    在板子上有一个写有 CH340C 的电子元件, 这个就是电子元件的作用就是将 USB 协议转换为串口协议,
    要让这个这个电子元件正常工作, 需要安装 CH340 驱动.

    Ubuntu 20.04 系统的 Linux 内核已经自带了这个驱动, 可以使用如下命令查看

        ➜  lsmod | grep ch34
        ch341                  20480  0
        usbserial              53248  1 ch341

    如果不成功, 对驱动打补丁参考如下链接:

        https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all#linux

    当串口驱动成功安装后, 数据是如何从电脑传输到板子上呢?

    在 USB 接口里有 4 根线, 外侧的两根长金手指负责供电, 即电源线, 内侧的两根金手指比较短,
    用于数据传输, 即数据线.

    当我们再电脑上通过串口向板子传输数据时, 电脑端采用 USB 数据传输协议通过数据线传到板子上,
    然后 CH340 驱动解析 USB 数据将其转换为串口数据传输给板子上的其他元器件.

    串口除了传输数据的功能之外, 还有调试功能, 板子上程序的输出一般都是被重定向到串口中,
    然后打印出数据供调试用.

- 串口工具

    通过串口工具, 我们可以观察到窗口上传输的数据, 这里使用的串口工具是 kermit,
    其首页为 http://www.kermitproject.org/ck90.html

    编译安装 c-kermit 的命令如下

        wget http://www.kermitproject.org/ftp/kermit/test/tar/x.zip
        unzip x.zip -d c-kermit && cd c-kermit
        make linux

    会生成一个 wermit 二进制文件, 将其保存到某个路径比如 $HOME/.usr/bin,
    建议将其重命名为 kermit, 然后将该路径加入环境变量, 然后运行即可.

- 连接串口设备

    首先查看串口设备,  Linux 中使用 ttyS<x> 来表示串口设备名, 其中 <x> 是可变的,
    比如 ttyS0, ttyS1, 对应到 Windows 中就分别表示 COM1, COM2. 特别地,
    基于 USB 的串口设备名称一般类似于 ttyUSB0, ttyUSB1 等, 这些设备对应的文件路径位于 /dev 下面,
    比如 ttyUSB0 的位置就是 /dev/ttyUSB0.

    使用如下命令查看连接的串口设备

        ➜   dmesg | grep tty
        [    0.352821] printk: console [tty0] enabled
        [    1.179333] tty tty50: hash matches
        [193518.041877] usb 3-2: ch341-uart converter now attached to ttyUSB0

    查看串口参数

        sudo stty -F /dev/ttyUSB0 -a

    设置串口参数(波特率设置为 115200, 8 位数据模式, 其实这步是不必要的, 可以值接在 kermit 中设置)

        sudo stty -F /dev/ttyUSB0 ispeed 115200 ospeed 115200 cs8

    查看设置后的串口属性信息

        ➜  sudo stty -F /dev/ttyUSB0 -a
        speed 115200 baud; rows 0; columns 0; line = 0;
        intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>; eol2 = <undef>; swtch = <undef>;
        start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W; lnext = ^V; discard = ^O; min = 1; time = 0;
        -parenb -parodd -cmspar cs8 hupcl -cstopb cread clocal -crtscts
        -ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr icrnl ixon -ixoff -iuclc -ixany -imaxbel
        -iutf8
        opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
        isig icanon iexten echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke -flusho
        -extproc

    现在执行 wermit 进入串口工具, 然后执行如下命令

        set line /dev/ttyUSB0
        set carrier-watch off
        connect

    出现如下输出表示连接成功

        Connecting to /dev/ttyUSB0, speed 115200
         Escape character: Ctrl-\ (ASCII 28, FS): enabled
        Type the escape character followed by C to get back,
        or followed by ? to see other options.
        ----------------------------------------------------

    如果想回到 kermit, 按下 Ctrl-\, 再按下 C 即可, 如果想再连接到串口, 输入 c 即可.

- 刷入 WiFi 固件

    由于是通过串口刷 WiFi 固件, 因此需要断开其他的串口连接.

    - 让串口链路连接到 WiFi 电子元件上, 其具体做法为

        1. 定位到 `串口切换` 

            串口切换一共两排, 每排四个 Pin, 示意图如下

                * * * *
                * * * *

            将短路帽(黄色塑料套)按如下连接 

                * + + *
                * - - *

            加号的连接到一起, 减号的连接到一起.


        2. 定位到 `ESP 固件下载配置`

            这个配置一共有 3 个, 依次如下

                3v3 IO0 GND

            用短路帽把 IO0 和 GND 连接.

        3. 定位到 `BOOT 配置`

            这个配置一共有 3 个, 依次如下

                0 BOOT 1

            将短路帽把 0 和 BOOT 连接.

    - 安装 esptool 工具

        需要用到工具 esptool, 地址为 https://github.com/espressif/esptool, 安装命令如下

            pip3 install esptool

        安装完成后刷新系统环境变量, 会得到一个命令 esptool.py, 我们就是通过这个命令将 WiFi 固件刷入到板子中.
        以上准备完成后, 测试一下 esp 是否正常工作, 命令如下

            sudo -E env "PATH=$PATH" esptool.py --port /dev/ttyUSB0 read_mac

        输出类似如下

            esptool.py v2.8
            Serial port /dev/ttyUSB0
            Connecting....
            Detecting chip type... ESP8266
            Chip is ESP8266EX
            Features: WiFi
            Crystal is 26MHz
            MAC: 40:f5:20:08:6f:b1
            Uploading stub...
            Running stub...
            Stub running...
            MAC: 40:f5:20:08:6f:b1
            Hard resetting via RTS pin...


        如果连接不上, 考虑如下几种情形

        1. reset 板子

            如果之前你执行过 esptool.py, 你必须手动按一下板子上的 reset 按键,
            如果你不按 reset 继续执行 esptool.py 命令, 那么这个命令会一直挂起, 输出类似于下面这种

                esptool.py v2.8
                Serial port /dev/ttyUSB0
                Connecting........_____....

        2. 串口被占用

            如果提示下面的错误表示你的串口被占用了, 看看你的串口工具是不是连上了.

                serial.serialutil.SerialException: read failed: device reports readiness to read but returned no data (device disconnected or multiple access on port?)

        3. STLINK 和串口同时连上了

            把 STLINK 和串口线都拔下来, 只插串口线.

    - 刷入固件

        确定 esp 正常工作后, 下载固件

            git clone https://github.com/tencentyun/qcloud-iot-esp-wifi.git

        然后解压固件

            cd qcloud-iot-esp-wifi
            unzip qcloud-iot-at-esp8266/QCloud_IoT_AT_ESP8266_FW/QCloud_IoT_AT_ESP8266_v2.1.1_20200903.zip

        腾讯云 IoT 定制 AT 串口使用 UART0, Tx 为 GPIO1, Rx 为 GPIO3, UART_1_3 的固件
        采用的 Tx 和 Rx 和 AT 的一致, 因此我们使用 UART_1_3 固件, 其路径为

            QCloud_IoT_AT_ESP8266_v2.1.1_20200903/QCloud_IoT_AT_ESP8266_v2.1.1_20200903_UART_1_3.bin

        执行如下命令刷入固件

            sudo -E env "PATH=$PATH" esptool.py --port /dev/ttyUSB0 --chip esp8266 --baud 1500000 --after hard_reset  write_flash 0x0 QCloud_IoT_AT_ESP8266_v2.1.1_20200903/QCloud_IoT_AT_ESP8266_v2.1.1_20200903_UART_1_3.bin

        成功执行后的输出示例如下

            esptool.py v2.8
            Serial port /dev/ttyUSB0
            Connecting...
            Chip is ESP8266EX
            Features: WiFi
            Crystal is 26MHz
            MAC: 40:f5:20:08:6f:b1
            Uploading stub...
            Running stub...
            Stub running...
            Changing baud rate to 1500000
            Changed.
            Configuring flash size...
            Auto-detected Flash size: 4MB
            Flash params set to 0x0040
            Compressed 2039808 bytes to 485108...
            Wrote 2039808 bytes (485108 compressed) at 0x00000000 in 7.0 seconds (effective 2326.6 kbit/s)...
            Hash of data verified.

            Leaving...
            Hard resetting via RTS pin...

    - 恢复串口连接

        1. `串口切换` 

            连接示意图

                1 1 2 2
                3 3 4 4

            数值相同的连接到一起

        2. `ESP 固件下载配置`

            连接 3v3 和 IO0

        3. `BOOT 配置`

            保持不变

- 腾讯云

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

- 甲醛传感器底板连接

    根据板子和底板的 5v 和 GND 接口对应关系, 将底板连接到板子上,
    然后根据传感器的 Rx 和 Tx 口将传感器和板子底板连接起来.

- 刷入 TencentOS 系统


    - 下载源码

        下载 TencentOS 源码, 这里假定保存路径为 `<path/to/tos>`.

            git clone https://github.com/Tencent/TencentOS-tiny.git <path/to/tos>

        切出 tos_evb_g0 分支源码并保存到 `<path/to/tos_evb_g0>`.

            git clone -b tos_evb_g0 <path/to/tos> <path/to/tos_evb_g0>

        进入 `<path/to/tos_evb_g0>` 目录, 将 TOS_CONFIG/_user_config.h 复制一份保存为 TOS_CONFIG/user_config.h,
        然后修改 user_config.h 中的宏定义.

    - 编译

        使用如下命令安装工具链

            sudo apt-get install -y gcc-arm-none-eabi

        进入 tos_evb 目录下执行如下命令编译固件

            export TOS_SRC_ROOT=<path/to/tos>
            mkdir build && cmake .. && make

        执行成功后会生成文件 mqtt_iot_explorer_tc_ch20_oled.bin

    - 连接 STLINK 并刷入固件

        将 STLINK 连接到板子上, 然后执行如下命令刷入

            make flash

- 测试

    串口工具连接板子

        set line /dev/ttyUSB0
        set speed 115200
        set carrier-watch off
        connect

    按下板子上的 reset 按键, 程序就运行了, 串口会输出 WIFI CONNECTED 之类的,
    腾讯云上显示设备上线.
