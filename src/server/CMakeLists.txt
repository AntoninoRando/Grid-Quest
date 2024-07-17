add_executable(gridquestserver
    server.cpp
    server_quest.cpp
    ../cursor.cpp
    ../grid.cpp
    ../settings.cpp
    ../utils.cpp
    ../monitors.cpp
)

target_include_directories(gridquestserver PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(gridquestserver PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(gridquestserver PRIVATE pqxx)

# --- LINK LIBRARIES -----------------------------------------------------------

target_link_libraries(gridquestserver PRIVATE hiredis)