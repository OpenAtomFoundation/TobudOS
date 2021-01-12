# 甲醛检测仪 - mac 环境下搭建

由于之前在 Linux 上对环境进行了配置, 板子基本上已经初始化好了,
目前在 mac 上配置需要做的工作不是太多, 下面有些步骤可能不太详细,
可以参考 [Linux](./setup.linux.ubuntu.md).

## 工具准备

- stlink 安装

    - 依赖项

        - [cmake](https://cmake.org/download/): 下载 tar.gz 的压缩包自行安装并添加到环境变量中
        - [llvm](https://releases.llvm.org/download.html): 参考 cmake 处理
        - libusb: brew install libusb

    - 安装

            git clone https://github.com/stlink-org/stlink
            cd stlink
            make release

        安装到 $HOME/.usr/stlink 目录中

            cd build/Release && make install DESTDIR=$HOME/.usr/stlink

        使用 ioreg 查看 USB 设备:

            ➜  ioreg -p IOUSB
            +-o Root  <class IORegistryEntry, id 0x100000100, retain 23>
              +-o  ...
              | +-o USB2.1 Hub@14100000  <class AppleUSBDevice, id 0x100145cd9, registered, matched, active, busy 0 (0 ms), retain 15>
              |   +-o ...
              |   +-o STM32 STLink@14110000  <class AppleUSBDevice, id 0x100145d1a, registered, matched, active, busy 0 (1 ms), retain 12>
              |   +-o ...
              +-o ...

        可以看到我们的 STM32 设备已经连接上了, 然后使用 st-info --probe 看看是否能检测到
        stlink 编程器, 如果找到则有类似下面的输出

            Found 1 stlink programmers
            ...

- CH340 驱动

    - macOS Mojava 10.4 或者之后的版本: 无需安装
    - macOS Mojava 10.4 之后的版本: 到[这里](https://github.com/adrianmihalko/ch340g-ch34g-ch34x-mac-os-x-driver)下载安装

    安装完成后通过 `ls -1 /dev/cu*` 能够看到一个 `/dev/cu.wchusbserial****` 之类的设备.

- 串口工具

        wget http://www.kermitproject.org/ftp/kermit/test/tar/x.zip
        unzip x.zip -d c-kermit && cd c-kermit
        make KFLAGS='-Wno-error=implicit-function-declaration' macosx
        make DESTDIR=$HOME/.usr/kermit BINDIR=$DESTDIR MANDIR=$DESTDIR install

    会安装到 $HOME/.usr/kermit 目录, 将该目录添加到环境变量 PATH 中.

- 连接串口设备

    显示可用的串口

        ls -1 /dev/tty.* /dev/cu.*

    输出样例

        /dev/cu.Bluetooth-Incoming-Port
        /dev/cu.usbserial-14120
        /dev/tty.Bluetooth-Incoming-Port
        /dev/tty.usbserial-14120

    根据 https://pbxbook.com/other/mac-tty.html, 我们知道我们应该选择 cu 开头的设备,
    因此这里我们使用 /dev/cu.usbserial-14120.

    打开 ~/.kermrc, 输入如下命令

        set line /dev/cu.usbserial-14120
        set speed 115200
        set carrier-watch off
        connect

   然后每次执行 kermit 命令就会自动连上串口, 能够看到调试信息输出, 要退出的话按下 Ctrl-\ 然后按下 C 即可.

##  WiFi 固件安装

参考 [Linux](./setup.linux.ubuntu.md).

## 腾讯云帐号

参考 [Linux](./setup.linux.ubuntu.md).

## TencentOS 固件安装

编译所需要的工具链安装方法如下

    brew install armmbed/formulae/arm-none-eabi-gcc

安装后的路径为

    /usr/local/bin/arm-none-eabi-*

其他参考 [Linux](./setup.linux.ubuntu.md).
