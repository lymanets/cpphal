include(${CMAKE_CURRENT_BINARY_DIR}/generated/device.cmake)

function(hal_configure_target target optimization)

    target_link_options(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX,C>:
            --specs=nosys.specs
            --specs=nano.specs
            ${HAL_LINK_OPTIONS}
            -ffunction-sections
            -fdata-sections
            -fstack-usage
            -nostartfiles
            -Wl,-T -Xlinker ${HAL_LINKER_SCRIPT}
            -Wl,-static
            -Wl,--gc-sections
            ${optimization}
            -Wl,-Map -Xlinker ${target}.map
            -Wl,--print-memory-usage
            -lm>
    )

    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-mlittle-endian -std=c++20 -Wall ${optimization}>)
    target_compile_options(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX,C>:
            ${HAL_COMPILE_OPTIONS}
            -nostartfiles
            -fno-exceptions
            -fsigned-char
            -ffunction-sections
            -fdata-sections
            -fno-unwind-tables
            -fno-rtti
            -fno-asynchronous-unwind-tables
            -fmerge-all-constants
            -fno-use-cxa-atexit
            -fipa-icf
            -fno-threadsafe-statics
            -fstack-usage
            -Wno-unused-function
            -Wno-register
            >
    )

    target_include_directories(${target} PRIVATE ${HAL_INCLUDE_DIRECTORIES})

    print_size(${target})
endfunction(hal_configure_target)

function(print_size target)
    add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_SIZE} ARGS $<TARGET_FILE:${target}>)
endfunction()