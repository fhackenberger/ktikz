unix: {
#	QMAKE_CXXFLAGS += `pkg-config --cflags poppler-qt4`
#	QMAKE_LFLAGS += `pkg-config --libs poppler-qt4` # using this, qmake adds 3 times "--libs" to the LFLAGS, which is not recognized by g++ 4.6
	PKG_CONFIG = $$pkgConfigExecutable()
	greaterThan(QT_MAJOR_VERSION, 4) {
		POPPLERINCLUDES = $$system($$PKG_CONFIG --cflags poppler-qt5)
		POPPLERLIBS = $$system($$PKG_CONFIG --libs poppler-qt5)
	} else {
		POPPLERINCLUDES = $$system($$PKG_CONFIG --cflags poppler-qt4)
		POPPLERLIBS = $$system($$PKG_CONFIG --libs poppler-qt4)
	}
	QMAKE_CXXFLAGS += $$POPPLERINCLUDES
	QMAKE_LFLAGS += $$POPPLERLIBS
}

win32 {
	INCLUDEPATH += $${_PRO_FILE_PWD_}/app $${_PRO_FILE_PWD_}/win32/poppler
	LIBS += -L$${_PRO_FILE_PWD_}/win32/poppler/
}

greaterThan(QT_MAJOR_VERSION, 4) {
	LIBS += -lpoppler-qt5
} else {
	LIBS += -lpoppler-qt4
}
