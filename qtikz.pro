lessThan(QT_VERSION, 4.6) {
	error("QTikZ requires Qt version 4.6 or higher.")
}

SUBDIRS += app
TEMPLATE = subdirs 
CONFIG += warn_on \
          qt \
          thread

include(qtikzconfig.pri)
include(qtikzdefaults.pri)

message(*** QTikZ v$${APPVERSION} ***)
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
message()
message("If you would like to change these paths,")
message("please adjust qtikzconfig.pri to your needs and rerun qmake.")

ts.target = ts
ts.CONFIG = recursive
QMAKE_EXTRA_TARGETS += ts

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
qtikzconf.pri \
qtikzdefaults.pri \
qtikzmacros.pri \
TODO \
app/CMakeLists.txt \
app/*.h \
app/*.cpp \
app/*.ts \
app/*.qrc \
app/*.desktop.template \
app/*.pro \
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
