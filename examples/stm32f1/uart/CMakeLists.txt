cmake_minimum_required(VERSION 3.26)

include(${CMAKE_SOURCE_DIR}/cmake/cpphal.cmake)

cpphal_create(stm32f1_uart  main.cpp)

