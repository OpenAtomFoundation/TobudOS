# Set minimum CMake version
cmake_minimum_required(VERSION 2.8.4)

if (NOT DEFINED ENV{TOS_SRC_ROOT})
    message(FATAL_ERROR "You must set TOS_SRC_ROOT environment variable!")
endif()

# Optional verbose mode, uncomment or pass in -DCMAKE_VERBOSE_MAKEFILE=ON
set(CMAKE_VERBOSE_MAKEFILE ON)
set(TOS_SRC_ROOT $ENV{TOS_SRC_ROOT})
message(STATUS "Tencent OS source root: ${TOS_SRC_ROOT}.")
set(BOARD TencentOS_Tiny_EVB_G0)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
# Set universal poject root directory
set(ROOTDIR ${CMAKE_SOURCE_DIR})

# Set the compiler (must be prior to project setup)
include(${ROOTDIR}/toolchain/arm-gcc.cmake)

# Configure project and languages
project(mqtt_iot_explorer_tc_ch20_oled C CXX ASM)

# Set device
if (NOT DEVICE)
    set(DEVICE STM32G070xx)
endif ()

# Set build
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif ()

##### Modules #####

# Libraries can be added to the LIBS variable
# or manually included here.

# Add base libs (emlib, CMSIS, device files)
include(${ROOTDIR}/toolchain/stm32-base.cmake)


set(LINKER_SCRIPT ${ROOTDIR}/BSP/STM32G070RBTx_FLASH.ld)


# This is normally set in stm32-base.cmake, but libraries may modify it so set
# it after libraries/subdirectories have been added
# Enable float printing in stm32, see https://stackoverflow.com/questions/28334435/stm32-printf-float-variable
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_DEFINITIONS} -u _printf_float -Xlinker -T${LINKER_SCRIPT} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections -Wl,-v ")

##### Files #####

# Add project headers
include_directories(${TOS_SRC_ROOT}/kernel/core/include)
include_directories(${TOS_SRC_ROOT}/kernel/pm/include)
include_directories(${TOS_SRC_ROOT}/kernel/hal/include)
include_directories(${TOS_SRC_ROOT}/arch/arm/arm-v7m/common/include)
include_directories(${TOS_SRC_ROOT}/arch/arm/arm-v7m/cortex-m0+/gcc)
include_directories(${TOS_SRC_ROOT}/osal/cmsis_os)


include_directories(${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Inc)
include_directories(${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Inc/Legacy)
include_directories(${TOS_SRC_ROOT}/platform/vendor_bsp/st/CMSIS/Device/ST/STM32G0xx/Include)
include_directories(${TOS_SRC_ROOT}/platform/vendor_bsp/st/CMSIS/Include)


include_directories(${ROOTDIR}/inc)
include_directories(${ROOTDIR}/BSP/Inc)
include_directories(${ROOTDIR}/TOS_CONFIG)


include_directories(${TOS_SRC_ROOT}/net/at/include)
include_directories(${TOS_SRC_ROOT}/devices/esp8266_tencent_firmware)
include_directories(${TOS_SRC_ROOT}/net/tencent_firmware_module_wrapper)


include_directories(${ROOTDIR}/BSP/Hardware/CH20)
include_directories(${ROOTDIR}/BSP/Hardware/OLED)
include_directories(${ROOTDIR}/BSP/Hardware/PM2D5)



#AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32Fg0xx_HAL_Driver/Src/ SOURCE_FILES)

AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/platform/hal/st/stm32g0xx/src/ SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/kernel/core/ SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/kernel/pm/ SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/arch/arm/arm-v7m/cortex-m0+/gcc/ SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/arch/arm/arm-v7m/common/ SOURCE_FILES)
#AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/osal/cmsis_os/ SOURCE_FILES)

AUX_SOURCE_DIRECTORY(${ROOTDIR}/BSP/Src SOURCE_FILES)

AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/net/at/src SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/devices/esp8266_tencent_firmware SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${TOS_SRC_ROOT}/net/tencent_firmware_module_wrapper SOURCE_FILES)


AUX_SOURCE_DIRECTORY(${ROOTDIR}/BSP/Hardware/CH20 SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${ROOTDIR}/BSP/Hardware/OLED SOURCE_FILES)
AUX_SOURCE_DIRECTORY(${ROOTDIR}/BSP/Hardware/PM2D5 SOURCE_FILES)


# Generate executable and link
add_executable(${PROJECT_NAME}
	${ROOTDIR}/BSP/startup_stm32g070xx.s
	${TOS_SRC_ROOT}/arch/arm/arm-v7m/cortex-m0+/gcc/port_s.S
	${TOS_SRC_ROOT}/osal/cmsis_os/cmsis_os.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_cortex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_tim.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_tim_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_uart.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_uart_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rcc.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rcc_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rtc.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rtc_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_flash.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_flash_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_gpio.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_dma.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_dma_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_i2c.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_i2c_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_spi.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_spi_ex.c
	${TOS_SRC_ROOT}/platform/vendor_bsp/st/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_adc.c
	${SOURCE_FILES}
    )

stm32_configure_linker_addresses(${PROJECT_NAME})

add_library(rustcore STATIC ${ROOTDIR}/libs/rustcore/stub.c)
add_library(rustapp STATIC ${ROOTDIR}/libs/rustapp/stub.c)
add_library(tosglue STATIC ${ROOTDIR}/tosglue.c)

target_link_libraries(${PROJECT_NAME} ${LIBS} c nosys rustcore rustapp)

# Link optional libraries if available
if (${HAS_HARDWARE})
    target_link_libraries(${PROJECT_NAME} hardware)
endif ()

if (${HAS_PROTOCOL})
    target_link_libraries(${PROJECT_NAME} protocol)
endif ()

##### Post build #####

# Add post build commands
include(${ROOTDIR}/toolchain/post-build.cmake)

# Add stlink commands
include(${ROOTDIR}/toolchain/stlink.cmake)

##### CMake debug prints #####
if (CMAKE_VERBOSE_MAKEFILE)
    print_debug_info()
endif()
