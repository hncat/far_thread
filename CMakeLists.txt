cmake_minimum_required(VERSION 3.5)
project(yf_thread)

list(APPEND _sources mutex.cc sem.cc cond_var.cc shared_mutex.cc)
message(STATUS "_sources: ${_sources}")

# 设置c++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 生成提示用的
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)
message(STATUS "library dir: ${CMAKE_SOURCE_DIR}/libs")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# 生成对象库
add_library(thread_obj OBJECT ${_sources})
target_link_libraries(thread_obj PRIVATE pthread)
set_target_properties(thread_obj PROPERTIES POSITION_INDEPENDENT_CODE 1)

# 生成静态库
add_library(yf_thread_static STATIC $<TARGET_OBJECTS:thread_obj>)
# 生成动态库
add_library(yf_thread_shared SHARED $<TARGET_OBJECTS:thread_obj>)

add_subdirectory(test)
