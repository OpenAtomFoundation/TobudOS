option(USE_STATIC_MBEDTLS_LIBRARY "Build mbed TLS static library." ON)
option(USE_SHARED_MBEDTLS_LIBRARY "Build mbed TLS shared library." OFF)
option(LINK_WITH_PTHREAD "Explicitly link mbed TLS library to pthread." OFF)

file(GLOB src_mbedtls  
    ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/library/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/src/*.c
)

if(${CONFIG_EXTRACT_SRC} STREQUAL "ON")
    file(GLOB inc_mbedtls ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls/include/mbedtls/*.h)
    file(GLOB inc_port ${CMAKE_CURRENT_SOURCE_DIR}/port/inc/*h)
    file(COPY ${src_mbedtls} DESTINATION ${PROJECT_SOURCE_DIR}/output/sdk/src/3rd/mbedtls)
    file(COPY ${inc_mbedtls} DESTINATION ${PROJECT_SOURCE_DIR}/output/sdk/inc/3rd/mbedtls)
    file(COPY ${inc_port} DESTINATION ${PROJECT_SOURCE_DIR}/output/sdk/inc/3rd)
endif()

add_library(mbedtls STATIC ${src_mbedtls})
