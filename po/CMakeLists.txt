FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

IF(NOT GETTEXT_MSGFMT_EXECUTABLE)
	MESSAGE(
"------
                 NOTE: msgfmt not found. Translations will *not* be installed
------")
ELSE(NOT GETTEXT_MSGFMT_EXECUTABLE)

	SET(catalogname ktikz)

	ADD_CUSTOM_TARGET(translations ALL)

	FILE(GLOB PO_FILES ${catalogname}*.po)

	FOREACH(_poFile ${PO_FILES})
		GET_FILENAME_COMPONENT(_poFileName ${_poFile} NAME)
		STRING(REGEX REPLACE "^${catalogname}_?" "" _langCode ${_poFileName} )
		STRING(REGEX REPLACE "\\.po$" "" _langCode ${_langCode} )

		IF( _langCode )
			GET_FILENAME_COMPONENT(_lang ${_poFile} NAME_WE)
			SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

			ADD_CUSTOM_COMMAND(TARGET translations
			    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check -o ${_gmoFile} ${_poFile}
			    DEPENDS ${_poFile})
			    INSTALL(FILES ${_gmoFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_langCode}/LC_MESSAGES/ RENAME ${catalogname}.mo)
		ENDIF( _langCode )

	ENDFOREACH(_poFile ${PO_FILES})

ENDIF(NOT GETTEXT_MSGFMT_EXECUTABLE)
