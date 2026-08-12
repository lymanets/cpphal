if (WIN32)
    set(__WIN32__ ON)
endif ()

if (NOT TOOLCHAIN_DIR)
    if (__WIN32__)
        set(TOOLCHAIN_DIR $ENV{USERPROFILE}/gcc_arm_12)
    else ()
        set(TOOLCHAIN_DIR $ENV{HOME}/gcc_arm_12)
    endif ()
endif ()
string(REPLACE "\\" "/" TOOLCHAIN_DIR "${TOOLCHAIN_DIR}")

message(STATUS "TOOLCHAIN_DIR=${TOOLCHAIN_DIR}")

macro(get_list_dirs result curdir)
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach (child ${children})
        if (IS_DIRECTORY ${curdir}/${child})
            list(APPEND dirlist "${child}")
        endif ()
    endforeach ()
    set(${result} ${dirlist})
endmacro()

if (__WIN32__)
    set(PREFIX_EXE ".exe")
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-mingw-w64-i686-arm-none-eabi.zip")
    set(TOOLCHAIN_NAME_PKG "gcc_arm.zip")
    set(TOOLCHAIN_DIR_ROOT "arm-gnu-toolchain-12.3.rel1-mingw-w64-i686-arm-none-eabi")
elseif (UNIX AND NOT APPLE)
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi.tar.xz")
    set(TOOLCHAIN_NAME_PKG "gcc_arm.tar.xz")
elseif (UNIX AND APPLE)
    set(TOOLCHAIN_URL "https://developer.arm.com/-/media/files/downloads/gnu/12.3.rel1/binrel/arm-gnu-toolchain-12.3.rel1-darwin-arm64-arm-none-eabi.tar.xz")
    set(TOOLCHAIN_NAME_PKG "gcc_arm.tar.xz")
    set(TOOLCHAIN_DIR_ROOT "arm-gnu-toolchain-12.3.rel1-darwin-arm64-arm-none-eabi")
endif ()

macro(move_directory_contents_to_parent dir)
    get_filename_component(parent "${dir}" DIRECTORY)

    file(GLOB children
            "${dir}/*"
            "${dir}/.*"
    )

    foreach (child IN LISTS children)
        get_filename_component(name "${child}" NAME)
        if (NOT name STREQUAL "." AND NOT name STREQUAL "..")
            file(RENAME "${child}" "${parent}/${name}")
        endif ()
    endforeach ()

    file(REMOVE_RECURSE "${dir}")
endmacro()

if (NOT EXISTS ${TOOLCHAIN_DIR})
    file(MAKE_DIRECTORY ${TOOLCHAIN_DIR})
    message("-- Download GNU Arm Embedded Toolchain")
    file(DOWNLOAD ${TOOLCHAIN_URL} ${TOOLCHAIN_DIR}/${TOOLCHAIN_NAME_PKG} SHOW_PROGRESS)
    message("-- Unpacking ${TOOLCHAIN_NAME_PKG}")
    file(ARCHIVE_EXTRACT
            INPUT "${TOOLCHAIN_DIR}/${TOOLCHAIN_NAME_PKG}"
            DESTINATION "${TOOLCHAIN_DIR}"
    )
    message("-- Install GNU Arm Embedded Toolchain")
    move_directory_contents_to_parent("${TOOLCHAIN_DIR}/${TOOLCHAIN_DIR_ROOT}")

    file(REMOVE_RECURSE ${TOOLCHAIN_DIR}/${TOOLCHAIN_NAME_PKG})
endif (NOT EXISTS ${TOOLCHAIN_DIR})

set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_DIR}/bin)
set(TOOLCHAIN_LIBC_DIR ${TOOLCHAIN_DIR}/arm-none-eabi)
set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_LIBC_DIR}/include)
set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_LIBC_DIR}/usr/lib)

set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-gcc${PREFIX_EXE} CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-g++${PREFIX_EXE} CACHE INTERNAL "")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-gcc${PREFIX_EXE})
set(CMAKE_RC_COMPILER ${CMAKE_C_COMPILER})

set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objcopy${PREFIX_EXE} CACHE INTERNAL "")
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objdump${PREFIX_EXE} CACHE INTERNAL "")

set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/arm-none-eabi-size${PREFIX_EXE} CACHE INTERNAL "")

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")

set(CMAKE_C_OUTPUT_EXTENSION ".obj")
set(CMAKE_CXX_OUTPUT_EXTENSION ".obj")

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_OSX_ARCHITECTURES "")
SET(APPLE FALSE)
