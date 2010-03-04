# Override the defaults specified in defaults.pri

#################################################
# Please do not commit custom modifications to
# this file into subversion, as we have to build
# a debian package and need sane defaults for
# that purpose
#################################################

# installation prefix:
#PREFIX = $$(HOME)/Applications
PREFIX = /usr

# install binary here:
BINDIR = $${PREFIX}/bin

# install translation files here:
TRANSLATIONSDIR = $${PREFIX}/share/ktikz

# install desktop file here (*nix only):
#DESKTOPDIR = $$(HOME)/.local/share/applications
DESKTOPDIR = $${PREFIX}/share/applications

# compile in debug mode:
#CONFIG += debug
# compile in release mode:
CONFIG -= debug
CONFIG += release

# qmake command:
QMAKECOMMAND = qmake-qt4
# lrelease command:
LRELEASECOMMAND = lrelease-qt4

# use KDE4 file dialogs:
#CONFIG += usekde # hide this to disable KDE file dialogs
#KDE_INCLUDEDIRS = /usr/lib/kde4/include/KDE /usr/lib/kde4/include
#KDE_LIBDIRS = /usr/lib/kde4/lib
