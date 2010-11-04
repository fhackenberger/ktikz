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
#BIN_INSTALL_DIR = $${PREFIX}/bin

# install data files (translation files, icons, example files) here:
#RESOURCES_INSTALL_DIR = $${PREFIX}/share/qtikz

# install desktop file here (*nix only):
#DESKTOP_INSTALL_DIR = $$(HOME)/.local/share/applications
DESKTOP_INSTALL_DIR = $${PREFIX}/share/applications

# install mimetype here:
#MIME_INSTALL_DIR = /usr/share/mime/packages

#install documentation here:
#DOCUMENTATION_INSTALL_DIR=$${RESOURCES_INSTALL_DIR}/documentation

# install man page in the man tree located here:
#MAN_INSTALL_DIR = $${PREFIX}/share/man

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
