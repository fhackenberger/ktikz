greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets printsupport

SOURCES += \
	$${PWD}/action.cpp \
	$${PWD}/colorbutton.cpp \
	$${PWD}/combobox.cpp \
	$${PWD}/file.cpp \
	$${PWD}/filedialog.cpp \
	$${PWD}/globallocale.cpp \
	$${PWD}/lineedit.cpp \
	$${PWD}/messagebox.cpp \
	$${PWD}/pagedialog.cpp \
	$${PWD}/printpreviewdialog.cpp \
	$${PWD}/recentfilesaction.cpp \
	$${PWD}/standardaction.cpp \
	$${PWD}/selectaction.cpp \
	$${PWD}/tempdir.cpp \
	$${PWD}/toggleaction.cpp \
	$${PWD}/toolbar.cpp \
	$${PWD}/url.cpp \
	$${PWD}/zoomaction.cpp
HEADERS += \
#	$$headerFiles($$SOURCES) \
	$${PWD}/colordialog.h \
	$${PWD}/fontdialog.h \
	$${PWD}/icon.h \
	$${PWD}/urlcompletion.h
