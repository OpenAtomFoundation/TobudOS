#使用openocd调试

需要编译特定的openocd


```git clone https://github.com/riscv-mcu/riscv-openocd.git```

如果用的是Sipeed USB-JTAG/TTL RISC-V Debugger，需要在编译的时候enable ftdi

```
./configure --enable-cmsis-dap --enable-ftdi
```