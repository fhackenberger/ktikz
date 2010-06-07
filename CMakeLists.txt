project(ktikz)
cmake_minimum_required(VERSION 2.6)

set(KTIKZ_VERSION "0.10")

add_definitions(-DORGNAME=\"Florian\ Hackenberger\" -DAPPNAME=\"ktikz\")
add_definitions(-DAPPVERSION=\"${KTIKZ_VERSION}\")
add_definitions(-DKTIKZ_USE_KDE)
add_definitions(-DKTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\"/usr/share/doc/texmf/pgf/pgfmanual.pdf.gz\")

find_package(Qt4 4.4.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
find_package(KDE4 4.3.0 REQUIRED)
find_package(PopplerQt4 0.6 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${POPPLER_QT4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ${POPPLER_QT4_INCLUDE_DIR})

add_subdirectory(app)
add_subdirectory(part)

add_subdirectory(doc)
add_subdirectory(po)

find_package(SharedMimeInfo)
install(FILES common/text-x-pgf.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
