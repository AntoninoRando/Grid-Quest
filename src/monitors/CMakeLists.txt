add_executable(inputsObserver
    inputsObserver.cpp
    ../monitors.cpp
    inputsObserve.cpp
)

target_include_directories(inputsObserver PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(inputsObserver PRIVATE hiredis)
target_link_libraries(inputsObserver PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(inputsObserver PRIVATE pqxx)

add_executable(questAnalyzer
    questAnalyzer.cpp
    ../monitors.cpp
    questAnalyze.cpp
)

target_include_directories(questAnalyzer PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(questAnalyzer PRIVATE hiredis)
target_link_libraries(questAnalyzer PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(questAnalyzer PRIVATE pqxx)

add_executable(sessionTracker
    sessionTracker.cpp
    ../monitors.cpp
    sessionTrack.cpp
)

target_include_directories(sessionTracker PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(sessionTracker PRIVATE hiredis)
target_link_libraries(sessionTracker PRIVATE PostgreSQL::PostgreSQL)
target_link_libraries(sessionTracker PRIVATE pqxx)