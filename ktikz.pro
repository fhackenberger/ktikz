SUBDIRS += app
TEMPLATE = subdirs 
CONFIG += warn_on \
          qt \
          thread

include(conf.pri)
include(defaults.pri)

message(*** KTikZ v$${APPVERSION} ***)
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

QMAKE_EXTRA_UNIX_TARGETS += PACKAGE
PACKAGE.target = package
PACKAGE.commands = "tar --exclude='debian-package' --exclude='*/.svn*' --exclude='moc_*' -cf - \
Changelog \
CMakeLists.txt \
conf.pri \
defaults.pri \
macros.pri \
Doxyfile \
INSTALL \
ktikz.pro \
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
part | tardy -Remove_Prefix ktikz -Prefix ktikz-$${APPVERSION} - - | gzip -c > ktikz-$${APPVERSION}.tar.gz"
