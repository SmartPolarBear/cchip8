cmake_minimum_required(VERSION 3.21)
project(cchip8 VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(cchip8 main.cpp)

# 3rd party libraries
add_subdirectory(libs)

add_subdirectory(display)

target_link_libraries(cchip8 PRIVATE glfw)
target_link_libraries(cchip8 PRIVATE glm::glm)