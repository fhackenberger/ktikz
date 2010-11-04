# included by ../app/app.pro

include(../qtikzconfig.pri)
include(../qtikzdefaults.pri)
include(../qtikzmacros.pri)

#TEMPLATESUBDIR = templates

### Desktop file

unix:!macx {
	ICONDIR = $$replace(RESOURCES_INSTALL_DIR, "/", "\\/")
	DESKTOPTEMPLATES = $${PWD}/qtikz.desktop.template

	createdesktop.name = create desktop file
	createdesktop.input = DESKTOPTEMPLATES
	createdesktop.output = ${QMAKE_FILE_BASE}
	createdesktop.commands = sed -e \"s/Icon=/Icon=$${ICONDIR}\\/qtikz-128.png/\" ${QMAKE_FILE_IN} > ${QMAKE_FILE_OUT}
	createdesktop.CONFIG = no_link target_predeps
	QMAKE_EXTRA_COMPILERS += createdesktop

	desktop.path = $${DESKTOP_INSTALL_DIR}
	desktop.files += $${OUT_PWD}/qtikz.desktop
	desktop.CONFIG += no_check_exist
	INSTALLS += desktop

	resources.files += icons/qtikz-128.png
}

### Templates

templates.path = $${RESOURCES_INSTALL_DIR}/$${TEMPLATESUBDIR}
templates.files += examples/template_example.pgs \
	examples/template_example2.pgs \
	examples/beamer-example-template.pgs
templates.files = $$addPrefix("$${PWD}/" $${templates.files})
INSTALLS += templates

### Resources (install resources here so that "make uninstall" tries and succeeds to remove $${RESOURCES_INSTAL_DIR} after everything inside it has been uninstalled)

resources.path = $${RESOURCES_INSTALL_DIR}
INSTALLS += resources

### Mimetype

unix:!macx {
	mimetype.path = $${MIME_INSTALL_DIR}
	mimetype.files += $${PWD}/qtikz.xml
	INSTALLS += mimetype
}
