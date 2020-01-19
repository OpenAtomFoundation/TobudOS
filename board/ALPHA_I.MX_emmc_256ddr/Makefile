TOP_DIR			:= ../..

CROSS_COMPILE		?= arm-linux-gnueabihf-
TARGET			?= TencentOS_tiny

CC			:= $(CROSS_COMPILE)gcc
LD			:= $(CROSS_COMPILE)ld
OBJCOPY 		:= $(CROSS_COMPILE)objcopy
OBJDUMP 		:= $(CROSS_COMPILE)objdump
GCC_VERSION		:= 6.5.0 # 4.9.4

LD_SCRIPT_S		:= link.ld.S
LD_SCRIPT_DIR		:= $(TOP_DIR)/arch/arm/arm-v7a/lds
LD_SCRIPT		:= link.ld

GCC_DIR			:= D:\Green\gcc-arm-linux-gnueabihf
OUT_ROOT		:= out
OBJ_DIR			:= obj
LIB_DIR			:= lib
TARGET_DIR		:= target
OUT_DIR			:= $(OUT_ROOT)/$(OBJ_DIR) $(OUT_ROOT)/$(LIB_DIR) $(OUT_ROOT)/$(TARGET_DIR)

NOECHO			:= @

LIBPATH			:= -lgcc -L$(GCC_DIR)/lib/gcc/arm-linux-gnueabihf/$(GCC_VERSION)

INCDIRS 		:= \
			$(TOP_DIR)/kernel/core/include \
			$(TOP_DIR)/kernel/pm/include \
			$(TOP_DIR)/arch/arm/arm-v7a/common/include \
			$(TOP_DIR)/arch/arm/arm-v7a/cortex-a7/gcc \
			$(TOP_DIR)/arch/arm/arm-v7a/cortex-a7/chip/imx6ul/include \
			imx6ul \
			TOS_CONFIG \
			stdio/include \
			bsp/clk \
			bsp/led \
			bsp/delay \
			bsp/beep \
			bsp/gpio \
			bsp/key \
			bsp/exit \
			bsp/int \
			bsp/epittimer \
			bsp/keyfilter \
			bsp/uart \
			bsp/lcd \
			bsp/rtc \
			bsp/i2c \
			bsp/ap3216c \
			bsp/spi \
			bsp/icm20608 \
			bsp/touchscreen \
			bsp/backlight

SRCDIRS			:= \
			src \
			$(TOP_DIR)/kernel/core \
			$(TOP_DIR)/arch/arm/arm-v7a/cortex-a7/gcc \
			$(TOP_DIR)/arch/arm/arm-v7a/common \
			$(TOP_DIR)/arch/arm/arm-v7a/cortex-a7/chip/imx6ul \
			stdio/lib \
			bsp/clk \
			bsp/led \
			bsp/delay \
			bsp/beep \
			bsp/gpio \
			bsp/key \
			bsp/exit \
			bsp/int \
			bsp/epittimer \
			bsp/keyfilter \
			bsp/uart \
			bsp/lcd \
			bsp/rtc \
			bsp/i2c \
			bsp/ap3216c \
			bsp/spi \
			bsp/icm20608 \
			bsp/touchscreen \
			bsp/backlight

INCLUDE			:= $(patsubst %, -I %, $(INCDIRS))

SFILES			:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES			:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

SFILENDIR		:= $(notdir $(SFILES))
CFILENDIR		:= $(notdir $(CFILES))

SOBJS			:= $(patsubst %, $(OUT_ROOT)/$(OBJ_DIR)/%, $(SFILENDIR:.S=.o))
COBJS			:= $(patsubst %, $(OUT_ROOT)/$(OBJ_DIR)/%, $(CFILENDIR:.c=.o))
OBJS			:= $(SOBJS) $(COBJS)

VPATH			:= $(SRCDIRS)

.PHONY: clean $(OUT_DIR)

$(TARGET).bin : $(OUT_DIR) $(OBJS) $(LD_SCRIPT)
	@echo "LD $(TARGET).elf"
	$(NOECHO)$(LD) -T$(LD_SCRIPT) -o $(OUT_ROOT)/$(TARGET_DIR)/$(TARGET).elf $(OBJS) $(LIBPATH)
	@echo "OBJCOPY $(TARGET).bin"
	$(NOECHO)$(OBJCOPY) -O binary -S $(OUT_ROOT)/$(TARGET_DIR)/$(TARGET).elf $(OUT_ROOT)/$(TARGET_DIR)/$@
	@echo "OBJDUMP $(TARGET).asm"
	$(NOECHO)$(OBJDUMP) -D -m arm $(OUT_ROOT)/$(TARGET_DIR)/$(TARGET).elf > $(OUT_ROOT)/$(TARGET_DIR)/$(TARGET).asm

$(LD_SCRIPT): $(LD_SCRIPT_DIR)/$(LD_SCRIPT_S) 
	@echo "generate link script"
	$(NOECHO)$(CC) $(INCLUDE) -E -P -o $@ $<

$(OUT_DIR) :
	@mkdir -p $@

# $(NOECHO)$(CC) -march=armv7-a -nostdlib -fno-builtin -Wall -c -O2 $(INCLUDE) -o $@ $<
$(SOBJS) : $(OUT_ROOT)/$(OBJ_DIR)/%.o : %.S
	@echo "CC $(notdir $<)"
	$(NOECHO)$(CC) -Wall -nostdlib -fno-builtin -c -O2 $(INCLUDE) -o $@ $<

# $(NOECHO)$(CC) -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -Wa,-mimplicit-it=thumb -nostdlib -fno-builtin -Wall -c -O2 $(INCLUDE) -o $@ $<
$(COBJS) : $(OUT_ROOT)/$(OBJ_DIR)/%.o : %.c
	@echo "CC $(notdir $<)"
	$(NOECHO)$(CC) -Wall -Wa,-mimplicit-it=thumb -nostdlib -fno-builtin -c -O2 $(INCLUDE) -o $@ $<

clean:
	@rm -rf $(OUT_ROOT) $(LD_SCRIPT)

