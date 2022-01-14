cmake_minimum_required(VERSION 3.21)

target_sources(cchip8
        PRIVATE processor.cpp)

target_include_directories(cchip8 PRIVATE include)