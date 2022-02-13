lessThan(QT_VERSION, 5.0) {
	error("QtikZ requires Qt version 5.0 or higher.")
}

ORGNAME = "Florian_Hackenberger"
APPNAME = qtikz
APPEXENAME = qtikz
APPVERSION = 0.12

include(qmake/qtikzconfig.pri)
include(qmake/qtikzdefaults.pri)
include(qmake/qtikzmacros.pri)
include(qmake/findpoppler.pri)

include(app/app.pri)
include(translations/translations.pri)
include(doc/doc.pri)
include(data/data.pri)
include(win32/win32.pri)

message(*** QtikZ v$${APPVERSION} ***)
message(Qt version: $$[QT_VERSION])
message(The program will be installed in)
message("  $${BIN_INSTALL_DIR}")
message(The resource files will be installed in)
message("  $${RESOURCES_INSTALL_DIR}")
message(The documentation will be installed in)
message("  $${DOCUMENTATION_INSTALL_DIR}")
unix:!macx {
	message(The desktop file will be installed in)
	message("  $${DESKTOP_INSTALL_DIR}")
	message(The mimetype will be installed in)
	message("  $${MIME_INSTALL_DIR}")
	message(The man page will be installed in)
	message("  $${MAN_INSTALL_DIR}")
}
message("If you would like to change these paths,")
message("please adjust qtikzconfig.pri to your needs and rerun qmake.")

license.path = $${RESOURCES_INSTALL_DIR}
license.files += LICENSE.GPL2
INSTALLS += license

unix:QMAKE_EXTRA_TARGETS += PACKAGE
PACKAGE.target = package
PACKAGE.commands = "tar --exclude='debian-package' --exclude='*/.svn*' --exclude='moc_*' -cf - \
Changelog \
CMakeLists.txt \
KtikzConfig.cmake \
KtikzCPackOptions.cmake \
Doxyfile \
INSTALL \
qtikz.pro \
qmake \
TODO \
app/CMakeLists.txt \
app/*.h \
app/*.cpp \
app/*.ts \
app/*.qrc \
app/*.desktop.template \
app/*.pri \
app/*.rc \
app/*.ui \
app/*.xml \
templates \
app/icons \
common \
data \
doc \
translations \
part | tardy -Remove_Prefix ktikz -Prefix qtikz-$${APPVERSION} - - | gzip -c > qtikz-$${APPVERSION}.tar.gz"
