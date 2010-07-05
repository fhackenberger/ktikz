include(../conf.pri)
include(../defaults.pri)
include(../macros.pri)

TEMPLATE = app
CONFIG += warn_on \
	thread \
	qt
QT += xml
LIBS += -lpoppler-qt4

LOCALESUBDIR = locale
TEMPLATESUBDIR = templates

DEFINES += ORGNAME=\\\"$${ORGNAME}\\\"
DEFINES += APPNAME=\\\"$${APPNAME}\\\"
DEFINES += APPVERSION=\\\"$${APPVERSION}\\\"
DEFINES += KTIKZ_TRANSLATIONS_INSTALL_DIR=\\\"$${RESOURCESDIR}/$${LOCALESUBDIR}\\\"
DEFINES += KTIKZ_TEMPLATES_INSTALL_DIR=\\\"$${RESOURCESDIR}/$${TEMPLATESUBDIR}\\\"
DEFINES += KTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\\\"$${TIKZ_DOCUMENTATION_DEFAULT}\\\"

### Build files

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc
UI_DIR = ui

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
RESOURCES = qtikz.qrc
TRANSLATIONS = qtikz_de.ts qtikz_es.ts qtikz_fr.ts

### Output

TARGET = qtikz
target.path = $${BINDIR}
INSTALLS += target

unix:!macx {
	ICONDIR = $$replace(RESOURCESDIR, "/", "\/")
	DESKTOPCREATE = "sed -e \"s/Icon=/Icon=$${ICONDIR}\/qtikz-128.png/\" qtikz.desktop.template > qtikz.desktop"
	system($$DESKTOPCREATE)

	desktop.path = $${DESKTOPDIR}
	desktop.files = qtikz.desktop
	INSTALLS += desktop

	resources.files += icons/qtikz-128.png
}

### Translations

LOCALEDIR = $${LOCALESUBDIR}/ # the function qmFiles assumes that this variable ends with / or is empty

!isEmpty(TRANSLATIONS) {
	updateqm.name = lrelease ${QMAKE_FILE_IN}
	updateqm.input = TRANSLATIONS
	updateqm.output = $${LOCALEDIR}${QMAKE_FILE_BASE}.qm
	updateqm.commands = $${LRELEASECOMMAND} -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
	updateqm.CONFIG = no_link target_predeps
	QMAKE_EXTRA_COMPILERS += updateqm

	#translations.path = $${RESOURCESDIR}
	#translations.files += $${LOCALESUBDIR}
	translations.path = $${RESOURCESDIR}/$${LOCALESUBDIR}
	translations.files += $$qmFiles($${TRANSLATIONS})
	translations.CONFIG += no_check_exist
	INSTALLS += translations
}

### Other resources

templates.path = $${RESOURCESDIR}/$${TEMPLATESUBDIR}
templates.files += ../examples/template_example.pgs \
	../examples/template_example2.pgs \
	../examples/beamer-example-template.pgs
INSTALLS += templates

resources.path = $${RESOURCESDIR}
INSTALLS += resources

### Mimetype

unix:!macx {
	mimetype.path = $${MIMEDIR}
	mimetype.files += ../common/qtikz.xml
	INSTALLS += mimetype
}
