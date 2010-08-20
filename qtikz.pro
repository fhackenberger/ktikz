lessThan(QT_VERSION, 4.4) {
	error("QTikZ requires Qt version 4.4 or higher.")
}

SUBDIRS += app
TEMPLATE = subdirs 
CONFIG += warn_on \
          qt \
          thread

include(conf.pri)
include(defaults.pri)

message(*** QTikZ v$${APPVERSION} ***)
message(Qt version: $$[QT_VERSION])
message(The program will be installed in)
message("  $${BINDIR}")
message(The resource files will be installed in)
message("  $${RESOURCESDIR}")
unix:!macx {
	message(The desktop file will be installed in)
	message("  $${DESKTOPDIR}")
	message(The mimetype will be installed in)
	message("  $${MIMEDIR}")
	message(The man page will be installed in)
	message("  $${MANDIR}")
}
message()
message("If you would like to change these paths,")
message("please adjust conf.pri to your needs and rerun qmake.")

ts.target = ts
ts.CONFIG = recursive
QMAKE_EXTRA_TARGETS += ts

license.path = $${RESOURCESDIR}
license.files += LICENSE.GPL2
INSTALLS += license

unix:QMAKE_EXTRA_TARGETS += PACKAGE
PACKAGE.target = package
PACKAGE.commands = "tar --exclude='debian-package' --exclude='*/.svn*' --exclude='moc_*' -cf - \
Changelog \
CMakeLists.txt \
conf.pri \
defaults.pri \
macros.pri \
Doxyfile \
INSTALL \
qtikz.pro \
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
part | tardy -Remove_Prefix ktikz -Prefix qtikz-$${APPVERSION} - - | gzip -c > qtikz-$${APPVERSION}.tar.gz"
