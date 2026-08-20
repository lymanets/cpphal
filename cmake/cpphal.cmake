
include(${CMAKE_CURRENT_LIST_DIR}/python.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/gcc_arm.cmake)

get_filename_component(__lib_root ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)

function(add_mcu_target PART dir)
    string(TOLOWER "${PART}" PART_LOWER)
    string(TOUPPER "${PART}" PART_UPPER)

    string(
            REGEX MATCH
            "^STM32([A-Z]?[0-9]+)([A-Z0-9]+)$"
            MATCH
            "${PART_UPPER}"
    )

    if(NOT MATCH)
        message(FATAL_ERROR
                "Invalid MCU name: ${PART_UPPER}\n"
                "Expected format: stm32f103rb"
        )
    endif()

    set(MCU_DEVICE "${CMAKE_MATCH_1}")
    set(MCU_PART "STM32${CMAKE_MATCH_1}${CMAKE_MATCH_2}")

    string(TOLOWER "${MCU_PART}" MCU_PART)
    string(
            REGEX MATCH
            "^STM32([A-Z][0-9])"
            FAMILY_MATCH
            "STM32${MCU_DEVICE}"
    )
    if(NOT FAMILY_MATCH)
        message(FATAL_ERROR
                "Invalid MCU name: ${PART_UPPER}\n"
                "Expected format: stm32f103rb"
        )
    endif()
    set(MCU_FAMILY "${CMAKE_MATCH_1}")

    string(SUBSTRING "${PART}" 10 1 LAST_CHAR)
    set(MANIFEST_NAME "STM32${MCU_DEVICE}x${LAST_CHAR}")
    string(TOLOWER "${MANIFEST_NAME}" MANIFEST_NAME)

    set(DEVICES_DIR ${__lib_root}/devices)
    set(FAMILY_DIR ${DEVICES_DIR}/STM32/${MCU_FAMILY})
    set(MANIFESTS_DIR ${FAMILY_DIR}/manifests)
    set(SVD_FILE ${FAMILY_DIR}/svd/STM32${MCU_DEVICE}.svd)

    set(MANIFEST_FILE "${MANIFESTS_DIR}/${MANIFEST_NAME}.yaml")
    set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated/${PART_LOWER}")
    set(${dir} ${GENERATED_DIR} PARENT_SCOPE)
    if (TARGET cpphal_mcu_${PART_LOWER})
        return()
    endif ()
    file(MAKE_DIRECTORY ${GENERATED_DIR})
    generate_device(${__lib_root} ${DEVICES_DIR} ${MANIFEST_FILE} ${SVD_FILE} ${GENERATED_DIR})
    add_library(cpphal_mcu_${PART_LOWER} INTERFACE)

    add_library(
            cpphal::mcu::${PART_LOWER}
            ALIAS
            cpphal_mcu_${PART_LOWER}
    )

    target_include_directories(
            cpphal_mcu_${PART_LOWER}
            INTERFACE
            "${GENERATED_DIR}/include"
    )

    target_compile_definitions(
            cpphal_mcu_${PART_LOWER}
            INTERFACE
            "${PART_UPPER}=1"
    )

    message(STATUS
            "MCU: ${MCU_PART}"
            " family=${MCU_FAMILY}"
            " device=${MCU_DEVICE}"
    )
endfunction()

function(cpphal_create_firmware TARGET PART SRCS FLASH_BASE VECT_TAB_OFFSET)
    include(${__lib_root}/cmake/hal_configure.cmake)
    add_mcu_target(${PART} GENERATED_DIR)
    string(TOLOWER "${PART}" PART)

    add_executable(${TARGET} ${SRCS} ${__lib_root}/src/cpphal/reset_handler.cpp)
    set(__args -Os)
    if (WITH_DEBUG_INFO)
        set(__args -g -Og)
    endif ()

    hal_configure_target(${TARGET} ${GENERATED_DIR} ${__args})

    target_link_libraries(${TARGET} PRIVATE cpphal cpphal::mcu::${PART})

    target_compile_definitions(${TARGET} PRIVATE FLASH_BASE=${FLASH_BASE} VECT_TAB_OFFSET=${VECT_TAB_OFFSET})

    set(GENERATED_VECTOR_CPP ${CMAKE_CURRENT_BINARY_DIR}/generated/hal_vector_table.cpp)

    add_custom_command(
            OUTPUT ${GENERATED_VECTOR_CPP}

            COMMAND
            ${VENV_PYTHON}
            ${__lib_root}/tools/find_configurator.py
            --header ${CMAKE_CURRENT_SOURCE_DIR}/board.hpp
            --output ${GENERATED_VECTOR_CPP}
            DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/board.hpp

            VERBATIM
    )

    target_sources(${TARGET} PRIVATE ${GENERATED_VECTOR_CPP})

endfunction(cpphal_create_firmware)