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

find_package(Qt4 4.4.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
find_package(KDE4 4.3.0 REQUIRED)
find_package(PopplerQt4 0.6 REQUIRED) # Warning: this is part of the kdelibs development package
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${POPPLER_QT4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ${POPPLER_QT4_INCLUDE_DIR})

add_subdirectory(app)
add_subdirectory(part)

add_subdirectory(doc)
add_subdirectory(po)

# Install text/x-pgf mimetype

find_package(SharedMimeInfo)
install(FILES common/qtikz.xml DESTINATION ${XDG_MIME_INSTALL_DIR} RENAME ktikz.xml)
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})

# Remove directories

add_custom_target(uninstalldirs)
add_dependencies(uninstalldirs uninstalldirs_app uninstalldirs_part uninstalldirs_doc)

# Make packages

include(InstallRequiredSystemLibraries)
#set(CPACK_PACKAGE_NAME "KTikZ")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "KTikZ is a program for making pictures using TikZ")
set(CPACK_PACKAGE_VENDOR "Florian Hackenberger")
set(CPACK_PACKAGE_VERSION ${KTIKZ_VERSION})
#set(CPACK_PACKAGE_VERSION_MAJOR "0")
#set(CPACK_PACKAGE_VERSION_MINOR "10")
#set(CPACK_PACKAGE_VERSION_PATCH "0")
string(REGEX REPLACE "\\..*" "" CPACK_PACKAGE_VERSION_MAJOR ${KTIKZ_VERSION})
string(REGEX REPLACE "([0-9]*)\\.([0-9]*)(\\.([0-9]*))?" "\\2" CPACK_PACKAGE_VERSION_MINOR ${KTIKZ_VERSION})
string(REGEX REPLACE "([0-9]*)\\.([0-9]*)(\\.)?" "" CPACK_PACKAGE_VERSION_PATCH ${KTIKZ_VERSION})
if("${CPACK_PACKAGE_VERSION_PATCH}" STREQUAL "") # if ${KTIKZ_VERSION} is of the form 0.10 instead of 0.10.3
	set(CPACK_PACKAGE_VERSION_PATCH "0")
endif("${CPACK_PACKAGE_VERSION_PATCH}" STREQUAL "")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "KTikZ ${KTIKZ_VERSION}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.GPL2")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES "/.svn/;/build*/*;/templates/;Makefile;~$;.qch$;.qhc$;/debian/")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${KTIKZ_VERSION}")
set(CPACK_SET_DESTDIR TRUE)
if(WIN32 AND NOT UNIX)
	# There is a bug in NSI that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backslashes.
#	set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "KTikZ ${KTIKZ_VERSION}")
	set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\ktikz.exe")
	set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} KTikZ")
	set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.hackenberger.at/ktikz-editor-for-the-tikz-language")
	set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.hackenberger.at/ktikz-editor-for-the-tikz-language")
	set(CPACK_NSIS_CONTACT "florian@hackenberger.at")
	set(CPACK_NSIS_MODIFY_PATH ON)
else(WIN32 AND NOT UNIX)
	set(CPACK_STRIP_FILES "bin/ktikz")
	set(CPACK_SOURCE_STRIP_FILES "")
endif(WIN32 AND NOT UNIX)
set(CPACK_PACKAGE_EXECUTABLES "ktikz;KTikZ")
#execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE DEBIAN_PACKAGE_ARCHITECTURE)
#set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${DEBIAN_PACKAGE_ARCHITECTURE})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "kdelibs5 (>=4.3.0), libpoppler-qt4-3 (>=0.6), poppler-utils (>=0.6), pgf")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "khelpcenter4")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Florian Hackenberger <florian@hackenberger.at>")
set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
#set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
