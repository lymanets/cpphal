
include(${CMAKE_CURRENT_LIST_DIR}/mcu_targets.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/python.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/gcc_arm.cmake)

get_filename_component(__lib_root ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)

string(TOLOWER "${MCU_TARGET}" __header)
string(SUBSTRING "${MCU_TARGET}" 0 9 __device)

generate_device(${__lib_root}/configs ${__lib_root}/manifests/${__header}.yaml ${__lib_root}/device)

include(${CMAKE_CURRENT_LIST_DIR}/hal_configure.cmake)

function(cpphal_create target sources)
    add_executable(${target} ${sources} ${__lib_root}/src/cpphal/reset_handler.cpp ${HAL_STARTUP})

    hal_configure_target(${target} -Os)

    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/include/cpphal)
    target_link_libraries(${target} PRIVATE cpphal)

    generate_ast(${target})
endfunction(cpphal_create)