cmake_minimum_required(VERSION 3.20)

project(TinyVM)

# ==========================================================
# C++ Standard
# ==========================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================================
# Dependencies
# ==========================================================

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)

find_program(CARGO_EXECUTABLE cargo REQUIRED)

# ==========================================================
# Rust Memory Library
# ==========================================================

set(MEMORY_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/TinyKernel/memory
)

set(TVM_MEMORY_LIB
    ${MEMORY_DIR}/target/release/libtvm_memory.a
)

add_custom_target(
    tvm_memory
    COMMAND ${CARGO_EXECUTABLE} build --release
    WORKING_DIRECTORY ${MEMORY_DIR}
    COMMENT "Building Rust memory library..."
)

# ==========================================================
# TinyKernel
# ==========================================================

add_library(TinyKernel
    Src/TinyKernel/kernel.c
    Src/TinyKernel/process.c

    Src/Platform/fileio.c
)

target_include_directories(TinyKernel PUBLIC
    Src/TinyKernel
    Src/TinyKernel/include
)

add_dependencies(TinyKernel
    tvm_memory
)

target_link_libraries(TinyKernel PRIVATE
    ${TVM_MEMORY_LIB}
)

# ==========================================================
# Tiny Shell
# ==========================================================

add_library(tiny_shell
    Src/shell/prompt.c
    Src/shell/shell.cpp
)

target_include_directories(tiny_shell PUBLIC
    Src/shell
)

target_link_libraries(tiny_shell PRIVATE
    TinyKernel
)

# ==========================================================
# Tiny Console
# ==========================================================

add_executable(TinyConsole
    Src/console/main.cpp
    Src/console/window.cpp
    Src/console/fonts.cpp
    Src/console/renderer.cpp
    Src/console/BootLogo.cpp
    Src/console/c_api.cpp

    Src/Platform/platform.cpp
    Src/Platform/fileio.c
)

target_include_directories(TinyConsole PRIVATE
    Src/console
    Src/Platform
)

target_link_libraries(TinyConsole PRIVATE
    TinyKernel
    tiny_shell

    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_image::SDL3_image
)

# ==========================================================
# TinyVM
# ==========================================================

add_executable(TinyVM
    Src/TinyVM/tinyvm.cpp
    Src/Platform/platform.cpp
)

target_include_directories(TinyVM PRIVATE
    Src/Platform
)

target_link_libraries(TinyVM PRIVATE
    TinyKernel
)