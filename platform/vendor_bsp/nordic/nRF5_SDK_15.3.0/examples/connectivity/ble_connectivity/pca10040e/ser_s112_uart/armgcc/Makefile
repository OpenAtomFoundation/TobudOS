PROJECT_NAME     := ble_connectivity_s112_uart_pca10040e
TARGETS          := nrf52810_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../..
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/nrf52810_xxaa.out: \
  LINKER_SCRIPT  := ble_connectivity_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52810.S \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/experimental/app_timer2.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/timer/experimental/drv_rtc.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52810.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/common/ble_dtm_init.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_event_enc.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_evt_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_gap_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_gap_evt_conn.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/ble/ble_gap_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/ble/ble_gatt_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_gattc_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_gattc_evt_conn.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/ble/ble_gattc_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_gatts_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/ble_gatts_evt_conn.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/ble/ble_gatts_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/common/ble_serialization.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/ble/ble_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/common/cond_field_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/conn_ble_gap_sec_keys.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/conn_ble_user_mem.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/common/conn_mw.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/middleware/conn_mw_ble.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/middleware/conn_mw_ble_gap.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/middleware/conn_mw_ble_gattc.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/middleware/conn_mw_ble_gatts.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/common/conn_mw_nrf_soc.c \
  $(SDK_ROOT)/components/serialization/connectivity/hal/dtm_uart.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers/nrf_soc_conn.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/ble/nrf_soc_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_cmd_decoder.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_dtm_cmd_decoder.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_error_handling.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_event_encoder.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_handlers.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_pkt_decoder.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_reset_cmd_decoder.c \
  $(SDK_ROOT)/components/serialization/common/ser_dbg_sd_str.c \
  $(SDK_ROOT)/components/serialization/common/transport/ser_hal_transport.c \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy/ser_phy_uart.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(PROJ_DIR)/main.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/ble/ble_dtm/ble_dtm.c \
  $(SDK_ROOT)/components/ble/ble_dtm/ble_dtm_hw_nrf52.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/serialization/connectivity/hal \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/serializers \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/serialization/connectivity \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/ble/middleware \
  $(SDK_ROOT)/components/serialization/common \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/util \
  ../config \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy/config \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/softdevice/s112/headers/nrf52 \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/common \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/serialization/common/struct_ser/ble \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/components/serialization/common/transport \
  $(SDK_ROOT)/components/libraries/timer/experimental \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/components/softdevice/s112/headers \
  $(SDK_ROOT)/components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -Os -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DDEVELOP_IN_NRF52832
CFLAGS += -DFLOAT_ABI_SOFT
CFLAGS += -DNRF52810_XXAA
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3
CFLAGS += -DNRF_SD_BLE_API_VERSION=6
CFLAGS += -DS112
CFLAGS += -DSER_CONNECTIVITY
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=soft
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=soft
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DBSP_DEFINES_ONLY
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DDEVELOP_IN_NRF52832
ASMFLAGS += -DFLOAT_ABI_SOFT
ASMFLAGS += -DNRF52810_XXAA
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=6
ASMFLAGS += -DS112
ASMFLAGS += -DSER_CONNECTIVITY
ASMFLAGS += -DSOFTDEVICE_PRESENT

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52810_xxaa: CFLAGS += -D__HEAP_SIZE=512
nrf52810_xxaa: CFLAGS += -D__STACK_SIZE=2048
nrf52810_xxaa: ASMFLAGS += -D__HEAP_SIZE=512
nrf52810_xxaa: ASMFLAGS += -D__STACK_SIZE=2048

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52810_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52810_xxaa
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52810_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52810_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s112_nrf52_6.1.1_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s112/hex/s112_nrf52_6.1.1_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
