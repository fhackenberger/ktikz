SUBDIRS += src
TEMPLATE = subdirs 
CONFIG += warn_on \
          qt \
          thread

translationscreate.target = translations
translationscreate.CONFIG += recursive
QMAKE_EXTRA_TARGETS = translationscreate

include(conf.pri)
include(defaults.pri)

message(*** KTikZ v$${APPVERSION} ***)
message(The program will be installed in)
message("  $${BINDIR}")
message(The translation files will be installed in)
message("  $${TRANSLATIONSDIR}")
unix:!macx {
	message(The desktop file will be installed in)
	message("  $${DESKTOPDIR}")
}
message()
message("If you would like to change these paths,")
message("please adjust conf.pri to your needs and rerun qmake.")
message()
USEKDEMESSAGE1 = "In order to enable KDE file dialogs in this application,"
USEKDEMESSAGE2 = "unhide the line \"CONFIG += usekde\" in conf.pri."
usekde {
	USEKDEMESSAGE1 = "In order to disable KDE file dialogs in this application,"
	USEKDEMESSAGE2 = "hide the line \"CONFIG += usekde\" in conf.pri."
}
message($${USEKDEMESSAGE1})
message($${USEKDEMESSAGE2})

QMAKE_EXTRA_UNIX_TARGETS += PACKAGE
PACKAGE.target = package
PACKAGE.commands = "tar --exclude='debian-package' --exclude='*/.svn*' --exclude='moc_*' -cf - \
Changelog \
conf.pri \
defaults.pri \
macros.pri \
Doxyfile \
INSTALL \
ktikz.pro \
TODO \
src/*.h \
src/*.cpp \
src/*.qm \
src/*.ts \
src/*.qrc \
src/*.desktop.template \
src/*.pro \
src/*.ui \
src/*.xml \
templates \
src/images | tardy -Remove_Prefix ktikz -Prefix ktikz-$${APPVERSION} - - | gzip -c > ktikz-$${APPVERSION}.tar.gz"
