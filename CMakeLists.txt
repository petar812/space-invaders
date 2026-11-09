cmake_minimum_required(VERSION 3.20)

project(SpaceInvaders VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(space_invaders
    src/main.cpp
)

target_link_libraries(space_invaders PRIVATE
    glfw
    GLEW::GLEW
    OpenGL::GL
)
