add_executable(${PROJECT_NAME}
    main.cpp
    cursor.cpp
    grid.cpp
    settings.cpp
    state.cpp
    utils.cpp
    menu.cpp
    monitors.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE hiredis)