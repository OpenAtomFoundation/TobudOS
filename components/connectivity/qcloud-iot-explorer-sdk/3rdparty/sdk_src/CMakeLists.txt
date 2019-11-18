# �������ã�������Ҫ�����Դ�ļ�

# UTILS
file(GLOB src_utils	${CMAKE_CURRENT_SOURCE_DIR}/utils/*.c)
set(src_sdk  ${src_utils})

# data_template
file(GLOB src_mqtt ${CMAKE_CURRENT_SOURCE_DIR}/protocol/mqtt/*.c)
list(APPEND src_sdk ${src_mqtt})

file(GLOB src_data_template ${CMAKE_CURRENT_SOURCE_DIR}/services/data_template/*.c)
list(APPEND src_sdk ${src_data_template})

# ota mqtt
if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON")
	file(GLOB src_mqtt_ota 
				${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_client.c
				${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_fetch.c
				${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_lib.c
				${CMAKE_CURRENT_SOURCE_DIR}/services/ota/ota_mqtt.c)
	list(APPEND src_sdk ${src_mqtt_ota})
endif()
	

 
# HTTP 
if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON")
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


# ���ɿ⣬��ȡԴ��
if(${EXTRACT_SRC} STREQUAL  "ON")
	file(COPY ${src_sdk} DESTINATION ${PROJECT_SOURCE_DIR}/output/qcloud_iot_c_sdk/sdk_src/)
endif()
set(service_target "iot_sdk")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/${BUILD_TYPE}/lib/)  
add_library(${service_target} STATIC ${src_sdk})
