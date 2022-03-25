file(GLOB src_service_mqtt 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_manage/*.c
)
set(inc_service_mqtt ${CMAKE_CURRENT_SOURCE_DIR}/inc/)

set(src_services ${src_services} ${src_service_mqtt} PARENT_SCOPE)
set(inc_services ${inc_services} ${inc_service_mqtt} PARENT_SCOPE)

file(GLOB src_file_manage_sample ${CMAKE_CURRENT_SOURCE_DIR}/sample/file_manage_sample.c)
add_executable(file_manage_sample ${src_file_manage_sample})
target_link_libraries(file_manage_sample ${libsdk})

if( ${CONFIG_IOT_TEST} STREQUAL "ON")
    file(GLOB src_unit_test ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    set(src_test ${src_test} ${src_unit_test} PARENT_SCOPE)
endif()
