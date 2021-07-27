# WiFi 模块

由于是通过串口刷 WiFi 固件, 因此需要断开其他的串口连接, 同时也要断开 stlink 的连接.

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

    更新 zsh/bash 配置文件

        export PATH=$HOME/.local/bin:${PATH}

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

        配置一下串口速度

            sudo stty -F /dev/ttyUSB0 ispeed 115200 ospeed 115200 cs8

        执行如下命令刷入固件 (虚拟机需要将 --baud 修改为 115200)

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
