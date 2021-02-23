cmake_minimum_required(VERSION 2.8)

PROJECT(UIcode)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

FILE(GLOB SOURCE *.cpp)
FILE(GLOB RESOURCE resource/*.cpp)

ADD_LIBRARY(UIcode  ${SOURCE}
                    ${RESOURCE})

execute_process(
    COMMAND ${CMAKE_SOURCE_DIR}/.sync_build.sh HelloStar
)
