add_executable(inputs
    trackInputs.cpp
    ../monitors.cpp
)

target_include_directories(inputs PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(inputs PRIVATE hiredis)