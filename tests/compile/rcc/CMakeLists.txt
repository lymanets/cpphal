include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/tests.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/cpphal.cmake)

add_compile_fail_test(
        rcc_valid
        rcc_valid.cpp
        cpphal::mcu::stm32f103rb
        "NONE"
)

add_compile_fail_test(
        rcc_invalid_ahb
        rcc_invalid_ahb.cpp
        cpphal::mcu::stm32f103rb
        "rcc: Invalid AHB frequency"
)

add_compile_fail_test(
        rcc_invalid_apb1
        rcc_invalid_apb1.cpp
        cpphal::mcu::stm32f103rb
        "rcc: Invalid APB1 frequency"
)

add_compile_fail_test(
        rcc_invalid_apb2
        rcc_invalid_apb2.cpp
        cpphal::mcu::stm32f103rb
        "rcc: Invalid APB2 frequency"
)

add_compile_fail_test(
        rcc_invalid_sysclk
        rcc_invalid_sysclk.cpp
        cpphal::mcu::stm32f103rb
        "rcc: PLL multiplier not found"
)

