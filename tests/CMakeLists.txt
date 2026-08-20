include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/tests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cpphal.cmake)

enable_testing()

add_mcu_target(stm32f103rb GENERATED_DIR)

add_subdirectory(compile)