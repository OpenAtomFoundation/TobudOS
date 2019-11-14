option(USE_STATIC_MBEDTLS_LIBRARY "Build mbed TLS static library." ON)
option(USE_SHARED_MBEDTLS_LIBRARY "Build mbed TLS shared library." OFF)
option(LINK_WITH_PTHREAD "Explicitly link mbed TLS library to pthread." OFF)

set(src_crypto
        ./library/aes.c
        ./library/aesni.c
        ./library/arc4.c
        ./library/asn1parse.c
        ./library/asn1write.c
        ./library/base64.c
        ./library/bignum.c
        ./library/blowfish.c
        ./library/camellia.c
        ./library/ccm.c
        ./library/cipher.c
        ./library/cipher_wrap.c
        ./library/cmac.c
        ./library/ctr_drbg.c
        ./library/des.c
        ./library/dhm.c
        ./library/ecdh.c
        ./library/ecdsa.c
        ./library/ecjpake.c
        ./library/ecp.c
        ./library/ecp_curves.c
        ./library/entropy.c
        ./library/entropy_poll.c
        ./library/error.c
        ./library/gcm.c
        ./library/havege.c
        ./library/hmac_drbg.c
        ./library/md.c
        ./library/md2.c
        ./library/md4.c
        ./library/md5.c
        ./library/md_wrap.c
        ./library/memory_buffer_alloc.c
        ./library/oid.c
        ./library/padlock.c
        ./library/pem.c
        ./library/pk.c
        ./library/pk_wrap.c
        ./library/pkcs12.c
        ./library/pkcs5.c
        ./library/pkparse.c
        ./library/pkwrite.c
        ./library/platform.c
        ./library/ripemd160.c
        ./library/rsa.c
        ./library/sha1.c
        ./library/sha256.c
        ./library/sha512.c
        ./library/threading.c
        ./library/timing.c
        ./library/version.c
        ./library/version_features.c
        ./library/xtea.c
)

set(src_x509
        ./library/certs.c
        ./library/pkcs11.c
        ./library/x509.c
        ./library/x509_create.c
        ./library/x509_crl.c
        ./library/x509_crt.c
        ./library/x509_csr.c
        ./library/x509write_crt.c
        ./library/x509write_csr.c
)

set(src_tls
        ./library/debug.c
        ./library/net_sockets.c
        ./library/ssl_cache.c
        ./library/ssl_ciphersuites.c
        ./library/ssl_cli.c
        ./library/ssl_cookie.c
        ./library/ssl_srv.c
        ./library/ssl_ticket.c
        ./library/ssl_tls.c
)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes")
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes -Wdocumentation -Wno-documentation-deprecated-sync -Wunreachable-code")
endif(CMAKE_COMPILER_IS_CLANG)

if(WIN32)
    set(libs ${libs} ws2_32)
endif(WIN32)

if(USE_PKCS11_HELPER_LIBRARY)
    set(libs ${libs} pkcs11-helper)
endif(USE_PKCS11_HELPER_LIBRARY)

if(ENABLE_ZLIB_SUPPORT)
    set(libs ${libs} ${ZLIB_LIBRARIES})
endif(ENABLE_ZLIB_SUPPORT)

if(LINK_WITH_PTHREAD)
    set(libs ${libs} pthread)
endif()

if (NOT USE_STATIC_MBEDTLS_LIBRARY AND NOT USE_SHARED_MBEDTLS_LIBRARY)
    message(FATAL_ERROR "Need to choose static or shared mbedtls build!")
endif(NOT USE_STATIC_MBEDTLS_LIBRARY AND NOT USE_SHARED_MBEDTLS_LIBRARY)

if(USE_STATIC_MBEDTLS_LIBRARY AND USE_SHARED_MBEDTLS_LIBRARY)
    set(mbedtls_static_target "mbedtls_static")
    set(mbedx509_static_target "mbedx509_static")
    set(mbedcrypto_static_target "mbedcrypto_static")
elseif(USE_STATIC_MBEDTLS_LIBRARY)
    set(mbedtls_static_target "mbedtls")
    set(mbedx509_static_target "mbedx509")
    set(mbedcrypto_static_target "mbedcrypto")
endif()



if(USE_STATIC_MBEDTLS_LIBRARY AND ${FEATURE_AUTH_WITH_NOTLS} STREQUAL "OFF")
    SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/${BUILD_TYPE}/lib/)  

    add_library(${mbedcrypto_static_target} STATIC ${src_crypto})
    set_target_properties(${mbedcrypto_static_target} PROPERTIES OUTPUT_NAME mbedcrypto)
    target_link_libraries(${mbedcrypto_static_target} ${libs})

    add_library(${mbedx509_static_target} STATIC ${src_x509})
    set_target_properties(${mbedx509_static_target} PROPERTIES OUTPUT_NAME mbedx509)
    target_link_libraries(${mbedx509_static_target} ${libs} ${mbedcrypto_static_target})

    add_library(${mbedtls_static_target} STATIC ${src_tls})
    set_target_properties(${mbedtls_static_target} PROPERTIES OUTPUT_NAME mbedtls)
    target_link_libraries(${mbedtls_static_target} ${libs} ${mbedx509_static_target})
endif()
