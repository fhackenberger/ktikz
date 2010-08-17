ORGNAME = "Florian_Hackenberger"
APPNAME = ktikz
APPVERSION = 0.10

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
	isEmpty(DESKTOPDIR) {
		debug {
			DESKTOPDIR = $$(HOME)/.local/share/applications
		} else {
			DESKTOPDIR = /usr/share/applications
		}
	}
	isEmpty(MIMEDIR) {
		MIMEDIR = /usr/share/mime/packages
	}
	isEmpty(MANDIR) {
		MANDIR = $${PREFIX}/share/man
	}
	isEmpty(BINDIR) {
		BINDIR = $${PREFIX}/bin
	}
	isEmpty(RESOURCESDIR) {
		RESOURCESDIR = $${PREFIX}/share/qtikz
	}
	isEmpty(TEMPLATE_EDITOR) {
		TEMPLATE_EDITOR_DEFAULT = kwrite
	}
}
macx { # untested
	CONFIG += link_prl
	BINDIR = /Applications
	RESOURCESDIR = Contents/Resources
	isEmpty(TEMPLATE_EDITOR) {
		TEMPLATE_EDITOR_DEFAULT = kwrite
	}
}
win32 {
	isEmpty(PREFIX) {
#		PREFIX = $$quote(C:/Program Files/QTikZ)
		PREFIX = "C:/QTikZ"
	}
	isEmpty(BINDIR) {
		BINDIR = $${PREFIX}
	}
	isEmpty(RESOURCESDIR) {
		RESOURCESDIR = $${PREFIX}/data
	}
	isEmpty(TEMPLATE_EDITOR) {
		TEMPLATE_EDITOR_DEFAULT = notepad
	}
}
