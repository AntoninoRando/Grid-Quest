add_executable(inputs
    trackInputs.cpp
    ../monitors.cpp
)

target_include_directories(inputs PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(inputs PRIVATE hiredis)
target_link_libraries(inputs PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(inputs PRIVATE pqxx)

add_executable(stats
    analyzeStats.cpp
    ../monitors.cpp
    questAnalyze.cpp
)

target_include_directories(stats PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(stats PRIVATE hiredis)
target_link_libraries(stats PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(stats PRIVATE pqxx)

add_executable(times
    loadTimes.cpp
    ../monitors.cpp
    sessionTrack.cpp
)

target_include_directories(times PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(times PRIVATE hiredis)
target_link_libraries(times PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(times PRIVATE pqxx)