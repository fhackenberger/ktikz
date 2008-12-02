APPVERSION = 0.8

isEmpty(QMAKECOMMAND) {
	QMAKECOMMAND = qmake
}
isEmpty(LRELEASECOMMAND) {
	LRELEASECOMMAND = lrelease
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
	isEmpty(BINDIR) {
		BINDIR = $${PREFIX}/bin
	}
	isEmpty(TRANSLATIONSDIR) {
		TRANSLATIONSDIR = $${PREFIX}/share/ktikz
	}
}
macx { # untested
	CONFIG += link_prl
	BINDIR = /Applications
	TRANSLATIONSDIR = Contents/Resources
}
win32 {
	isEmpty(PREFIX) {
#		PREFIX = "C:\Program Files\KTikZ"
		PREFIX = "C:/KTikZ"
	}
	isEmpty(BINDIR) {
		BINDIR = $${PREFIX}
	}
	isEmpty(TRANSLATIONSDIR) {
		TRANSLATIONSDIR = $${PREFIX}/translations
	}
}
