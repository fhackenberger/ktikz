include(../qtikzconfig.pri)
include(../qtikzdefaults.pri)
include(../qtikzmacros.pri)

TEMPLATE = app
CONFIG += warn_on \
	thread \
	qt

win32:INCLUDEPATH += $${_PRO_FILE_PWD_} $${_PRO_FILE_PWD_}/../win32/poppler
win32:LIBS += -L$${_PRO_FILE_PWD_}/../win32/poppler/
LIBS += -lpoppler-qt4

LOCALESUBDIR = locale
TEMPLATESUBDIR = templates

DEFINES += ORGNAME=\\\"$${ORGNAME}\\\"
DEFINES += APPNAME=\\\"$${APPNAME}\\\"
DEFINES += APPVERSION=\\\"$${APPVERSION}\\\"
DEFINES += KTIKZ_TRANSLATIONS_INSTALL_DIR=\\\"$${RESOURCES_INSTALL_DIR}/$${LOCALESUBDIR}\\\"
DEFINES += KTIKZ_TEMPLATES_INSTALL_DIR=\\\"$${RESOURCES_INSTALL_DIR}/$${TEMPLATESUBDIR}\\\"
DEFINES += KTIKZ_DOCUMENTATION_INSTALL_DIR=\\\"$${DOCUMENTATION_INSTALL_DIR}\\\"
DEFINES += KTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\\\"$${TIKZ_DOCUMENTATION_DEFAULT}\\\"
DEFINES += KTIKZ_TEMPLATE_EDITOR_DEFAULT=\\\"$${TEMPLATE_EDITOR_DEFAULT}\\\"

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
	editindentwidget.ui \
	editreplacewidget.ui \
	../common/templatewidget.ui
#SOURCES += $$formSources($$FORMS) \ # linguist does not use translations in corresponding cpp files if we use this :-(
SOURCES += ../common/utils/action.cpp \
	../common/utils/colorbutton.cpp \
	../common/utils/file.cpp \
	../common/utils/filedialog.cpp \
	../common/utils/lineedit.cpp \
	../common/utils/recentfilesaction.cpp \
	../common/utils/standardaction.cpp \
	../common/utils/selectaction.cpp \
	../common/utils/toggleaction.cpp \
	../common/utils/url.cpp \
	aboutdialog.cpp \
	assistantcontroller.cpp \
	configappearancewidget.cpp \
	configdialog.cpp \
	configeditorwidget.cpp \
	configgeneralwidget.cpp \
	editgotolinewidget.cpp \
	editindentwidget.cpp \
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
	../common/tikzpreviewthread.cpp \
	../common/tikztemporaryfilecontroller.cpp
HEADERS += $$headerFiles($$SOURCES) \
	../common/mainwidget.h \
	../common/utils/colordialog.h \
	../common/utils/combobox.h \
	../common/utils/fontdialog.h \
	../common/utils/icon.h
RESOURCES = qtikz.qrc

### Output

TARGET = qtikz
target.path = $${BIN_INSTALL_DIR}
INSTALLS += target

### Translations

include(../translations/translations.pri)

### Documentation

include(../doc/doc.pri)

### Desktop file and templates (do this after translations and documentation, because in the following $${RESOURCES_INSTALL_DIR} is removed if empty when uninstalling)

include(../data/data.pri)

### Install icon and dll files

include(../win32/win32.pri)
