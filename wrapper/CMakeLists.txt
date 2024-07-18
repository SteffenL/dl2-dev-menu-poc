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
    src/dllmain.cpp
    src/exports.cpp
    src/exports.def
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
)

target_link_libraries(wrapper PRIVATE -static core)
set_target_properties(wrapper PROPERTIES OUTPUT_NAME xinput1_3)
