find_package(steamworks REQUIRED CONFIG)

add_library(steam)

target_sources(steam PRIVATE
    src/ScopedSteamApiInit.cpp
    src/SteamApi.cpp
)

target_include_directories(steam PUBLIC include)
target_link_libraries(steam PRIVATE steamworks)
