add_executable(setup)
target_sources(setup PRIVATE
    src/main.cpp
)
target_link_libraries(setup PRIVATE core)
