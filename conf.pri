# Override the defaults specified in defaults.pri

# installation prefix:
#PREFIX = $$(HOME)/Applications

# install binary here:
#BINDIR = $${PREFIX}/bin

# install translation files here:
#TRANSLATIONSDIR = $${PREFIX}/share/ktikz

# install desktop file here (*nix only):
#DESKTOPDIR = $$(HOME)/.local/share/applications

# compile in debug mode:
#CONFIG += debug
# compile in release mode:
CONFIG -= debug
CONFIG += release

# qmake command:
QMAKECOMMAND = qmake-qt4
# lrelease command:
LRELEASECOMMAND = lrelease-qt4
