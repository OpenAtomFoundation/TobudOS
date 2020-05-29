##
##   ______                              _
##  / _____)             _              | |
## ( (____  _____ ____ _| |_ _____  ____| |__
##  \____ \| ___ |    (_   _) ___ |/ ___)  _ \
##  _____) ) ____| | | || |_| ____( (___| | | |
## (______/|_____)_|_|_| \__)_____)\____)_| |_|
## (C)2013-2017 Semtech
##  ___ _____ _   ___ _  _____ ___  ___  ___ ___
## / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
## \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
## |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
## embedded.connectivity.solutions.==============
##
## License:  Revised BSD License, see LICENSE.TXT file included in the project
## Authors:  Johannes Bruder (STACKFORCE), Miguel Luis (Semtech)
##
project(B-L072Z-LRWAN1)
cmake_minimum_required(VERSION 3.6)
enable_language(ASM)

#---------------------------------------------------------------------------------------
# Target
#---------------------------------------------------------------------------------------

list(APPEND ${PROJECT_NAME}_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/adc-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/delay-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/eeprom-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/gpio-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lpm-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/rtc-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/spi-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sx1276-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/uart-board.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/sysIrqHandlers.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/utilities.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmsis/arm-gcc/startup_stm32l072xx.s"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmsis/system_stm32l0xx.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_adc_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc_ex.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_spi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c"
)

add_library(${PROJECT_NAME} OBJECT EXCLUDE_FROM_ALL ${${PROJECT_NAME}_SOURCES})

target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_HAL_DRIVER -DSTM32L072xx)

# Add define if debbuger support is enabled
target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<BOOL:${USE_DEBUGGER}>:USE_DEBUGGER>)

# Add define if radio debug pins support is enabled
target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<BOOL:${USE_RADIO_DEBUG}>:USE_RADIO_DEBUG>)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis
    ${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32
    ${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/cmsis
    ${CMAKE_CURRENT_SOURCE_DIR}/../mcu/stm32/STM32L0xx_HAL_Driver/Inc
    $<TARGET_PROPERTY:board,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:system,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:radio,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:peripherals,INTERFACE_INCLUDE_DIRECTORIES>
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
