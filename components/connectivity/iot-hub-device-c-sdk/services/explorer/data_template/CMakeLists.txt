file(GLOB src_system_mqtt ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(inc_system_mqtt ${CMAKE_CURRENT_SOURCE_DIR}/inc/)

set(src_services ${src_services} ${src_system_mqtt} PARENT_SCOPE)
set(inc_services ${inc_services} ${inc_system_mqtt} PARENT_SCOPE)

file(GLOB src_data_template_sample ${CMAKE_CURRENT_SOURCE_DIR}/sample/data_template_sample.c)
add_executable(data_template_sample ${src_data_template_sample})
target_link_libraries(data_template_sample ${libsdk})

if( ${CONFIG_IOT_TEST} STREQUAL "ON")
    file(GLOB src_unit_test ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    set(src_test ${src_test} ${src_unit_test} PARENT_SCOPE)
endif()
