cmake_minimum_required(VERSION 3.20)

project(TinyConsole)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

add_executable(TinyConsole
    Src/console/main.cpp
    Src/console/window.cpp
    Src/console/fonts.cpp
    Src/console/renderer.cpp
    Src/console/c_api.cpp

    Src/Platform/platform.cpp
)

target_include_directories(TinyConsole PRIVATE
    Src/console
    Src/Platform
)

target_link_libraries(TinyConsole PRIVATE
    TinyKernel
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

add_library(TinyKernel
        Src/TinyKernel/kernel.c
        Src/TinyKernel/kernel.h
)

