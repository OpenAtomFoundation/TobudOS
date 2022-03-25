# 项目信息
project (QCloud_IoT_SDK C CXX)

# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.5)

# 设置根目录地址
set(IOT_SDK_SOURCE_DIR ${PROJECT_SOURCE_DIR})

# 编译工具链
include(${PROJECT_SOURCE_DIR}/config/toolchains/linux_x86_gcc.cmake)

# 工程配置
include(${PROJECT_SOURCE_DIR}/config/settings/iot_app_all.cmake)
