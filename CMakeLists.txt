cmake_minimum_required(VERSION 3.24)

project(
    dl2_dev_menu
    VERSION 0.5.0
    DESCRIPTION "A proof of concept for using the developer menu in the Steam version of the Dying Light 2 game by Techland."
    HOMEPAGE_URL https://www.steffenl.com/
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND NOT MSVC)
    add_link_options(-static)
endif()

if(MSVC)
    add_compile_options(/utf-8)
endif()

include(FetchContent)

FetchContent_Declare(minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG 91cc9466e383d13a43d7cf33c7c8fdccb27095d3 # CMake support unavailable in v1.3.3 and older
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(minhook)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(core)
add_subdirectory(steam)
add_subdirectory(steam_game)
add_subdirectory(main)
add_subdirectory(wrapper)
add_subdirectory(setup)
add_subdirectory(packaging)
