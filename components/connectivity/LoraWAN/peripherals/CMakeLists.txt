##
##   ______                              _
##  / _____)             _              | |
## ( (____  _____ ____ _| |_ _____  ____| |__
##  \____ \| ___ |    (_   _) ___ |/ ___)  _ \
##  _____) ) ____| | | || |_| ____( (___| | | |
## (______/|_____)_|_|_| \__)_____)\____)_| |_|
## (C)2013-2017 Semtech
##  ___ _____ _   ___ _  _____ ___  ___  ___ ___
## / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
## \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
## |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
## embedded.connectivity.solutions.==============
##
## License:  Revised BSD License, see LICENSE.TXT file included in the project
## Authors:  Johannes Bruder (STACKFORCE), Miguel Luis (Semtech)
##
project(peripherals)
cmake_minimum_required(VERSION 3.6)

#---------------------------------------------------------------------------------------
# Target
#---------------------------------------------------------------------------------------

if(${SECURE_ELEMENT} MATCHES SOFT_SE)
    file(GLOB ${PROJECT_NAME}_SOURCES "*.c" "soft-se/*.c")
else()
    if(${SECURE_ELEMENT} MATCHES LR1110_SE)
        if (${RADIO} MATCHES lr1110)
            file(GLOB ${PROJECT_NAME}_SOURCES "*.c" "lr1110-se/*.c")
        else()
            message(FATAL_ERROR "LR1110_SE secure elemeent can only be used when LR1110 radio is selected.")
        endif()
    elseif((${SECURE_ELEMENT} MATCHES ATECC608A_TNGLORA_SE))
        file(GLOB ${PROJECT_NAME}_SOURCES "*.c"
            "atecc608a-tnglora-se/*.c"
            "atecc608a-tnglora-se/cryptoauthlib/lib/*.c"
            "atecc608a-tnglora-se/cryptoauthlib/lib/basic/*.c"
            "atecc608a-tnglora-se/cryptoauthlib/lib/crypto/*.c"
            "atecc608a-tnglora-se/cryptoauthlib/lib/crypto/hashes/*.c"
            "atecc608a-tnglora-se/cryptoauthlib/lib/hal/atca_hal.c"
            "atecc608a-tnglora-se/cryptoauthlib/lib/host/*.c"
        )
    else()
        message(FATAL_ERROR "No secure-element selected.")
    endif()
endif()

add_library(${PROJECT_NAME} OBJECT EXCLUDE_FROM_ALL ${${PROJECT_NAME}_SOURCES})

if(${SECURE_ELEMENT_PRE_PROVISIONED} MATCHES ON)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DSECURE_ELEMENT_PRE_PROVISIONED)
endif()

if(${SECURE_ELEMENT} MATCHES SOFT_SE)
    target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/soft-se)
else()
    if(${SECURE_ELEMENT} MATCHES LR1110_SE)
        if(${RADIO} MATCHES lr1110)
            target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lr1110-se)
        endif()
    elseif((${SECURE_ELEMENT} MATCHES ATECC608A_TNGLORA_SE))
        target_include_directories( ${PROJECT_NAME} PUBLIC 
            ${CMAKE_CURRENT_SOURCE_DIR}/atecc608a-tnglora-se
            ${CMAKE_CURRENT_SOURCE_DIR}/atecc608a-tnglora-se/cryptoauthlib/lib
            ${CMAKE_CURRENT_SOURCE_DIR}/atecc608a-tnglora-se/cryptoauthlib/lib/basic
            ${CMAKE_CURRENT_SOURCE_DIR}/atecc608a-tnglora-se/cryptoauthlib/lib/crypto
            ${CMAKE_CURRENT_SOURCE_DIR}/atecc608a-tnglora-se/cryptoauthlib/lib/crypto/hashes
            ${CMAKE_CURRENT_SOURCE_DIR}/atecc608a-tnglora-se/cryptoauthlib/lib/hal
            ${CMAKE_CURRENT_SOURCE_DIR}/atecc608a-tnglora-se/cryptoauthlib/lib/host
        )
    endif()
endif()

target_include_directories( ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:board,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:system,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:mac,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:radio,INTERFACE_INCLUDE_DIRECTORIES>
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
