project(ktikz)
cmake_minimum_required(VERSION 2.6)

set(KTIKZ_VERSION "0.10")

add_definitions(-DORGNAME=\"Florian_Hackenberger\" -DAPPNAME=\"ktikz\")
add_definitions(-DAPPVERSION=\"${KTIKZ_VERSION}\")
add_definitions(-DKTIKZ_USE_KDE)
add_definitions(-DKTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\"/usr/share/doc/texmf/pgf/pgfmanual.pdf.gz\")
if(WIN32 AND NOT UNIX)
	add_definitions(-DKTIKZ_TEMPLATE_EDITOR_DEFAULT=\"notepad\")
else(WIN32 AND NOT UNIX)
	add_definitions(-DKTIKZ_TEMPLATE_EDITOR_DEFAULT=\"kwrite\")
endif(WIN32 AND NOT UNIX)

find_package(Qt4 4.6.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
find_package(KDE4 4.3.0 REQUIRED)
find_package(PopplerQt4 0.6 REQUIRED) # Warning: this is part of the kdelibs development package
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${POPPLER_QT4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ${POPPLER_QT4_INCLUDE_DIR})

add_subdirectory(app)
add_subdirectory(part)

add_subdirectory(doc)
add_subdirectory(translations)
add_subdirectory(data)

# Remove directories

add_custom_target(uninstalldirs)
add_dependencies(uninstalldirs uninstalldirs_app uninstalldirs_part uninstalldirs_doc)

# Make packages

include(KtikzCPackOptions.cmake)
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
