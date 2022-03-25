file(GLOB src_system_mqtt ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(src_services ${src_services} ${src_system_mqtt} PARENT_SCOPE)

file(GLOB src_system_mqtt_sample ${CMAKE_CURRENT_SOURCE_DIR}/sample/system_mqtt_sample.c)
add_executable(system_mqtt_sample ${src_system_mqtt_sample})
target_link_libraries(system_mqtt_sample ${libsdk})

if( ${CONFIG_IOT_TEST} STREQUAL "ON")
    file(GLOB src_unit_test ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    set(src_test ${src_test} ${src_unit_test} PARENT_SCOPE)
endif()
