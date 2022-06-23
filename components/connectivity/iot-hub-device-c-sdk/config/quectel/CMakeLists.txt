# Copyright (C) 2022 Tencent Technologies Limited.
# All rights reserved.
#

set(target ql_qcloud_iot)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_app_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_QCLOUD_IOT)

set(PLATFORM "quectel")
set(IOT_SDK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/iot_c_sdk")
include(${CMAKE_CURRENT_SOURCE_DIR}/config/iot_ql_sdk_setting.cmake)

target_include_directories(${target} PUBLIC
	${IOT_SDK_SOURCE_DIR}/include/
	${IOT_SDK_SOURCE_DIR}/include/common
	${IOT_SDK_SOURCE_DIR}/include/config
	${IOT_SDK_SOURCE_DIR}/include/services/common
	${IOT_SDK_SOURCE_DIR}/include/services/hub
	${IOT_SDK_SOURCE_DIR}/include/services/explorer
	${IOT_SDK_SOURCE_DIR}/3rd/mbedtls/mbedtls/include
	${IOT_SDK_SOURCE_DIR}/3rd/mbedtls/port/inc
	${inc_mbedtls_port}
	${inc_common}
	${inc_platform}
	${inc_services}
)
target_sources(${target} PRIVATE qcloud_iot_mqtt_demo.c ${src_mbedtls_port} ${src_common} ${src_platform} ${src_services})

relative_glob(srcs include/*.h src/*.c inc/*.h)
beautify_c_code(${target} ${srcs})
