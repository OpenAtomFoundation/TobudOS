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

file(GLOB ${PROJECT_NAME}_SOURCES "*.c"
                                  "soft-se/*.c")

add_library(${PROJECT_NAME} OBJECT EXCLUDE_FROM_ALL ${${PROJECT_NAME}_SOURCES})

target_include_directories( ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/soft-se
    $<TARGET_PROPERTY:board,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:system,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:mac,INTERFACE_INCLUDE_DIRECTORIES>
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
