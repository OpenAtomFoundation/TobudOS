file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(inc ${CMAKE_CURRENT_SOURCE_DIR}/inc/)

set(src_common ${src_common} ${src} PARENT_SCOPE)
set(inc_common ${inc_common} ${inc} PARENT_SCOPE)

if( ${CONFIG_IOT_TEST} STREQUAL "ON")
    file(GLOB src_unit_test ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    set(src_test ${src_test} ${src_unit_test} PARENT_SCOPE)
endif()
