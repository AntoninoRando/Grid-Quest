cmake_minimum_required(VERSION 3.29.2)

project(GridQuest)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

# Disabling hiredis tests
set(DISABLE_TESTS ON)
# Disabling libpqxx tests
set(SKIP_BUILD_TEST ON)
set(INSTALL_TEST OFF)

add_subdirectory(src)
add_subdirectory(requirements/hiredis)
add_subdirectory(requirements/libpqxx)
add_subdirectory(test)