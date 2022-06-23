###################### CONFIG  #####################################

# 开启单元测试
set(CONFIG_IOT_TEST OFF)

# 开启示例编译
set(CONFIG_COMPILE_SAMPLE OFF)

# 打开IOT DEBUG
set(CONFIG_IOT_DEBUG OFF)

# 代码抽取，ON表示根据配置抽取源码到ouput/sdk目录
set(CONFIG_EXTRACT_SRC OFF)

# 接入认证方式，使用证书认证：CERT；使用密钥认证：KEY
set(CONFIG_AUTH_MODE "KEY")

# 接入认证是否不使用TLS，证书方式必须选择使用TLS，密钥认证可选择不使用TLS
set(CONFIG_AUTH_WITH_NOTLS OFF)

# 使用用户自己的mbedtls库
set(CONFIG_USR_MBEDTLS ON)

# 是否打开代码中获取设备信息功能，OFF时将从device_info.json中读取设备信息
set(CONFIG_DEBUG_DEV_INFO_USED ON)

# 是否使能多线程
set(CONFIG_MULTITHREAD_ENABLED ON)

# 使用JSON格式上报日志 默认使用text格式
set(CONFIG_LOG_UPLOAD_TYPE_JSON ON)

# 使用AES加密上报日志
set(CONFIG_LOG_UPLOAD_AES_ENCRYPT_POST OFF)

# 是否使用动态注册功能
set(CONFIG_DEV_DYN_REG_ENABLED ON)

option(IOT_DEBUG "Enable IOT_DEBUG" ${CONFIG_IOT_DEBUG})
option(DEBUG_DEV_INFO_USED "Enable DEBUG_DEV_INFO_USED" ${CONFIG_DEBUG_DEV_INFO_USED})
option(AUTH_WITH_NO_TLS "Enable AUTH_WITH_NO_TLS" ${CONFIG_AUTH_WITH_NOTLS})
option(LOG_UPLOAD_TYPE_JSON "Enable LOG_UPLOAD_DEBUG" ${CONFIG_LOG_UPLOAD_TYPE_JSON})
option(LOG_UPLOAD_AES_ENCRYPT_POST "Log upload with AES encrypt" ${CONFIG_LOG_UPLOAD_AES_ENCRYPT_POST})
option(DEV_DYN_REG_ENABLED "Enable DEV_DYN_REG_ENABLED" ${CONFIG_DEV_DYN_REG_ENABLED})
option(MULTITHREAD_ENABLED "Enable AUTH_WITH_NO_TLS" ${CONFIG_MULTITHREAD_ENABLED})

if(${CONFIG_AUTH_MODE} STREQUAL  "KEY")
	option(AUTH_MODE_KEY "Enable AUTH_MODE_KEY" ON)
	option(AUTH_MODE_CERT "Enable AUTH_MODE_CERT" OFF)
elseif(${CONFIG_AUTH_MODE} STREQUAL  "CERT" AND ${CONFIG_AUTH_WITH_NOTLS} STREQUAL "OFF")
	option(AUTH_MODE_KEY "Enable AUTH_MODE_KEY" OFF)
	option(AUTH_MODE_CERT "Enable AUTH_MODE_CERT" ON)
else()
	message(FATAL_ERROR "INVAILD AUTH_MODE:${FEATURE_AUTH_MODE} WITH AUTH_WITH_NO_TLS:${FEATURE_AUTH_WITH_NOTLS}!")
endif()

configure_file (
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
	${IOT_SDK_SOURCE_DIR}/include/services/hub
	${IOT_SDK_SOURCE_DIR}/include/services/explorer
)

###################### PLATFORM MODULE #######################################
set(src_platform CACHE INTERNAL "")
set(inc_platform CACHE INTERNAL "")

add_subdirectory(${IOT_SDK_SOURCE_DIR}/platform)

# set include
include_directories(${inc_platform})

###################### COMMON MODULE #######################################
set(src_common CACHE INTERNAL "")
set(inc_common CACHE INTERNAL "")

# mqtt packet
add_subdirectory(${IOT_SDK_SOURCE_DIR}/common/mqtt_packet)

# utils
add_subdirectory(${IOT_SDK_SOURCE_DIR}/common/utils)

# cryptology
add_subdirectory(${IOT_SDK_SOURCE_DIR}/common/cryptology)

# set include
include_directories(${inc_common})

###################### SERVICE MODULE ####################################
set(src_services CACHE INTERNAL "")
set(inc_services CACHE INTERNAL "")

# mqtt client (must include except dynamic register)
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/mqtt_client)

## MQTT ONLY
# 是否打开广播功能
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/hub/broadcast)

# 是否使能获取iot后台时间功能
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/system)

## HTTP ONLY

# 是否使能设备动态注册
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/dynreg)

## MQTT & HTTP

# 是否使能网关功能
#add_subdirectory()

# 是否使能OTA固件升级功能
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/ota)

# 是否使能资源管理功能
#add_subdirectory()

# 是否使能HTTP请求腾讯云后台功能 如果开启了日志上报和动态注册 则必须要使能
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/http_signed)

# 是否使能日志上报云端功能
add_subdirectory(${IOT_SDK_SOURCE_DIR}/services/common/log_upload)

# set include
include_directories(${inc_services})

###################### MBEDTLS MODULE ####################################
if(${CONFIG_AUTH_WITH_NOTLS} STREQUAL "OFF" AND ${CONFIG_USR_MBEDTLS} STREQUAL "OFF")
	add_definitions("-DMBEDTLS_CONFIG_FILE=\"qcloud_iot_tls_psk_config.h\"")
	add_subdirectory(${IOT_SDK_SOURCE_DIR}/3rd/mbedtls)
	set(src_mbedtls_port "")
	set(int_mbedtls_port "")
else(${CONFIG_USR_MBEDTLS} STREQUAL "ON")
	set(src_mbedtls_port ${IOT_SDK_SOURCE_DIR}/3rd/mbedtls/port/src/qcloud_iot_tls_client.c)
	set(int_mbedtls_port ${IOT_SDK_SOURCE_DIR}/3rd/mbedtls/port/inc/)
endif()
