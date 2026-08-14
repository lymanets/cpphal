cmake_minimum_required(VERSION 3.26)

include(${CMAKE_SOURCE_DIR}/cmake/cpphal.cmake)

set(srcs "")
list(APPEND srcs main.cpp)

cpphal_create(stm32f1_uart ${srcs} 0x08000000 0)

