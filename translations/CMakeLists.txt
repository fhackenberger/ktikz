include(../KtikzConfig.cmake)

### Install qm files

file(GLOB ktikz_TS_FILES */qtikz_*.ts)
qt4_add_translation(ktikz_QM_FILES ${ktikz_TS_FILES})
add_custom_target(translations ALL DEPENDS ${ktikz_QM_FILES})
install(FILES ${ktikz_QM_FILES} DESTINATION ${KTIKZ_TRANSLATIONS_INSTALL_DIR})

### Install po files

find_package(Gettext)
if(GETTEXT_FOUND)
	add_subdirectory(es)
	add_subdirectory(fr)
else(GETTEXT_FOUND)
	message(
"------
                 NOTE: Gettext not found. Translations will *not* be installed.
------")
endif(GETTEXT_FOUND)
