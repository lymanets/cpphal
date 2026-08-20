execute_process(
        COMMAND
        ${CXX}
        ${COMPILE_ARGS}

        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE OUTPUT
        ERROR_VARIABLE ERROR
)

if(RESULT EQUAL 0)
    if ("NONE" MATCHES "${EXPECTED_ERROR}")
        return()
    endif ()
    message(
            FATAL_ERROR
            "Compile-fail test '${SOURCE}' unexpectedly compiled successfully."
    )
endif()

set(DIAGNOSTIC "${OUTPUT}${ERROR}")

if(NOT DIAGNOSTIC MATCHES "${EXPECTED_ERROR}")
    message(
            FATAL_ERROR
            "Compile-fail test '${SOURCE}' failed to compile, "
            "but expected diagnostic was not found.\n\n"
            "Expected:\n"
            "${EXPECTED_ERROR}\n\n"
            "Actual diagnostic:\n"
            "${DIAGNOSTIC}"
    )
endif()