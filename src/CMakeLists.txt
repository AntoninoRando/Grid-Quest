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

target_link_libraries(${PROJECT_NAME} PRIVATE hiredis)
target_link_libraries(${PROJECT_NAME} PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(${PROJECT_NAME} PRIVATE pqxx)

add_subdirectory(monitors)