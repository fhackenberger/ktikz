include(../conf.pri)
include(../defaults.pri)
include(../macros.pri)

TEMPLATE = app
CONFIG += warn_on \
	thread \
	qt
QT += xml
LIBS += -lpoppler-qt4

DEFINES += APPVERSION=\\\"$${APPVERSION}\\\"
DEFINES += KTIKZ_INSTALL_TRANSLATIONS=\\\"$${TRANSLATIONSDIR}\\\"

### Build files

DESTDIR = ../bin
MOC_DIR = ../build/moc
OBJECTS_DIR = ../build/obj
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui

### Input

FORMS += editgotolinewidget.ui \
	editindentdialog.ui \
	editreplacewidget.ui \
	templatewidget.ui
#SOURCES += $$formSources($$FORMS) \ # linguist does not use translations in corresponding cpp files if we use this :-(
SOURCES += aboutdialog.cpp \
	colorbutton.cpp \
	configappearancewidget.cpp \
	configdialog.cpp \
	editgotolinewidget.cpp \
	editindentdialog.cpp \
	editreplacewidget.cpp \
	ktikz.cpp \
	loghighlighter.cpp \
	logtextedit.cpp \
	main.cpp \
	templatewidget.cpp \
	tikzcommandinserter.cpp \
	tikzcommandwidget.cpp \
	tikzeditor.cpp \
	tikzeditorhighlighter.cpp \
	tikzeditorview.cpp \
	tikzpngpreviewer.cpp \
	tikzpreview.cpp
HEADERS += $$headerFiles($$SOURCES)
RESOURCES = application.qrc
TRANSLATIONS = ktikz_de.ts ktikz_es.ts ktikz_fr.ts

### Output

TARGET = ktikz
target.path = $${BINDIR}
INSTALLS += target

unix:!macx {
	ICONDIR = $$replace(TRANSLATIONSDIR, "/", "\/")
	DESKTOPCREATE = "sed -e \"s/Icon=/Icon=$${ICONDIR}\/ktikz-128.png/\" ktikz.desktop.template > ktikz.desktop"
	system($$DESKTOPCREATE)
	desktop.path = $${DESKTOPDIR}
	desktop.files = ktikz.desktop
	INSTALLS += desktop
}

### Translations

translationscreate.commands = $$LRELEASECOMMAND $$TRANSLATIONS; $$QMAKECOMMAND
translationscreate.target = translations
QMAKE_EXTRA_TARGETS = translationscreate

QMAKE_CLEAN += $$qmFiles($$TRANSLATIONS)

translations.path = $${TRANSLATIONSDIR}
translations.files = $$qmFiles($$TRANSLATIONS) images/ktikz-128.png template_example.pgs
INSTALLS += translations
