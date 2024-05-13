cmake_minimum_required(VERSION 3.29.2)

project(GridQuest)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_subdirectory(requirements/hiredis)
add_subdirectory(src)