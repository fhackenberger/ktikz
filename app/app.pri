TEMPLATE = app
TARGET = $${APPEXENAME}
VERSION = $${APPVERSION}

LANGUAGE = C++
CONFIG += qt warn_on thread
QT += core gui widgets

DEFINES += QT_STL QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY QT_STRICT_ITERATORS QT_NO_URL_CAST_FROM_STRING QT_NO_KEYWORDS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050200
#DEFINES += -Wold-style-cast -Wfloat-equal -Woverloaded-virtual -Wshadow -Wundef -Weffc++ -Wdouble-promotion
#QMAKE_CXXFLAGS += -std=c++11
#QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic -Weffc++ -Wlogical-op -Wmissing-declarations -Wold-style-cast -Woverloaded-virtual -Wshadow -Wstrict-null-sentinel -Wswitch-default -Wuseless-cast -Wzero-as-null-pointer-constant -fmessage-length=0 -fdiagnostics-show-location=every-line
#QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic -Wlogical-op -Wmissing-declarations -Wold-style-cast -Woverloaded-virtual
#QMAKE_CXX = clang

DEFINES += ORGNAME=\\\"$${ORGNAME}\\\"
DEFINES += APPNAME=\\\"$${APPNAME}\\\"
DEFINES += APPVERSION=\\\"$${APPVERSION}\\\"
DEFINES += KTIKZ_DOCUMENTATION_INSTALL_DIR=\\\"$${DOCUMENTATION_INSTALL_DIR}\\\"
DEFINES += KTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\\\"$${TIKZ_DOCUMENTATION_DEFAULT}\\\"
DEFINES += KTIKZ_TRANSLATIONS_INSTALL_DIR=\\\"$${TRANSLATIONS_INSTALL_DIR}\\\"

INCLUDEPATH += $${PWD}/../common/

### Build files

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc
UI_DIR = ui

### Input

include($${PWD}/../common/common.pri)

FORMS += \
	$${PWD}/configappearancewidget.ui \
	$${PWD}/configeditorwidget.ui \
	$${PWD}/configgeneralwidget.ui \
	$${PWD}/configpreviewwidget.ui \
	$${PWD}/editgotolinewidget.ui \
	$${PWD}/editindentwidget.ui \
	$${PWD}/editreplacewidget.ui \
	$${PWD}/usercommandeditdialog.ui
#SOURCES += $$formSources($$FORMS) \ # linguist does not use translations in corresponding cpp files if we use this :-(
SOURCES += \
	$${PWD}/aboutdialog.cpp \
	$${PWD}/assistantcontroller.cpp \
	$${PWD}/configappearancewidget.cpp \
	$${PWD}/configdialog.cpp \
	$${PWD}/configeditorwidget.cpp \
	$${PWD}/configgeneralwidget.cpp \
	$${PWD}/configpreviewwidget.cpp \
	$${PWD}/editgotolinewidget.cpp \
	$${PWD}/editindentwidget.cpp \
	$${PWD}/editreplacewidget.cpp \
	$${PWD}/editreplacecurrentwidget.cpp \
	$${PWD}/ktikzapplication.cpp \
	$${PWD}/linenumberwidget.cpp \
	$${PWD}/loghighlighter.cpp \
	$${PWD}/logtextedit.cpp \
	$${PWD}/main.cpp \
	$${PWD}/mainwindow.cpp \
	$${PWD}/tikzcommandinserter.cpp \
	$${PWD}/tikzcommandwidget.cpp \
	$${PWD}/tikzdocumentationcontroller.cpp \
	$${PWD}/tikzeditor.cpp \
	$${PWD}/tikzeditorhighlighter.cpp \
	$${PWD}/tikzeditorview.cpp \
	$${PWD}/usercommandeditdialog.cpp \
	$${PWD}/usercommandinserter.cpp
HEADERS += $$headerFiles($$SOURCES)
RESOURCES = $${PWD}/qtikz.qrc

### Output

target.path = $${BIN_INSTALL_DIR}
INSTALLS += target
