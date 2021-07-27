# 串口工具 - kermit

- 安装

    - linux

            wget http://www.kermitproject.org/ftp/kermit/test/tar/x.zip
            unzip x.zip -d c-kermit && cd c-kermit
            make linux
            make DESTDIR=$HOME/.usr/kermit BINDIR=$DESTDIR MANDIR=$DESTDIR install

        然后导出环境变量

            KERMIT=$HOME/.usr/kermit
            export PATH=${KERMIT}:${PATH}

- 使用

    配置串口

        sudo stty -F /dev/ttyUSB0 ispeed 115200 ospeed 115200 cs8

    打开 ~/.kermrc, 配置 kermit 启动参数

        set line /dev/ttyUSB0
        set speed 115200
        set carrier-watch off
        connect

    执行 sudo -E env "PATH=$PATH" kermit 启动串口工具, 出现如下输出表示连接成功

        Connecting to /dev/ttyUSB0, speed 115200
         Escape character: Ctrl-\ (ASCII 28, FS): enabled
        Type the escape character followed by C to get back,
        or followed by ? to see other options.
        ----------------------------------------------------

    如果想回到 kermit, 按下 Ctrl-\, 再按下 C 即可, 如果想再连接到串口, 输入 c 即可.
