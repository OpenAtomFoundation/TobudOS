cmake_minimum_required(VERSION 3.8)

project(mqtt_demo)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(TINY_ROOT ../../../)

## kernel
include_directories(${TINY_ROOT}/osal/cmsis_os)
include_directories(${TINY_ROOT}/kernel/core/include)
include_directories(${TINY_ROOT}/kernel/evtdrv/include)
include_directories(${TINY_ROOT}/kernel/hal/include)
include_directories(${TINY_ROOT}/kernel/pm/include)

aux_source_directory(${TINY_ROOT}/osal/cmsis_os CMSIS_SRCS)
aux_source_directory(${TINY_ROOT}/kernel/core CORE_SRCS)
aux_source_directory(${TINY_ROOT}/kernel/evtdrv EVTDRV_SRCS)
aux_source_directory(${TINY_ROOT}/kernel/pm PM_SRCS)

set(KERNEL_SRCS ${CMSIS_SRCS} ${EVTDRV_SRCS} ${PM_SRCS} ${CORE_SRCS})

## net
include_directories(${TINY_ROOT}/components/connectivity/Eclipse-Paho-MQTT/3rdparty/include)
include_directories(${TINY_ROOT}/components/connectivity/Eclipse-Paho-MQTT/wrapper/include)
include_directories(${TINY_ROOT}/net/sal_module_wrapper)

aux_source_directory(${TINY_ROOT}/components/connectivity/Eclipse-Paho-MQTT/3rdparty/src MQTT_3RD_SRCS)
aux_source_directory(${TINY_ROOT}/components/connectivity/Eclipse-Paho-MQTT/wrapper/src MQTT_SRCS)
aux_source_directory(${TINY_ROOT}/net/sal_module_wrapper SAL_SRCS)

set(NET_SRCS ${MQTT_3RD_SRCS} ${MQTT_SRCS} ${SAL_SRCS})

## arch
set(ARCH_ROOT ${TINY_ROOT}/arch/linux)

include_directories(${ARCH_ROOT}/common/include)
include_directories(${ARCH_ROOT}/posix/gcc)

aux_source_directory(${ARCH_ROOT}/common ARCH_COMMON_SRCS)
aux_source_directory(${ARCH_ROOT}/posix/gcc ARCH_POSIX_SRCS)

set(ARCH_SRCS ${ARCH_COMMON_SRCS} ${ARCH_POSIX_SRCS})

set(TINY_SRCS ${ARCH_SRCS} ${KERNEL_SRCS} ${NET_SRCS})

## app
include_directories(./)
include_directories(./inc)

aux_source_directory(./src APP_SRCS)

add_executable(mqtt_demo ${APP_SRCS} ${TINY_SRCS})

target_link_libraries(mqtt_demo pthread)