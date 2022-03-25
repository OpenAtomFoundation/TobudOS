file(GLOB src_mqtt_client ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(inc_mqtt_client ${CMAKE_CURRENT_SOURCE_DIR}/inc/)

set(src_services ${src_services} ${src_mqtt_client} PARENT_SCOPE)
set(inc_services ${inc_services} ${inc_mqtt_client} PARENT_SCOPE)

file(GLOB src_mqtt_sample ${CMAKE_CURRENT_SOURCE_DIR}/sample/mqtt_sample.c)
add_executable(mqtt_sample ${src_mqtt_sample})
target_link_libraries(mqtt_sample ${libsdk})

if( ${CONFIG_IOT_TEST} STREQUAL "ON")
    file(GLOB src_unit_test ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    set(inc_mqtt_client_test ${CMAKE_CURRENT_SOURCE_DIR}/test)
    set(src_test ${src_test} ${src_unit_test} PARENT_SCOPE)
    set(inc_test ${inc_test} ${inc_mqtt_client_test} PARENT_SCOPE)
endif()
