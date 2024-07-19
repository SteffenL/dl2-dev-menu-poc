if(NOT DEFINED steamworks_ROOT)
    message(FATAL_ERROR "steamworks_ROOT should be set to the path of Steamworks SDK")
endif()

find_path(steamworks_INCLUDE_DIR NAMES steam_api.h HINTS ${steamworks_ROOT}/sdk/public/steam)

if(NOT steamworks_INCLUDE_DIR)
    message(FATAL_ERROR "Include directory not found")
endif()

add_library(steamworks SHARED IMPORTED)
target_include_directories(steamworks INTERFACE ${steamworks_ROOT}/sdk/public)
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P GREATER 4)
        set_target_properties(steamworks PROPERTIES
            IMPORTED_LOCATION ${steamworks_ROOT}/sdk/redistributable_bin/win64/steam_api64.dll
            IMPORTED_IMPLIB ${steamworks_ROOT}/sdk/redistributable_bin/win64/steam_api64.lib
        )
    else()
        set_target_properties(steamworks PROPERTIES
            IMPORTED_LOCATION ${steamworks_ROOT}/sdk/redistributable_bin/steam_api.dll
            IMPORTED_IMPLIB ${steamworks_ROOT}/sdk/redistributable_bin/steam_api.lib
        )
    endif()
endif()
