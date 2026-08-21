include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/tests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/cpphal.cmake)

add_compile_fail_test(
        gpio_input
        gpio_input.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_input_pullup
        gpio_input_pullup.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_input_pulldown
        gpio_input_pulldown.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_analog
        gpio_analog.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_output_push_pull
        gpio_output_push_pull.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_output_open_drain
        gpio_output_open_drain.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_alternate_push_pull
        gpio_alternate_push_pull.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_alternate_open_drain
        gpio_alternate_open_drain.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_multiple_pins
        gpio_multiple_pins.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "NONE"
)

add_compile_fail_test(
        gpio_duplicate_pin
        gpio_duplicate_pin.cpp
        cpphal::mcu::stm32f103rb
        ${GENERATED_DIR}
        "gpio: Pin is already configured"
)