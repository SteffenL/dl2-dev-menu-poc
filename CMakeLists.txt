cmake_minimum_required(VERSION 3.24)

project(
    dl2_dev_menu
    VERSION 0.1.0
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

if(MSVC)
    add_compile_options(/utf-8)
endif()

include(FetchContent)

FetchContent_Declare(minhook
    GIT_REPOSITORY git@github.com:TsudaKageyu/minhook.git
    GIT_TAG 91cc9466e383d13a43d7cf33c7c8fdccb27095d3 # CMake support unavailable in v1.3.3 and older
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(minhook)

file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
    CONTENT "#include <winver.h>\n\
#pragma code_page(65001)\n\
VS_VERSION_INFO VERSIONINFO\n\
 FILEVERSION ${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},0\n\
 PRODUCTVERSION ${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},0\n\
 FILEFLAGSMASK 0x3fL\n\
#ifdef _DEBUG\n\
 FILEFLAGS 0x1L\n\
#else\n\
 FILEFLAGS 0x0L\n\
#endif\n\
 FILEOS 0x40004L\n\
 FILETYPE 0x2L\n\
 FILESUBTYPE 0x0L\n\
BEGIN\n\
    BLOCK \"StringFileInfo\"\n\
    BEGIN\n\
        BLOCK \"040904b0\"\n\
        BEGIN\n\
            VALUE \"CompanyName\", \"Steffen André Langnes\"\n\
            VALUE \"FileDescription\", \"Wrapper DLL part of a proof of concept for using the developer menu in Dying Light 2.\"\n\
            VALUE \"FileVersion\", \"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.0\"\n\
            VALUE \"InternalName\", \"$<TARGET_FILE_NAME:wrapper>\"\n\
            VALUE \"LegalCopyright\", \"Copyright 2024, Steffen André Langnes\"\n\
            VALUE \"OriginalFilename\", \"$<TARGET_FILE_NAME:wrapper>\"\n\
            VALUE \"ProductName\", \"DL2 Dev Menu PoC\"\n\
            VALUE \"ProductVersion\", \"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.0\"\n\
        END\n\
    END\n\
    BLOCK \"VarFileInfo\"\n\
    BEGIN\n\
        VALUE \"Translation\", 0x409, 1200\n\
    END\n\
END")

add_library(wrapper MODULE)

target_sources(wrapper PRIVATE
    src/exports.def
    src/main.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
)

target_link_libraries(wrapper PRIVATE -static dbghelp minhook)
set_target_properties(wrapper PROPERTIES OUTPUT_NAME xinput1_3)

add_subdirectory(packaging)
