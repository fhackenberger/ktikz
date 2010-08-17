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
#BINDIR = $${PREFIX}/bin

# install data files (translation files, icons, example files) here:
#RESOURCESDIR = $${PREFIX}/share/qtikz

# install desktop file here (*nix only):
#DESKTOPDIR = $$(HOME)/.local/share/applications
DESKTOPDIR = $${PREFIX}/share/applications

# install mimetype here:
#MIMEDIR = /usr/share/mime/packages

# install man page in the man tree located here:
#MANDIR = $${PREFIX}/share/man

# compile in debug mode:
#CONFIG += debug
# compile in release mode:
CONFIG -= debug
CONFIG += release

# qmake command:
QMAKECOMMAND = qmake-qt4
# lrelease command:
LRELEASECOMMAND = lrelease-qt4
# qcollectiongenerator command:
#QCOLLECTIONGENERATORCOMMAND = qcollectiongenerator

# TikZ documentation default file path:
TIKZ_DOCUMENTATION_DEFAULT = "/usr/share/doc/texmf/pgf/pgfmanual.pdf.gz"

# Default template editor:
#TEMPLATE_EDITOR_DEFAULT = "kwrite"
