cmake_minimum_required(VERSION 3.21)
project(cchip8)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(cchip8 main.cpp)

target_link_libraries(cchip8 PRIVATE glfw)
target_link_libraries(cchip8 PRIVATE glm::glm)