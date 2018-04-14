include(../KtikzConfig.cmake)

### Install desktop file

install(FILES ktikz.desktop DESTINATION ${KDE_INSTALL_APPDIR})

### Install text/x-pgf mimetype

find_package(SharedMimeInfo)
install(FILES qtikz.xml DESTINATION ${KDE_INSTALL_MIMEDIR} RENAME ktikz.xml)
update_xdg_mimetypes(${KDE_INSTALL_MIMEDIR})

### Install examples

install(FILES examples/template_example.pgs
	examples/template_example2.pgs
	examples/beamer-example-template.pgs
	DESTINATION ${KTIKZ_TEMPLATES_INSTALL_DIR})

### Install appdata file

install(FILES ktikz.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
