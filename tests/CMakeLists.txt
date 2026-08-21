include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/tests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cpphal.cmake)

enable_testing()

add_mcu_target(stm32f103rb GENERATED_DIR)

add_custom_target(hal_compile_tests
        COMMAND ${CMAKE_CTEST_COMMAND}
        --output-on-failure
        -R "\"^(basic_|pwm_|rcc_|gpio_)\""
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_subdirectory(compile)