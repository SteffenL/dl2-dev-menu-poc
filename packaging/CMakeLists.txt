set(CPACK_PACKAGE_NAME sl_dl2_dev_menu_poc)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Steffen André Langnes")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A proof of concept for using the developer menu in Dying Light 2.")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_COMPONENTS_ALL dl2_dev_menu)
set(CPACK_GENERATOR ZIP)

install(TARGETS wrapper DESTINATION .)

install(FILES scripts/install.bat DESTINATION . COMPONENT dl2_dev_menu)
install(FILES scripts/start_game.bat DESTINATION . COMPONENT dl2_dev_menu)
install(FILES scripts/uninstall.bat DESTINATION . COMPONENT dl2_dev_menu)
install(FILES readme.txt DESTINATION . COMPONENT dl2_dev_menu)

include(CPack)
