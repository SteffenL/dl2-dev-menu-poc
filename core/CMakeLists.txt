add_library(core)

target_sources(core PRIVATE
    src/game.cpp
    src/hooks.cpp
    src/log.cpp
    src/main.cpp
    src/state.cpp
    src/system.cpp
)

target_include_directories(core PUBLIC include)
target_link_libraries(core PRIVATE dbghelp minhook)
