include(../KtikzConfig.cmake)

### Install qm files

file(GLOB ktikz_TS_FILES */qtikz_*.ts)
qt4_add_translation(ktikz_QM_FILES ${ktikz_TS_FILES})
add_custom_target(translations ALL DEPENDS ${ktikz_QM_FILES})
install(FILES ${ktikz_QM_FILES} DESTINATION ${KTIKZ_TRANSLATIONS_INSTALL_DIR})

### Install po files

find_package(Gettext)
if(GETTEXT_FOUND)
	add_subdirectory(cs)
	add_subdirectory(de)
	add_subdirectory(es)
	add_subdirectory(fr)
else(GETTEXT_FOUND)
	message(
"------
                 NOTE: Gettext not found. Translations will *not* be installed.
------")
endif(GETTEXT_FOUND)

### Generate ts files (only to be used when ../app/tikzcommands.xml has changed)

set(TIKZCOMMANDS_TR_H tikzcommands_tr.h)
set(XMLPATTERNS xmlpatterns)
set(LUPDATE lupdate)

add_custom_target(ts)
find_program(SED_EXE NAMES sed)
if(SED_EXE)
	add_custom_command(TARGET ts
		COMMAND ${XMLPATTERNS} -output ${TIKZCOMMANDS_TR_H}_tmp ${CMAKE_CURRENT_SOURCE_DIR}/../app/extract-tikzcommands.xq
		COMMAND ${SED_EXE} -e "s/amp\\;//g" ${TIKZCOMMANDS_TR_H}_tmp > ${TIKZCOMMANDS_TR_H}
		COMMAND ${LUPDATE} ${CMAKE_CURRENT_SOURCE_DIR}/../app ${CMAKE_CURRENT_SOURCE_DIR}/../common ${CMAKE_CURRENT_SOURCE_DIR}/../common/utils ${CMAKE_CURRENT_BINARY_DIR}/${TIKZCOMMANDS_TR_H} -ts ${ktikz_TS_FILES}
		COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/${TIKZCOMMANDS_TR_H}_tmp
		COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/${TIKZCOMMANDS_TR_H}
		VERBATIM
	)
else(SED_EXE)
	message(STATUS "Unable to find sed. Please install it if you want to use the \"make ts\" target.")
endif(SED_EXE)
