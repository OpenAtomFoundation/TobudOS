
# UTILS
file(GLOB src_utils	${CMAKE_CURRENT_SOURCE_DIR}/utils/*.c)
set(src_sdk  ${src_utils})

# MQTT
if(${FEATURE_MQTT_COMM_ENABLED} STREQUAL "ON")
	# mqtt
	file(GLOB src_mqtt ${CMAKE_CURRENT_SOURCE_DIR}/protocol/mqtt/*.c)
	list(APPEND src_sdk ${src_mqtt})
	
	# shadow 
	if(${FEATURE_MQTT_DEVICE_SHADOW} STREQUAL "ON")
		file(GLOB src_shadow ${CMAKE_CURRENT_SOURCE_DIR}/services/shadow/*.c)
		list(APPEND src_sdk ${src_shadow})
	endif()
	
	# ota mqtt
	if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON" AND ${FEATURE_OTA_SIGNAL_CHANNEL} STREQUAL "MQTT")
		file(GLOB src_mqtt_ota 
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_client.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_fetch.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_lib.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_mqtt.c)
		list(APPEND src_sdk ${src_mqtt_ota})
	endif()
	
	# gateway
	if(${FEATURE_GATEWAY_ENABLED} STREQUAL "ON")
		file(GLOB src_gateway ${CMAKE_CURRENT_SOURCE_DIR}/services/gateway/*.c)
		list(APPEND src_sdk ${src_gateway})
	endif()
endif()
 
# COAP 
if(${FEATURE_COAP_COMM_ENABLED} STREQUAL "ON")
	# coap
	file(GLOB src_coap ${CMAKE_CURRENT_SOURCE_DIR}/protocol/coap/*.c)
	list(APPEND src_sdk ${src_coap})
	
	# ota coap
	if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON" AND ${FEATURE_OTA_SIGNAL_CHANNEL} STREQUAL "COAP")
		file(GLOB src_coap_ota 
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_client.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_fetch.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_lib.c
					${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_coap.c)
		list(APPEND src_sdk ${src_coap_ota})
	endif()
endif()
 
# HTTP 
if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON" OR ${FEATURE_DEV_DYN_REG_ENABLED} STREQUAL "ON" OR ${FEATURE_LOG_UPLOAD_ENABLED} STREQUAL "ON")
	# http
	file(GLOB src_http 	${CMAKE_CURRENT_SOURCE_DIR}/protocol/http/*.c)
	list(APPEND src_sdk ${src_http})
endif()

# AT OR TCP
if(${FEATURE_AT_TCP_ENABLED} STREQUAL "ON")
	# at
	file(GLOB src_at_socket ${CMAKE_CURRENT_SOURCE_DIR}/network/at_socket/*.c)
	list(APPEND src_sdk ${src_at_socket})
endif()

# TLS
if(${FEATURE_AUTH_WITH_NOTLS} STREQUAL "OFF")
	file(GLOB src_tls ${CMAKE_CURRENT_SOURCE_DIR}/network/tls/*.c)
	list(APPEND src_sdk ${src_tls})
endif()

# NETWORK
# tcp
file(GLOB src_socket ${CMAKE_CURRENT_SOURCE_DIR}/network/socket/*.c)
list(APPEND src_sdk ${src_socket})
# interface
file(GLOB src_network ${CMAKE_CURRENT_SOURCE_DIR}/network/*.c)
list(APPEND src_sdk ${src_network})

# OTHRE SERVICES
if(${FEATURE_LOG_UPLOAD_ENABLED} STREQUAL "ON")
	file(GLOB src_log_upload ${CMAKE_CURRENT_SOURCE_DIR}/services/log/*.c)
	list(APPEND src_sdk ${src_log_upload})
endif()

if(${FEATURE_DEV_DYN_REG_ENABLED} STREQUAL "ON")
	file(GLOB src_dyn_reg ${CMAKE_CURRENT_SOURCE_DIR}/services/dynreg/*.c)
	list(APPEND src_sdk ${src_dyn_reg})
endif()


if(${FEATURE_SYSTEM_COMM_ENABLED} STREQUAL "ON")
	file(GLOB src_system ${CMAKE_CURRENT_SOURCE_DIR}/services/system/*.c)
	list(APPEND src_sdk ${src_system})
endif()


if(${EXTRACT_SRC} STREQUAL  "ON")
	file(COPY ${src_sdk} DESTINATION ${PROJECT_SOURCE_DIR}/output/qcloud_iot_c_sdk/sdk_src/)
endif()
set(service_target "iot_sdk")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/${BUILD_TYPE}/lib/)  
add_library(${service_target} STATIC ${src_sdk})
