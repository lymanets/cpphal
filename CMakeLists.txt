cmake_minimum_required(VERSION 3.26)

include(cmake/gcc_arm.cmake)

option(WITH_EXAMPLES "Build with examples" ON)
option(WITH_DEBUG_INFO "Build with debug info" OFF)

project(cpphal VERSION 0.1.0 LANGUAGES CXX C ASM)

add_library(${CMAKE_PROJECT_NAME} INTERFACE)

target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/cpphal)

if (WITH_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif ()