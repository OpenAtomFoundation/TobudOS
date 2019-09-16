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
project(radio)
cmake_minimum_required(VERSION 3.6)

#---------------------------------------------------------------------------------------
# Options
#---------------------------------------------------------------------------------------

# Allow switching of radios
set(RADIO_LIST sx1272 sx1276 sx126x)
set(RADIO sx1272 CACHE STRING "Default radio is sx1272")
set_property(CACHE RADIO PROPERTY STRINGS ${RADIO_LIST})
set_property(CACHE RADIO PROPERTY ADVANCED)

#---------------------------------------------------------------------------------------
# Target
#---------------------------------------------------------------------------------------

file(GLOB ${PROJECT_NAME}_SOURCES "${RADIO}/*.c")

add_library(${PROJECT_NAME} OBJECT EXCLUDE_FROM_ALL ${${PROJECT_NAME}_SOURCES})

add_dependencies(${PROJECT_NAME} board)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/sx126x
    ${CMAKE_CURRENT_SOURCE_DIR}/sx1272
    ${CMAKE_CURRENT_SOURCE_DIR}/sx1276
    $<TARGET_PROPERTY:board,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:system,INTERFACE_INCLUDE_DIRECTORIES>
)

## TEMPORALLY WORKAROUND to allow radio debugging.
## It will be removed with the introduction of a debug-board interface.
option(USE_RADIO_DEBUG "Enable Radio Debug GPIO's" OFF)
target_compile_definitions(${PROJECT_NAME} PUBLIC  $<$<BOOL:${USE_RADIO_DEBUG}>:USE_RADIO_DEBUG>)
target_include_directories(${PROJECT_NAME} PUBLIC $<TARGET_PROPERTY:${BOARD},INTERFACE_INCLUDE_DIRECTORIES>)
##

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
