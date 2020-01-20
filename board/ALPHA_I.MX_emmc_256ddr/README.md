# TencentOS tiny Cortex V7-A支持(基于正点原子阿尔法alpha开发板)

## 如何运行

1. 安装arm交叉编译器（本构建采用6.5.0版本）
2. 在TencentOS-tiny\board\ALPHA_I.MX_emmc_256ddr目录下，根据自己的arm交叉编译器版本及安装目录，对应修改Makefile中的如下变量：
   - GCC_VERSION
   - GCC_DIR
3. 在TencentOS-tiny\board\ALPHA_I.MX_emmc_256ddr目录下执行make命令
4. 利用正点原子官方的imxdownload工具，将out/target/TencenOS_tiny.bin烧写到SD卡中
5. 将SD卡插入阿尔法开发板，接上串口，开发板复位上电，可从串口中看到两个任务以一定的时序交叉运行（业务入口文件为TencentOS-tiny\board\ALPHA_I.MX_emmc_256ddr\src\main.c）。

## 注意

1. 正点原子阿尔法开发板的相关板级驱动，由正点原子官方示例提供，并已提前征得正点原子官方同意将相关源码作为示例代码集成。

2. 正点原子官方烧写工具目前只支持linux平台。
3. 后续TencentOS tiny会集成自己的镜像烧写工具（原子官方的烧写工具可定制性稍有欠缺）