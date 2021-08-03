# ST-LINK 驱动安装

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

    ST-LINK 的驱动源码地址为 https://github.com/stlink-org/stlink

    安装编译依赖

        - ubuntu

                sudo apt install clang build-essential cmake libusb-1.0-0 libusb-1.0-0-dev

        - archlinux

                sudo pacman -S cmake libusb clang

    下载源码并编译, 注意 st-link 默认分支是 develop, 这个分支上的功能是不稳定的

        git clone https://github.com/stlink-org/stlink
        cd stlink
        git checkout v1.6.1

    安装到 $HOME/.usr/stlink 目录中

        make release
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

    刷新环境变量

        exec $SHELL

    更新库缓存

        sudo ldconfig

    然后查看板子信息

        ➜  ~ st-info --probe
        Found 1 stlink programmers
         serial:     2c5a15000b14324d434d4e00
         hla-serial: "\x2c\x5a\x15\x00\x0b\x14\x32\x4d\x43\x4d\x4e\x00"
         flash:      131072 (pagesize: 2048)
         sram:       36864
         chipid:     0x0460
         descr:      G070/G071/G081

    如果上述命令输出如下

        Found 1 stlink programmers
        version:    V2J35S7
        serial:     2C5A15000B14324D434D4E00
        flash:      0 (pagesize: 0)
        sram:       0
        chipid:     0x0000
        descr:      unknown device

    这表示无法识别 st-link 设备, 检查是否使用了 develop 分支的代码, 这个分支上的代码无法正确识别 st-link,
    切换到 v1.6.1 tag 解决问题.

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

- 虚拟机映射

    VirtualBox 虚拟机需要将用户添加到 vboxusers 用户组

        sudo usermod -aG vboxusers $USER

    添加完毕后, 重启主机.
