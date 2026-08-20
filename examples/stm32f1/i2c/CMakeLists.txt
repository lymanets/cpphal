cmake_minimum_required(VERSION 3.26)

include(${CMAKE_SOURCE_DIR}/cmake/cpphal.cmake)

set(srcs "")
list(APPEND srcs main.cpp)

cpphal_create_firmware(stm32f1_i2c stm32f103rb ${srcs} 0x08000000 0)

