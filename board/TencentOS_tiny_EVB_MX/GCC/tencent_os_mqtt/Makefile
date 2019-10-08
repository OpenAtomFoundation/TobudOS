######################################
# target
######################################
TARGET = TencentOS_tiny

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0

TOP_DIR = ../../../../
#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
KERNEL_SRC =  \
		${wildcard $(TOP_DIR)/kernel/core/*.c}
		C_SOURCES += $(KERNEL_SRC)

ARCH_SRC =  \
		${wildcard $(TOP_DIR)/arch/arm/arm-v7m/cortex-m4/gcc/*.c} \
		${wildcard $(TOP_DIR)/arch/arm/arm-v7m/common/*.c}
		C_SOURCES += $(ARCH_SRC)

CMSIS_SRC =  \
		${wildcard $(TOP_DIR)/osal/cmsis_os/*.c}
		C_SOURCES += $(CMSIS_SRC)

PLATFORM_SRC =  \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac_ex.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c \
		$(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c \
		$(TOP_DIR)/platform/hal/st/stm32l4xx/src/tos_hal_uart.c
		C_SOURCES += $(PLATFORM_SRC)

NET_SRC =  \
		$(TOP_DIR)/net/at/src/tos_at.c \
		$(TOP_DIR)/net/at/src/tos_at_utils.c \
		$(TOP_DIR)/net/sal_module_wrapper/sal_module_wrapper.c \
		$(TOP_DIR)/devices/esp8266/esp8266.c
		C_SOURCES += $(NET_SRC)

BSP_SRC =  \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/gpio.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/main.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/mcu_init.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/stm32l4xx_hal_msp.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/stm32l4xx_it_module.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/usart.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/adc.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/dac.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/i2c.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/spi.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Src/system_stm32l4xx.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Hardware/DHT11/DHT11_BUS.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Hardware/OLED/oled.c \
		$(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Hardware/E53_IA1/E53_IA1.c
		C_SOURCES += $(BSP_SRC)

COMPONENTS_SRC =  \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/wrapper/src/mqtt_wrapper.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/wrapper/src/transport_wrapper.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTConnectClient.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTConnectServer.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTDeserializePublish.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTFormat.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTPacket.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTSerializePublish.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTSubscribeClient.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTSubscribeServer.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTUnsubscribeClient.c \
		$(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src/MQTTUnsubscribeServer.c
		C_SOURCES += $(COMPONENTS_SRC)

EXAMPLES_SRC =  \
		$(TOP_DIR)/examples/tencent_os_mqtt/mqtt_example.c 
		C_SOURCES += $(EXAMPLES_SRC)

# ASM sources
ASM_SOURCES =  \
startup_stm32l431xx.s \

ASM_SOURCES_S =  \
$(TOP_DIR)/arch/arm/arm-v7m/cortex-m4/gcc/port_s.S

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L431xx \
-DUSE_HAL_DRIVER \
-DSTM32L431xx

# AS includes
AS_INCLUDES = 

# C includes
KERNEL_INC = \
		-I $(TOP_DIR)/kernel/core/include  \
		-I $(TOP_DIR)/kernel/pm/include  \
		-I $(TOP_DIR)/kernel/hal/include \
		-I $(TOP_DIR)/arch/arm/arm-v7m/common/include \
		-I $(TOP_DIR)/arch/arm/arm-v7m/cortex-m4/gcc
		C_INCLUDES += $(KERNEL_INC)

CMSIS_INC = \
		-I $(TOP_DIR)/osal/cmsis_os
		C_INCLUDES += $(CMSIS_INC)

PLATFORM_INC = \
		-I $(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Inc \
		-I $(TOP_DIR)/platform/vendor_bsp/st/STM32L4xx_HAL_Driver/Inc/Legacy \
		-I $(TOP_DIR)/platform/vendor_bsp/st/CMSIS/Device/ST/STM32L4xx/Include \
		-I $(TOP_DIR)/platform/vendor_bsp/st/CMSIS/Include
		C_INCLUDES += $(PLATFORM_INC)

BOARD_INC =  \
		-I $(TOP_DIR)/board/TencentOS_tiny_EVB_MX/TOS-CONFIG \
		-I $(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Inc \
		-I $(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Hardware/DHT11 \
		-I $(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Hardware/BH1750 \
		-I $(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Hardware/E53_IA1 \
		-I $(TOP_DIR)/board/TencentOS_tiny_EVB_MX/BSP/Hardware/OLED
		C_INCLUDES += $(BOARD_INC)

NET_INC = \
		-I $(TOP_DIR)/net/at/include \
		-I $(TOP_DIR)/net/sal_module_wrapper \
		-I $(TOP_DIR)/devices/esp8266
		C_INCLUDES += $(NET_INC)

COMPONENTS_INC = \
		-I $(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/wrapper/include \
		-I $(TOP_DIR)/components/connectivity/Eclipse-Paho-MQTT/3rdparty/include
		C_INCLUDES += $(COMPONENTS_INC)

EXAMPLES_INC = \
		-I $(TOP_DIR)/examples/tencent_os_mqtt
		C_INCLUDES += $(EXAMPLES_INC)

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32L431RCTx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -u _printf_float -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) \
		-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES_S:.S=.o)))
vpath %.S $(sort $(dir $(ASM_SOURCES_S)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)
