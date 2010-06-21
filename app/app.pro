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

LOCALESUBDIR = locale

DEFINES += ORGNAME=\\\"$${ORGNAME}\\\"
DEFINES += APPNAME=\\\"$${APPNAME}\\\"
DEFINES += APPVERSION=\\\"$${APPVERSION}\\\"
DEFINES += KTIKZ_TRANSLATIONS_INSTALL_DIR=\\\"$${RESOURCESDIR}/$${LOCALESUBDIR}\\\"
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
usekde {
	RESOURCES = ktikz.qrc
} else {
	RESOURCES = qtikz.qrc
}
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
		ICONDIR = $$replace(RESOURCESDIR, "/", "\/")
		DESKTOPCREATE = "sed -e \"s/Icon=/Icon=$${ICONDIR}\/qtikz-128.png/\" qtikz.desktop.template > qtikz.desktop"
		system($$DESKTOPCREATE)
		desktop.files = qtikz.desktop
		resources.files += images/qtikz-128.png
	}
	desktop.path = $${DESKTOPDIR}
	INSTALLS += desktop
}

### Translations

LOCALEDIR = $${LOCALESUBDIR}/ # the function qmFiles assumes that this variable ends with / or is empty

!isEmpty(TRANSLATIONS) {
	updateqm.name = lrelease ${QMAKE_FILE_IN}
	updateqm.input = TRANSLATIONS
	updateqm.output = $${LOCALEDIR}${QMAKE_FILE_BASE}.qm
	updateqm.commands = $${LRELEASECOMMAND} -silent ${QMAKE_FILE_IN} -qm $${LOCALEDIR}${QMAKE_FILE_BASE}.qm; $${QMAKECOMMAND} $${_PRO_FILE_}
	updateqm.CONFIG = no_link target_predeps
	QMAKE_EXTRA_COMPILERS += updateqm

	translations.path = $${RESOURCESDIR}/$${LOCALESUBDIR}
	translations.files += $$qmFiles($${TRANSLATIONS})
	INSTALLS += translations
}

resources.path = $${RESOURCESDIR}
resources.files += ../examples/template_example.pgs
INSTALLS += resources
