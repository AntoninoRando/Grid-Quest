# Setting up GoogleTest --------------------------------------------------------

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
  # GIT_REPOSITORY https://github.com/google/googletest.git
  # GIT_TAG        5376968f6948923e2411081fd9372e71a59d8e77
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# ------------------------------------------------------------------------------

add_executable(tests
  main.cpp
  grid_tests.cpp
  ../src/grid.cpp
  ../src/cursor.cpp
  ../src/grid.cpp
  ../src/settings.cpp
  ../src/state.cpp
  ../src/utils.cpp
  ../src/menu.cpp
  ../src/monitors.cpp
)

find_package(PostgreSQL REQUIRED)
include_directories (${PostgreSQL_INCLUDE_DIRS})
link_directories(${PostgreSQL_LIBRARY_DIRS})

target_include_directories(tests PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(tests
PRIVATE
  GTest::gtest
  GTest::gtest_main
  hiredis
  PostgreSQL::PostgreSQL
  pqxx)

if (WIN32)
  add_custom_command(
    TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
    COMMAND_EXPAND_LISTS
  )
endif ()