
include(${CMAKE_CURRENT_LIST_DIR}/mcu_targets.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/python.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/gcc_arm.cmake)

get_filename_component(__lib_root ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)

string(TOLOWER "${MCU_TARGET}" __header)
string(SUBSTRING "${MCU_TARGET}" 0 9 __device)

generate_device(${__lib_root}/configs ${__lib_root}/manifests/${__header}.yaml ${__lib_root}/device)

include(${CMAKE_CURRENT_LIST_DIR}/hal_configure.cmake)

function(cpphal_create TARGET SRCS FLASH_BASE VECT_TAB_OFFSET)
    add_executable(${TARGET} ${SRCS} ${__lib_root}/src/cpphal/reset_handler.cpp)

    hal_configure_target(${TARGET} -Os)

    target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include/cpphal)
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${TARGET} PRIVATE cpphal)
    target_compile_definitions(${TARGET} PRIVATE ${MCU_TARGET} FLASH_BASE=${FLASH_BASE} VECT_TAB_OFFSET=${VECT_TAB_OFFSET})

    generate_ast(${TARGET})

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

endfunction(cpphal_create)