add_executable(setup)
target_sources(setup PRIVATE
    src/main.cpp
)
target_link_libraries(setup PRIVATE core steam)

add_custom_command(TARGET setup POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:setup> $<TARGET_FILE_DIR:setup>
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:wrapper> $<TARGET_FILE_DIR:setup>
    COMMAND_EXPAND_LISTS
)

install(FILES $<TARGET_RUNTIME_DLLS:setup> DESTINATION . COMPONENT dl2_dev_menu)
