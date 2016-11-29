greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets printsupport

include($${_PRO_FILE_PWD_}/qmake/findpoppler.pri)

DEFINES += KTIKZ_TEMPLATE_EDITOR_DEFAULT=\\\"$${TEMPLATE_EDITOR_DEFAULT}\\\"
DEFINES += KTIKZ_TEMPLATES_INSTALL_DIR=\\\"$${TEMPLATES_INSTALL_DIR}\\\"

include($${PWD}/utils/utils.pri)

FORMS += $${PWD}/templatewidget.ui
SOURCES += \
	$${PWD}/templatewidget.cpp \
	$${PWD}/tikzpreview.cpp \
	$${PWD}/tikzpreviewcontroller.cpp \
	$${PWD}/tikzpreviewgenerator.cpp \
	$${PWD}/tikzpreviewmessagewidget.cpp \
	$${PWD}/tikzpreviewrenderer.cpp
HEADERS += \
#	$$headerFiles($$SOURCES) \
	$${PWD}/mainwidget.h \
    $$PWD/textcodecprofile.h
