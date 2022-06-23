file(GLOB src_at_module ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(inc_at_module ${CMAKE_CURRENT_SOURCE_DIR}/inc/)

set(src_services ${src_services} ${src_at_module} PARENT_SCOPE)
set(inc_services ${inc_services} ${inc_at_module} PARENT_SCOPE)
