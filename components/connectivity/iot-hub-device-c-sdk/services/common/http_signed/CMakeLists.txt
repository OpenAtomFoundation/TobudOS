file(GLOB src_http_signed ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(src_services ${src_services} ${src_http_signed} PARENT_SCOPE)