file(GLOB src_http_client ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(src_services ${src_services} ${src_http_client} PARENT_SCOPE)
