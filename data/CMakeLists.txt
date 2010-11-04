include(../KtikzConfig.cmake)

### Install desktop file

install(FILES ktikz.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

### Install text/x-pgf mimetype

find_package(SharedMimeInfo)
install(FILES qtikz.xml DESTINATION ${XDG_MIME_INSTALL_DIR} RENAME ktikz.xml)
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})

### Install examples

install(FILES examples/template_example.pgs
	examples/template_example2.pgs
	examples/beamer-example-template.pgs
	DESTINATION ${KTIKZ_TEMPLATES_INSTALL_DIR})
