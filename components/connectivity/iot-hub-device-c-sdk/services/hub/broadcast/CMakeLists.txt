file(GLOB src_broadcast ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(inc_broadcast ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(src_services ${src_services} ${src_broadcast} PARENT_SCOPE)
set(inc_services ${inc_services} ${inc_broadcast} PARENT_SCOPE)

file(GLOB src_broadcast_sample ${CMAKE_CURRENT_SOURCE_DIR}/sample/broadcast_sample.c)
add_executable(broadcast_sample ${src_broadcast_sample})
target_link_libraries(broadcast_sample ${libsdk})

if( ${CONFIG_IOT_TEST} STREQUAL "ON")
    file(GLOB src_unit_test ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    set(src_test ${src_test} ${src_unit_test} PARENT_SCOPE)
endif()
