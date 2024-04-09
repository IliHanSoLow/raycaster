cmake_minimum_required(VERSION 3.10)
project(Raycaster)

add_executable(main src/main.c)

# Link the necessary libraries
target_link_libraries(main GL glfw GLEW m cglm)

# Include directories
target_include_directories(main PUBLIC include/)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
