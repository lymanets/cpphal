include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/tests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/cpphal.cmake)

#Basic tests
add_compile_fail_test(
        basic_auto_period
        basic_auto_period.cpp
        cpphal::mcu::stm32f103rb
        "timer::Basic: Frequency<> must divide the timer clock exactly"
)

add_compile_fail_test(
        basic_zero_frequency
        basic_zero_frequency.cpp
        cpphal::mcu::stm32f103rb
        "timer::Basic: invalid frequency"
)

add_compile_fail_test(
        basic_frequency_too_high
        basic_frequency_too_high.cpp
        cpphal::mcu::stm32f103rb
        "timer::Basic: frequency cannot be greater than timer clock"
)

add_compile_fail_test(
        basic_valid
        basic_valid.cpp
        cpphal::mcu::stm32f103rb
        "NONE"
)

# Pwm tests
add_compile_fail_test(
        pwm_duplicate_channel
        pwm_duplicate_channel.cpp
        cpphal::mcu::stm32f103rb
        "timer::Pwm: channel specified more than once"
)

add_compile_fail_test(
        pwm_frequency_invalid
        pwm_frequency_invalid.cpp
        cpphal::mcu::stm32f103rb
        "timer::Pwm: invalid frequency"
)

add_compile_fail_test(
        pwm_frequency_greater_than_clock_timer
        pwm_frequency_greater_than_clock_timer.cpp
        cpphal::mcu::stm32f103rb
        "timer::Pwm: frequency cannot be greater than timer clock"
)

add_compile_fail_test(
        pwm_invalid_channel
        pwm_invalid_channel.cpp
        cpphal::mcu::stm32f103rb
        "timer::Pwm: channel should be in range"
)

add_compile_fail_test(
        pwm_invalid_duty
        pwm_invalid_duty.cpp
        cpphal::mcu::stm32f103rb
        "timer::Pwm: initial duty must be in range"
)

add_compile_fail_test(
        pwm_unconfigured_channel
        pwm_unconfigured_channel.cpp
        cpphal::mcu::stm32f103rb
        "timer::Pwm: channel is not configured"
)

add_compile_fail_test(
        pwm_valid
        pwm_valid.cpp
        cpphal::mcu::stm32f103rb
        "NONE"
)