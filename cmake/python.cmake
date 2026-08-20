find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(VENV_DIR ${CMAKE_BINARY_DIR}/.venv)

if (WIN32)
    set(VENV_PYTHON ${VENV_DIR}/Scripts/python.exe)
else ()
    set(VENV_PYTHON ${VENV_DIR}/bin/python)
endif ()

if (NOT EXISTS ${VENV_PYTHON})
    execute_process(
            COMMAND ${Python3_EXECUTABLE} -m venv ${VENV_DIR}
            RESULT_VARIABLE result
    )

    if (NOT result EQUAL 0)
        message(FATAL_ERROR "Failed to create Python virtual environment")
    endif ()

    execute_process(
            COMMAND ${VENV_PYTHON} -m pip install --upgrade pip
            COMMAND ${VENV_PYTHON} -m pip install -r ${CMAKE_SOURCE_DIR}/requirements.txt
            COMMAND_ERROR_IS_FATAL ANY
    )
endif ()

function(generate_device root_dir signal_config manifest svd_dir out_dir)
    execute_process(
            COMMAND ${VENV_PYTHON} -m scripts.cli ${signal_config} ${manifest} ${svd_dir} -o ${out_dir}
            WORKING_DIRECTORY ${root_dir}/generator
            COMMAND ${CMAKE_COMMAND} -E touch ${out_dir}/generated.stamp
            COMMAND_ERROR_IS_FATAL ANY
    )
endfunction(generate_device)

