include(../conf.pri)
include(../defaults.pri)
include(../macros.pri)

TEMPLATE = app
CONFIG += warn_on \
	thread \
	qt
QT += xml
LIBS += -lpoppler-qt4
usekde {
	DEFINES += KTIKZ_USE_KDE
	INCLUDEPATH += $${KDE_INCLUDEDIRS} ../common
	LIBS += -lpoppler-qt4 -L$${KDE_LIBDIRS} -lkfile
}

DEFINES += ORGNAME=\\\"$${ORGNAME}\\\"
DEFINES += APPNAME=\\\"$${APPNAME}\\\"
DEFINES += APPVERSION=\\\"$${APPVERSION}\\\"
DEFINES += KTIKZ_TRANSLATIONS_INSTALL_DIR=\\\"$${TRANSLATIONSDIR}\\\"
DEFINES += KTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\\\"$${TIKZ_DOCUMENTATION_DEFAULT}\\\"

### Build files

usekde {
	DESTDIR = ../buildkde
	MOC_DIR = ../buildkde/moc
	OBJECTS_DIR = ../buildkde/obj
	RCC_DIR = ../buildkde/rcc
	UI_DIR = ../buildkde/ui
} else {
	DESTDIR = ../buildqt
	MOC_DIR = ../buildqt/moc
	OBJECTS_DIR = ../buildqt/obj
	RCC_DIR = ../buildqt/rcc
	UI_DIR = ../buildqt/ui
}

### Input

FORMS += configappearancewidget.ui \
	configeditorwidget.ui \
	configgeneralwidget.ui \
	editgotolinewidget.ui \
	editindentdialog.ui \
	editreplacewidget.ui \
	../common/templatewidget.ui
#SOURCES += $$formSources($$FORMS) \ # linguist does not use translations in corresponding cpp files if we use this :-(
SOURCES += ../common/utils/action.cpp \
	../common/utils/colorbutton.cpp \
	../common/utils/filedialog.cpp \
	../common/utils/lineedit.cpp \
	../common/utils/recentfilesaction.cpp \
	../common/utils/standardaction.cpp \
	../common/utils/selectaction.cpp \
	../common/utils/toggleaction.cpp \
	../common/utils/url.cpp \
	aboutdialog.cpp \
	configappearancewidget.cpp \
	configdialog.cpp \
	configeditorwidget.cpp \
	configgeneralwidget.cpp \
	editgotolinewidget.cpp \
	editindentdialog.cpp \
	editreplacewidget.cpp \
	editreplacecurrentwidget.cpp \
	ktikzapplication.cpp \
	loghighlighter.cpp \
	logtextedit.cpp \
	main.cpp \
	mainwindow.cpp \
	tikzcommandinserter.cpp \
	tikzcommandwidget.cpp \
	tikzdocumentationcontroller.cpp \
	tikzeditor.cpp \
	tikzeditorhighlighter.cpp \
	tikzeditorview.cpp \
	../common/templatewidget.cpp \
	../common/tikzpreview.cpp \
	../common/tikzpreviewcontroller.cpp \
	../common/tikzpreviewgenerator.cpp \
	../common/tikzpreviewthread.cpp
HEADERS += $$headerFiles($$SOURCES) \
	../common/mainwidget.h \
	../common/utils/colordialog.h \
	../common/utils/fontdialog.h \
	../common/utils/icon.h
RESOURCES = application.qrc
TRANSLATIONS = ktikz_de.ts ktikz_es.ts ktikz_fr.ts

### Output

usekde {
	TARGET = ktikz
} else {
	TARGET = qtikz
}
target.path = $${BINDIR}
INSTALLS += target

unix:!macx {
	usekde {
		desktop.files = ktikz.desktop
	} else {
		ICONDIR = $$replace(TRANSLATIONSDIR, "/", "\/")
		DESKTOPCREATE = "sed -e \"s/Icon=/Icon=$${ICONDIR}\/qtikz-128.png/\" qtikz.desktop.template > qtikz.desktop"
		system($$DESKTOPCREATE)
		desktop.files = qtikz.desktop
		translations.files += images/qtikz-128.png
	}
	desktop.path = $${DESKTOPDIR}
	INSTALLS += desktop
}

### Translations

translationscreate.commands = $$LRELEASECOMMAND $$TRANSLATIONS; $$QMAKECOMMAND
translationscreate.target = translations
QMAKE_EXTRA_TARGETS = translationscreate

QMAKE_CLEAN += $$qmFiles($$TRANSLATIONS)

translations.path = $${TRANSLATIONSDIR}
translations.files += $$qmFiles($$TRANSLATIONS) template_example.pgs
INSTALLS += translations
