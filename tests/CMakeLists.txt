include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/tests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cpphal.cmake)

enable_testing()

add_mcu_target(stm32f103rb GENERATED_DIR)

add_custom_target(hal_compile_tests
        COMMAND ${CMAKE_CTEST_COMMAND}
        --output-on-failure
        -R "^pwm_"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_compile_fail_test(
        pwm_duplicate_channel
        compile/pwm_duplicate_channel.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "timer::Pwm: channel specified more than once"
)

#add_compile_fail_test(
#        pwm_invalid_mode
#        compile/pwm_invalid_mode.cpp
#        "timer::Pwm: invalid PWM mode"
#)
#
#add_compile_fail_test(
#        pwm_invalid_duty
#        compile/pwm_invalid_duty.cpp
#        "timer::Pwm: initial duty must be in range"
#)
#
add_compile_fail_test(
        pwm_unconfigured_channel
        compile/pwm_unconfigured_channel.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "timer::Pwm: channel is not configured"
)