function(add_compile_fail_test NAME SOURCE MCU_TARGET GENERATED_DIR EXPECTED_ERROR)
    include(${GENERATED_DIR}/device.cmake)
    set(TEST_BINARY "${CMAKE_CURRENT_BINARY_DIR}/${NAME}")

    get_target_property(
            HAL_INCLUDE_DIRS
            cpphal
            INTERFACE_INCLUDE_DIRECTORIES
    )

    get_target_property(
            MCU_INCLUDE_DIRS
            ${MCU_TARGET}
            INTERFACE_INCLUDE_DIRECTORIES
    )

    get_target_property(
            HAL_COMPILE_DEFINITIONS
            cpphal
            INTERFACE_COMPILE_DEFINITIONS
    )

    get_target_property(
            MCU_COMPILE_DEFINITIONS
            ${MCU_TARGET}
            INTERFACE_COMPILE_DEFINITIONS
    )

    set(COMPILE_ARGS -std=c++20 ${HAL_COMPILE_OPTIONS})

    if (MCU_INCLUDE_DIRS)
        foreach (DIR IN LISTS MCU_INCLUDE_DIRS)
            list(APPEND COMPILE_ARGS "-I${DIR}")
        endforeach ()
    endif ()

    if (HAL_INCLUDE_DIRS)
        foreach (DIR IN LISTS HAL_INCLUDE_DIRS)
            list(APPEND COMPILE_ARGS "-I${DIR}")
        endforeach ()
    endif ()

    if (MCU_COMPILE_DEFINITIONS)
        foreach (DEF IN LISTS MCU_COMPILE_DEFINITIONS)
            list(APPEND COMPILE_ARGS "-D${DEF}")
        endforeach ()
    endif ()

    if (HAL_COMPILE_DEFINITIONS)
        foreach (DEF IN LISTS HAL_COMPILE_DEFINITIONS)
            list(APPEND COMPILE_ARGS "-D${DEF}")
        endforeach ()
    endif ()

    list(APPEND COMPILE_ARGS "-DCOMPILE_TEST")
    list(APPEND COMPILE_ARGS "-c")
    list(APPEND COMPILE_ARGS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
    list(APPEND COMPILE_ARGS "-o")
    list(APPEND COMPILE_ARGS "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.o")

    message(STATUS "Adding test ${NAME}")
    string(REPLACE ";" "\\;" COMPILE_ARGS "${COMPILE_ARGS}")
    add_test(
            NAME ${NAME}
            COMMAND
            ${CMAKE_COMMAND}
            -DCXX=${CMAKE_CXX_COMPILER}
            -DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}
            -DEXPECTED_ERROR=${EXPECTED_ERROR}
            -DCOMPILE_ARGS=${COMPILE_ARGS}
            -P ${PROJECT_SOURCE_DIR}/cmake/compile_fail_test.cmake
    )
endfunction()
