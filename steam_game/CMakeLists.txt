add_library(steam_game)

target_sources(steam_game PRIVATE
    src/integration.cpp
)

target_include_directories(steam_game PUBLIC include)
target_link_libraries(steam_game PRIVATE core steam)
