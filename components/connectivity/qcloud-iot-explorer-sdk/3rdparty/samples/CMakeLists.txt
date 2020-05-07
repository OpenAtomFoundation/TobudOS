
set(INC_DIR  ${PROJECT_SOURCE_DIR}/output/${BUILD_TYPE}/include)
set(LINK_DIR ${PROJECT_SOURCE_DIR}/output/${BUILD_TYPE}/lib/)
include_directories(${INC_DIR})
link_directories(${LINK_DIR})

if(FEATURE_AUTH_WITH_NOTLS)
set(link_lib
	iot_sdk
	iot_platform
)
else()
set(link_lib
	iot_sdk
	iot_platform
	mbedtls
	mbedx509
	mbedcrypto
)
endif()

if(CMAKE_COMPILER_IS_GNUCC)
	set(lib -Wl,--start-group ${link_lib} -Wl,--end-group)
else()
	set(lib ${link_lib})
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/${BUILD_TYPE}/bin)


# data_template
file(GLOB src_data_template_sample 			${PROJECT_SOURCE_DIR}/samples/data_template/data_template_sample.c)
add_executable(data_template_sample 		${src_data_template_sample})
target_link_libraries(data_template_sample 	 	 ${lib})

file(GLOB src_light_data_template_sample	${PROJECT_SOURCE_DIR}/samples/scenarized/light_data_template_sample.c)
add_executable(light_data_template_sample 	${src_light_data_template_sample})
target_link_libraries(light_data_template_sample ${lib})
	
# MQTT
file(GLOB src_mqtt_sample 					${PROJECT_SOURCE_DIR}/samples/mqtt/mqtt_sample.c)
add_executable(mqtt_sample 					${src_mqtt_sample})
target_link_libraries(mqtt_sample 		    ${lib})

# Raw data
file(GLOB src_raw_data 							${PROJECT_SOURCE_DIR}/samples/raw_data/raw_data_sample.c)
add_executable(raw_data_mqtt_sample 			${src_raw_data})
target_link_libraries(raw_data_mqtt_sample 		${lib})

# GATEWAY
if(${FEATURE_GATEWAY_ENABLED} STREQUAL "ON")
	file(GLOB src_gateway_sample  				${PROJECT_SOURCE_DIR}/samples/gateway/*.c)
	add_executable(gateway_sample 				${src_gateway_sample})
	target_link_libraries(gateway_sample 			 ${lib})
endif()

# OTA MQTT
if(${FEATURE_OTA_COMM_ENABLED} STREQUAL "ON")
	file(GLOB src_ota_mqtt_sample 				${PROJECT_SOURCE_DIR}/samples/ota/ota_mqtt_sample.c)
	add_executable(ota_mqtt_sample				${src_ota_mqtt_sample})
	target_link_libraries(ota_mqtt_sample			 ${lib})
endif()


