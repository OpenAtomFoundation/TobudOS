# ##################### CONFIG  #####################################

# 开启单元测试
set(CONFIG_IOT_TEST OFF)

# 开启示例编译
set(CONFIG_COMPILE_SAMPLE OFF)

# 打开IOT DEBUG
set(CONFIG_IOT_DEBUG OFF)

# 代码抽取，ON表示根据配置抽取源码到ouput/sdk目录
set(CONFIG_EXTRACT_SRC ON)

# 接入认证方式，使用证书认证：CERT；使用密钥认证：KEY
set(CONFIG_AUTH_MODE "KEY")

# 接入认证是否不使用TLS，证书方式必须选择使用TLS，密钥认证可选择不使用TLS
set(CONFIG_AUTH_WITH_NOTLS ON)

# 是否打开代码中获取设备信息功能，OFF时将从device_info.json中读取设备信息
set(CONFIG_DEBUG_DEV_INFO_USED ON)

# 是否使能多线程
set(CONFIG_MULTITHREAD_ENABLED ON)

# 是否使能AT模组
set(CONFIG_AT_MODULE_ENABLED ON)

option(IOT_DEBUG "Enable IOT_DEBUG" ${CONFIG_IOT_DEBUG})
option(DEBUG_DEV_INFO_USED "Enable DEBUG_DEV_INFO_USED" ${CONFIG_DEBUG_DEV_INFO_USED})
option(AUTH_WITH_NO_TLS "Enable AUTH_WITH_NO_TLS" ${CONFIG_AUTH_WITH_NOTLS})
option(MULTITHREAD_ENABLED "Enable AUTH_WITH_NO_TLS" ${CONFIG_MULTITHREAD_ENABLED})
option(AT_MODULE_ENABLE "Enable AUTH_WITH_NO_TLS" ${CONFIG_AT_MODULE_ENABLED})

if(${CONFIG_AUTH_MODE} STREQUAL "KEY")
	option(AUTH_MODE_KEY "Enable AUTH_MODE_KEY" ON)
	option(AUTH_MODE_CERT "Enable AUTH_MODE_CERT" OFF)
elseif(${CONFIG_AUTH_MODE} STREQUAL "CERT" AND ${CONFIG_AUTH_WITH_NOTLS} STREQUAL "OFF")
	option(AUTH_MODE_KEY "Enable AUTH_MODE_KEY" OFF)
	option(AUTH_MODE_CERT "Enable AUTH_MODE_CERT" ON)
else()
	message(FATAL_ERROR "INVAILD AUTH_MODE:${FEATURE_AUTH_MODE} WITH AUTH_WITH_NO_TLS:${FEATURE_AUTH_WITH_NOTLS}!")
endif()

configure_file(
	"${IOT_SDK_SOURCE_DIR}/config/settings/qcloud_iot_config.h.in"
	"${IOT_SDK_SOURCE_DIR}/include/config/qcloud_iot_config.h"
	@ONLY
)

# export include
include_directories(
	${IOT_SDK_SOURCE_DIR}/include/
	${IOT_SDK_SOURCE_DIR}/include/common
	${IOT_SDK_SOURCE_DIR}/include/config
	${IOT_SDK_SOURCE_DIR}/include/services/common
	${IOT_SDK_SOURCE_DIR}/include/services/explorer
)

# set output path
set(LIBRARY_OUTPUT_PATH ${IOT_SDK_SOURCE_DIR}/output/libs)
set(EXECUTABLE_OUTPUT_PATH ${IOT_SDK_SOURCE_DIR}/output/bin)

# set link lib dir
link_directories(${LIBRARY_OUTPUT_PATH})

# set test src
if(${CONFIG_IOT_TEST} STREQUAL "ON")
	set(src_test CACHE INTERNAL "")
	set(inc_test CACHE INTERNAL "")
endif()

# ##################### PLATFORM MODULE #######################################
set(src_platform CACHE INTERNAL "")
set(inc_platform CACHE INTERNAL "")

add_subdirectory(${IOT_SDK_SOURCE_DIR}/platform)

# add_subdirectory(${IOT_SDK_SOURCE_DIR}/platform/at_client)

# ##################### COMMON MODULE #######################################
set(src_common CACHE INTERNAL "")
set(inc_common CACHE INTERNAL "")

# utils
add_subdirectory(${IOT_SDK_SOURCE_DIR}/common/utils)

# cryptology
add_subdirectory(${IOT_SDK_SOURCE_DIR}/common/cryptology)

# ##################### SERVICE MODULE ####################################
set(src_services CACHE INTERNAL "")
set(inc_services CACHE INTERNAL "")

# mqtt client
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/at_module)

# 是否使能获取iot后台时间功能
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/system)

# 是否使能数据模板功能
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/explorer/data_template)

# 是否使能网关功能
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/gateway)

# ##################### EXTRACT ####################################
if(${CONFIG_EXTRACT_SRC} STREQUAL "ON")
	file(COPY ${src_platform} DESTINATION ${IOT_SDK_SOURCE_DIR}/output/sdk/src)
	file(COPY ${inc_platform} DESTINATION ${IOT_SDK_SOURCE_DIR}/output/sdk/inc/internal)

	file(COPY ${src_common} DESTINATION ${IOT_SDK_SOURCE_DIR}/output/sdk/src)
	file(COPY ${inc_common} DESTINATION ${IOT_SDK_SOURCE_DIR}/output/sdk/inc/internal)

	file(COPY ${src_services} DESTINATION ${IOT_SDK_SOURCE_DIR}/output/sdk/src)
	file(COPY ${inc_services} DESTINATION ${IOT_SDK_SOURCE_DIR}/output/sdk/inc/internal)

	file(GLOB inc_export
		${IOT_SDK_SOURCE_DIR}/include/*.h
		${IOT_SDK_SOURCE_DIR}/include/common/*.h
		${IOT_SDK_SOURCE_DIR}/include/config/*.h
		${IOT_SDK_SOURCE_DIR}/include/services/common/*.h
		${IOT_SDK_SOURCE_DIR}/include/services/explorer/*.h
	)
	file(COPY ${inc_export} DESTINATION ${IOT_SDK_SOURCE_DIR}/output/sdk/inc)
endif()
