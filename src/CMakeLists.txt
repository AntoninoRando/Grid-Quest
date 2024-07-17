add_executable(${PROJECT_NAME}
    main.cpp
    cursor.cpp
    grid.cpp
    settings.cpp
    state.cpp
    utils.cpp
    menu.cpp
    monitors.cpp
    monitors/sessionTrack.cpp
    monitors/questAnalyze.cpp
    resource.rc
)

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include")

# Linking "libpq" (official C PostgreSQL library), used for "libpqxx" (C++ 
# PostgreSQL library). BE SURE TO HAVE SET THE "bin" DIRECTORY OF THE POSTGRES
# INSTALLATION IN THE PATH ENVIRONMENT VARIABLE.
find_package(PostgreSQL REQUIRED)
include_directories (${PostgreSQL_INCLUDE_DIRS})
link_directories(${PostgreSQL_LIBRARY_DIRS})

# --- NCURSES ------------------------------------------------------------------
# find_package(Curses REQUIRED)
# include_directories(${CURSES_INCLUDE_DIR})

# include(FetchContent)

# FetchContent_Declare(
#   Curses
#   GIT_REPOSITORY https://github.com/mirror/ncurses
#   GIT_TAG        v6.4
# )
# FetchContent_MakeAvailable(Curses)

# include(Curses)


# --- LINK LIBRARIES -----------------------------------------------------------

target_link_libraries(${PROJECT_NAME} PRIVATE hiredis)
target_link_libraries(${PROJECT_NAME} PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(${PROJECT_NAME} PRIVATE pqxx)
# target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_LIBRARIES})

add_subdirectory(monitors)
add_subdirectory(server)