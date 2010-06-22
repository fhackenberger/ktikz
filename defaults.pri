ORGNAME = "Florian\ Hackenberger"
APPNAME = ktikz
APPVERSION = 0.10

isEmpty(QMAKECOMMAND) {
	QMAKECOMMAND = qmake
}
isEmpty(LRELEASECOMMAND) {
	LRELEASECOMMAND = lrelease
}
isEmpty(KDE_INCLUDEDIRS) {
	KDE_PREFIX = $$system(kde4-config --prefix)
	KDE_INCLUDEDIRS = $${KDE_PREFIX}/include/KDE $${KDE_PREFIX}/include
}
isEmpty(KDE_LIBDIRS) {
	KDE_PREFIX = $$system(kde4-config --prefix)
	KDE_LIBDIRS = $${KDE_PREFIX}/lib
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
	isEmpty(RESOURCESDIR) {
		RESOURCESDIR = $${PREFIX}/share/ktikz
	}
}
macx { # untested
	CONFIG += link_prl
	BINDIR = /Applications
	RESOURCESDIR = Contents/Resources
}
win32 {
	isEmpty(PREFIX) {
#		PREFIX = $$quote(C:/Program Files/KTikZ)
		PREFIX = "C:/KTikZ"
	}
	isEmpty(BINDIR) {
		BINDIR = $${PREFIX}
	}
	isEmpty(RESOURCESDIR) {
		RESOURCESDIR = $${PREFIX}/translations
	}
}
