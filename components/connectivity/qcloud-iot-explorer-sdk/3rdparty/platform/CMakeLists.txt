# src platform
file(GLOB src_os	./os/${PLATFORM}/*.c )
set(src_platform  ${src_os})

if(${FEATURE_AUTH_WITH_NOTLS} STREQUAL "OFF")
	file(GLOB src_platform_tls ./tls/mbedtls/*.c)
	list(APPEND src_platform ${src_platform_tls})
endif()

if(AT_TCP_ENABLED STREQUAL "OFF")
	file(GLOB src_os ./os/${PLATFORM}/HAL_AT*.c)
	list(REMOVE_ITEM src_platform ${src_os})
endif()

if(${EXTRACT_SRC} STREQUAL "ON")
	file(COPY ${src_platform} DESTINATION ${PROJECT_SOURCE_DIR}/output/qcloud_iot_c_sdk/platform/)
endif()

# src at_device
if(AT_TCP_ENABLED STREQUAL "ON")
	file(GLOB src_at_device	./at_device/${AT_DEVICE_NAME}/*.c)
endif()

if(AT_TCP_ENABLED STREQUAL "ON" AND ${EXTRACT_SRC} STREQUAL "ON")
	file(COPY ${src_at_device} DESTINATION ${PROJECT_SOURCE_DIR}/output/qcloud_iot_c_sdk/platform/)
endif()

# target
set(platform_target "iot_platform")

# add library
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/${BUILD_TYPE}/lib/)  
if(AT_TCP_ENABLED STREQUAL "ON")
	add_library(${platform_target} STATIC ${src_platform} ${src_at_device})
else()	
	add_library(${platform_target} STATIC ${src_platform})
endif()
