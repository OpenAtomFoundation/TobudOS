file(GLOB src_log_upload ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(inc_log_upload ${CMAKE_CURRENT_SOURCE_DIR}/inc/)

set(src_services ${src_services} ${src_log_upload} PARENT_SCOPE)
set(inc_services ${inc_services} ${inc_log_upload} PARENT_SCOPE)

file(GLOB src_log_upload_sample ${CMAKE_CURRENT_SOURCE_DIR}/sample/log_upload_sample.c)
add_executable(log_upload_sample ${src_log_upload_sample})
target_link_libraries(log_upload_sample ${libsdk})

if( ${CONFIG_IOT_TEST} STREQUAL "ON")
    file(GLOB src_unit_test ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    set(src_test ${src_test} ${src_unit_test} PARENT_SCOPE)
endif()
