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
		$(TOP_DIR)/components/security/mbedtls/wrapper/src/entropy_hardware_alt.c \
		$(TOP_DIR)/components/security/mbedtls/wrapper/src/net_module_alt.c \
		$(TOP_DIR)/components/security/mbedtls/wrapper/src/timing_alt.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/aes.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/aesni.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/arc4.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/aria.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/asn1parse.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/asn1write.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/base64.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/bignum.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/blowfish.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/camellia.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ccm.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/certs.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/chacha20.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/chachapoly.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/cipher.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/cipher_wrap.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/cmac.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ctr_drbg.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/debug.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/des.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/dhm.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ecdh.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ecdsa.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ecjpake.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ecp.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ecp_curves.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/entropy.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/entropy_poll.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/error.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/gcm.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/havege.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/hkdf.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/hmac_drbg.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/md.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/md_wrap.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/md2.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/md4.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/md5.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/memory_buffer_alloc.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/net_sockets.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/nist_kw.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/oid.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/padlock.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/pem.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/pk.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/pk_wrap.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/pkcs5.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/pkcs11.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/pkcs12.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/pkparse.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/pkwrite.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/platform.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/platform_util.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/poly1305.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ripemd160.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/rsa.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/rsa_internal.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/sha1.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/sha256.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/sha512.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ssl_cache.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ssl_ciphersuites.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ssl_cli.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ssl_cookie.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ssl_srv.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ssl_ticket.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/ssl_tls.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/threading.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/timing.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/version.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/version_features.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/x509.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/x509_create.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/x509_crl.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/x509_crt.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/x509_csr.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/x509write_crt.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/x509write_csr.c \
		$(TOP_DIR)/components/security/mbedtls/3rdparty/src/xtea.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/qcloud_log.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/qcloud_network.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/qcloud_device.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/qcloud_tls.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_aes.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_base64.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_hmac.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_httpc.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_json_parser.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_json_token.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_md5.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_sha1.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/utils/qcloud_string_utils.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/mqtt/qcloud_mqtt_client.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/mqtt/qcloud_mqtt_common.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/mqtt/qcloud_mqtt_connect.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/mqtt/qcloud_mqtt_glue.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/mqtt/qcloud_mqtt_publish.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/mqtt/qcloud_mqtt_subscribe.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/mqtt/qcloud_mqtt_unsubscribe.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/mqtt/qcloud_mqtt_yield.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/port/TencentOS_tiny/osal_dtls.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/port/TencentOS_tiny/osal_os.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/port/TencentOS_tiny/osal_tcp_module.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/port/TencentOS_tiny/osal_timer.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/port/TencentOS_tiny/osal_tls.c \
		$(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/src/port/TencentOS_tiny/osal_udp_module.c 
		C_SOURCES += $(COMPONENTS_SRC)

EXAMPLES_SRC =  \
		$(TOP_DIR)/examples/tencent_cloud_sdk_mqtt/mqtt_sample.c \
		$(TOP_DIR)/examples/tencent_cloud_sdk_mqtt/tencent_cloud_sdk_mqtt.c
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
-DMBEDTLS_CONFIG_FILE='<qcloud/tls_psk_config.h>'

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
		-I $(TOP_DIR)/devices/esp8266_tencent_firmware \
		-I $(TOP_DIR)/devices/rhf76_lora \
		-I $(TOP_DIR)/net/lora_module_wrapper \
		-I $(TOP_DIR)/net/tencent_firmware_module_wrapper \
		-I $(TOP_DIR)/devices/bc35_28_95 \
		-I $(TOP_DIR)/devices/esp8266
		C_INCLUDES += $(NET_INC)

COMPONENTS_INC = \
		-I $(TOP_DIR)/components/security/mbedtls/wrapper/include \
		-I $(TOP_DIR)/components/security/mbedtls/wrapper/include/qcloud \
		-I $(TOP_DIR)/components/connectivity/TencentCloud_SDK/source/include \
		-I $(TOP_DIR)/components/security/mbedtls/3rdparty/include \
		-I $(TOP_DIR)/components/security/mbedtls/3rdparty/include/mbedtls
		C_INCLUDES += $(COMPONENTS_INC)

EXAMPLES_INC = 
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
