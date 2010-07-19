kde4_create_handbook(index.docbook
	INSTALL_DESTINATION ${HTML_INSTALL_DIR}/en
	SUBDIR ktikz)

install(FILES ktikz.1 DESTINATION ${MAN_INSTALL_DIR}/man1)

add_custom_target(uninstalldirs_doc)
add_custom_command(TARGET uninstalldirs_doc
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove ${HTML_INSTALL_DIR}/en/ktikz/common
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${HTML_INSTALL_DIR}/en/ktikz
	COMMENT "Removing directory ${HTML_INSTALL_DIR}/en/ktikz"
	VERBATIM)
