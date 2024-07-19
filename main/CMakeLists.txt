add_library(main)

target_sources(main PRIVATE
    src/main.cpp
)

target_include_directories(main PUBLIC include)
target_link_libraries(main PRIVATE core)
