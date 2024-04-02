cmake_minimum_required(VERSION 3.10)
project(Raycaster)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(main src/main.c)

# Link the necessary libraries
target_link_libraries(main GL glfw GLEW m)

# Include directories
target_include_directories(main PUBLIC include/)
