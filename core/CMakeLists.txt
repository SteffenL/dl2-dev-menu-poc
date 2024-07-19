add_library(core)

target_sources(core PRIVATE
    src/cli.cpp
    src/game.cpp
    src/hooks.cpp
    src/log.cpp
    src/main.cpp
    src/registry.cpp
    src/state.cpp
    src/string.cpp
    src/system.cpp
)

target_include_directories(core PUBLIC include)
target_link_libraries(core PRIVATE dbghelp minhook steam)
