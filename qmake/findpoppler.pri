unix: {
	PKG_CONFIG = $$pkgConfigExecutable()
    POPPLERINCLUDES = $$system($$PKG_CONFIG --cflags poppler-qt5)
    POPPLERLIBS = $$system($$PKG_CONFIG --libs poppler-qt5)
	QMAKE_CXXFLAGS += $$POPPLERINCLUDES
	QMAKE_LFLAGS += $$POPPLERLIBS
}

win32 {
	INCLUDEPATH += $${_PRO_FILE_PWD_}/app $${_PRO_FILE_PWD_}/win32/poppler
	LIBS += -L$${_PRO_FILE_PWD_}/win32/poppler/
}

LIBS += -lpoppler-qt5
