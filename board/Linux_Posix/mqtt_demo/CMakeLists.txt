cmake_minimum_required(VERSION 3.8)

project(mqtt_demo)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(TINY_ROOT ../../../)

add_definitions(-DMQTT_NETWORK_TYPE_NO_TLS -DMQTT_NETSOCKET_USING_AT)

## kernel
include_directories(${TINY_ROOT}/osal/cmsis_os)
include_directories(${TINY_ROOT}/kernel/core/include)
include_directories(${TINY_ROOT}/kernel/evtdrv/include)
include_directories(${TINY_ROOT}/kernel/hal/include)
include_directories(${TINY_ROOT}/kernel/pm/include)

set(CMSIS_SRCS ${TINY_ROOT}/osal/cmsis_os/cmsis_os.c)
aux_source_directory(${TINY_ROOT}/kernel/core CORE_SRCS)
aux_source_directory(${TINY_ROOT}/kernel/evtdrv EVTDRV_SRCS)
aux_source_directory(${TINY_ROOT}/kernel/pm PM_SRCS)

set(KERNEL_SRCS ${CMSIS_SRCS} ${EVTDRV_SRCS} ${PM_SRCS} ${CORE_SRCS})

## net
include_directories(${TINY_ROOT}/net/sal_module_wrapper)
include_directories(${TINY_ROOT}/components/connectivity/mqttclient/mqttclient)

aux_source_directory(${TINY_ROOT}/net/sal_module_wrapper SAL_SRCS)

set(MQTT_ROOT ${TINY_ROOT}/components/connectivity/mqttclient)
include_directories(${MQTT_ROOT}/common)
include_directories(${MQTT_ROOT}/common/log)
include_directories(${MQTT_ROOT}/mqtt)
include_directories(${MQTT_ROOT}/mqttclient)
include_directories(${MQTT_ROOT}/network)
include_directories(${MQTT_ROOT}/platform/TencentOS-tiny)

aux_source_directory(${MQTT_ROOT}/common MQTT_COMMON)
aux_source_directory(${MQTT_ROOT}/common/log MQTT_COMMON_LOG)
aux_source_directory(${MQTT_ROOT}/mqttclient MQTT_CLIENT)
aux_source_directory(${MQTT_ROOT}/mqtt MQTT_CORE)
aux_source_directory(${MQTT_ROOT}/network MQTT_NETWORK)
aux_source_directory(${MQTT_ROOT}/platform/TencentOS-tiny MQTT_PLATFORM)

set(MQTT_SRCS 
    ${MQTT_ROOT}/common/log/arch/tencentos-tiny/arch.c
    ${MQTT_COMMON}
    ${MQTT_COMMON_LOG}
    ${MQTT_CLIENT}
    ${MQTT_CORE}
    ${MQTT_NETWORK}
    ${MQTT_PLATFORM}
)

set(NET_SRCS ${MQTT_SRCS} ${SAL_SRCS})

## arch
set(ARCH_ROOT ${TINY_ROOT}/arch/linux)

include_directories(${ARCH_ROOT}/common/include)
include_directories(${ARCH_ROOT}/posix/gcc)

aux_source_directory(${ARCH_ROOT}/common ARCH_COMMON_SRCS)
aux_source_directory(${ARCH_ROOT}/posix/gcc ARCH_POSIX_SRCS)

set(ARCH_SRCS ${ARCH_COMMON_SRCS} ${ARCH_POSIX_SRCS})


include_directories(${TINY_ROOT}/components/utils/JSON/include)

set(UTILS_SRCS ${TINY_ROOT}/components/utils/JSON/src/cJSON.c)

set(TINY_SRCS ${ARCH_SRCS} ${KERNEL_SRCS} ${NET_SRCS} ${UTILS_SRCS})

## app
include_directories(./)
include_directories(./inc)

aux_source_directory(./src APP_SRCS)

add_executable(mqtt_demo ${APP_SRCS} ${TINY_SRCS})

target_link_libraries(mqtt_demo pthread)