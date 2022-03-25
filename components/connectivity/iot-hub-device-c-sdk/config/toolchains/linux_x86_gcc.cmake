# 编译类型: release/debug
set(BUILD_TYPE "debug")

# 设置CMAKE使用编译工具及编译选项
set(CMAKE_SYSTEM_NAME  Linux)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -pthread -fstack-protector-strong -Wl,-z,now -Wl,-z,noexecstack -fPIE -pie -ffunction-sections -fdata-sections")
set(LINK_FLAGS    "${LINK_FLAGS} -Wl,--gc-sections")

if(${BUILD_TYPE} STREQUAL  "debug")
    set(CMAKE_BUILD_TYPE "Debug")
    list(APPEND CMAKE_MODULE_PATH "${IOT_SDK_SOURCE_DIR}/config/gcovr")
    include(code_coverage)
    append_coverage_compiler_flags()
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -D_FORTIFY_SOURCE=2") # 编译选项  -Wl,-Map,iot.map
endif()

set(PLATFORM "Linux")

set(libsdk  -Wl,--start-group ${libsdk} iot_common iot_services iot_platform pthread -Wl,--end-group)
