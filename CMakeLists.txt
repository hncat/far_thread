cmake_minimum_required(VERSION 3.5)
project(far_thread)

# 设置c++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 生成提示用的
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)
message(STATUS "library dir: ${CMAKE_SOURCE_DIR}/libs")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# 生成对象库
add_library(far_thread INTERFACE)

target_include_directories(far_thread INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test)
