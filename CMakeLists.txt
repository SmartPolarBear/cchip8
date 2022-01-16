cmake_minimum_required(VERSION 3.21)
project(cchip8 VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 CONFIG REQUIRED)

add_executable(cchip8 main.cpp)

# 3rd party libraries
add_subdirectory(libs)
target_link_libraries(cchip8
        PRIVATE GSL
        PRIVATE argparse
        PRIVATE magic_enum)

# supporting libraries
add_subdirectory(base)

add_subdirectory(processor)
add_subdirectory(display)
add_subdirectory(sound)

target_link_libraries(cchip8 PRIVATE SDL2::SDL2 SDL2::SDL2main)
# use custom entry point
target_compile_definitions(cchip8 PRIVATE -DSDL_MAIN_HANDLED)
