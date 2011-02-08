ORGNAME = "Florian_Hackenberger"
APPNAME = qtikz
APPVERSION = 0.11

isEmpty(QMAKECOMMAND) {
	QMAKECOMMAND = qmake
}
isEmpty(LRELEASECOMMAND) {
	LRELEASECOMMAND = lrelease
}
isEmpty(QCOLLECTIONGENERATORCOMMAND) {
	QCOLLECTIONGENERATORCOMMAND = qcollectiongenerator
}

unix:!macx {
	isEmpty(PREFIX) {
		debug {
			PREFIX = $$(HOME)
		} else {
			PREFIX = /usr/local
		}
	}
	isEmpty(BIN_INSTALL_DIR) {
		BIN_INSTALL_DIR = $${PREFIX}/bin
	}
	isEmpty(DESKTOP_INSTALL_DIR) {
		debug {
			DESKTOP_INSTALL_DIR = $$(HOME)/.local/share/applications
		} else {
			DESKTOP_INSTALL_DIR = /usr/share/applications
		}
	}
	isEmpty(MIME_INSTALL_DIR) {
		MIME_INSTALL_DIR = /usr/share/mime/packages
	}
	isEmpty(RESOURCES_INSTALL_DIR) {
		RESOURCES_INSTALL_DIR = $${PREFIX}/share/qtikz
	}
	isEmpty(DOCUMENTATION_INSTALL_DIR) {
		DOCUMENTATION_INSTALL_DIR = $${RESOURCES_INSTALL_DIR}/documentation
	}
	isEmpty(MAN_INSTALL_DIR) {
		MAN_INSTALL_DIR = $${PREFIX}/share/man
	}
	isEmpty(TEMPLATE_EDITOR) {
		TEMPLATE_EDITOR_DEFAULT = kwrite
	}
}
macx { # untested
	CONFIG += link_prl
	BIN_INSTALL_DIR = /Applications
	RESOURCES_INSTALL_DIR = Contents/Resources
	isEmpty(DOCUMENTATION_INSTALL_DIR) {
		DOCUMENTATION_INSTALL_DIR = $${RESOURCES_INSTALL_DIR}/documentation
	}
	isEmpty(TEMPLATE_EDITOR) {
		TEMPLATE_EDITOR_DEFAULT = kwrite
	}
}
win32 {
	isEmpty(PREFIX) {
#		PREFIX = $$quote(C:/Program Files/QtikZ)
		PREFIX = "C:/QtikZ"
	}
	isEmpty(BIN_INSTALL_DIR) {
		BIN_INSTALL_DIR = $${PREFIX}
	}
	isEmpty(RESOURCES_INSTALL_DIR) {
		RESOURCES_INSTALL_DIR = $${PREFIX}/data
	}
	isEmpty(DOCUMENTATION_INSTALL_DIR) {
		DOCUMENTATION_INSTALL_DIR = $${RESOURCES_INSTALL_DIR}/documentation
	}
	isEmpty(TEMPLATE_EDITOR) {
		TEMPLATE_EDITOR_DEFAULT = notepad
	}
}
