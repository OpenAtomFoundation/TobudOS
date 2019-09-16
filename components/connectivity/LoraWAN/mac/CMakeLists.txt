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
project(mac)
cmake_minimum_required(VERSION 3.6)

#---------------------------------------------------------------------------------------
# Options
#---------------------------------------------------------------------------------------

# Allow selection of region
option(REGION_EU868 "Region EU868" ON)
option(REGION_US915 "Region US915" OFF)
option(REGION_CN779 "Region CN779" OFF)
option(REGION_EU433 "Region EU433" OFF)
option(REGION_AU915 "Region AU915" OFF)
option(REGION_AS923 "Region AS923" OFF)
option(REGION_CN470 "Region CN470" OFF)
option(REGION_KR920 "Region KR920" OFF)
option(REGION_IN865 "Region IN865" OFF)
option(REGION_RU864 "Region RU864" OFF)
set(REGION_LIST REGION_EU868 REGION_US915 REGION_CN779 REGION_EU433 REGION_AU915 REGION_AS923 REGION_CN470 REGION_KR920 REGION_IN865 REGION_RU864)

#---------------------------------------------------------------------------------------
# Target
#---------------------------------------------------------------------------------------

file(GLOB ${PROJECT_NAME}_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/region/*.c"
)

add_library(${PROJECT_NAME} OBJECT EXCLUDE_FROM_ALL ${${PROJECT_NAME}_SOURCES})

# Loops through all regions and add compile time definitions for the enabled ones.
foreach( REGION ${REGION_LIST} )
    if(${REGION})
        target_compile_definitions(${PROJECT_NAME} PUBLIC -D"${REGION}")
    endif()
endforeach()

# Add define if class B is supported
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<BOOL:${CLASSB_ENABLED}>:LORAMAC_CLASSB_ENABLED>)

add_dependencies(${PROJECT_NAME} board)

target_include_directories( ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/region
    $<TARGET_PROPERTY:system,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:radio,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:board,INTERFACE_INCLUDE_DIRECTORIES>
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
