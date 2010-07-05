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

# install data files (translation files, icons, example files) here:
RESOURCESDIR = $${PREFIX}/share/ktikz

# install desktop file here (*nix only):
#DESKTOPDIR = $$(HOME)/.local/share/applications
DESKTOPDIR = $${PREFIX}/share/applications

# install mimetype here:
#MIMEDIR = /usr/share/mime/packages

# compile in debug mode:
#CONFIG += debug
# compile in release mode:
CONFIG -= debug
CONFIG += release

# qmake command:
QMAKECOMMAND = qmake-qt4
# lrelease command:
LRELEASECOMMAND = lrelease-qt4

# TikZ documentation default file path:
TIKZ_DOCUMENTATION_DEFAULT = "/usr/share/doc/texmf/pgf/pgfmanual.pdf.gz"
