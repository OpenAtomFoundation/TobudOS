file(GLOB src 
    ${CMAKE_CURRENT_SOURCE_DIR}/network/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/os/${PLATFORM}/*.c
)
set(inc ${CMAKE_CURRENT_SOURCE_DIR}/network/inc/)

set(src_platform ${src_platform} ${src} PARENT_SCOPE)
set(inc_platform ${inc_platform} ${inc} PARENT_SCOPE)
