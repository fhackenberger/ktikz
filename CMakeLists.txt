project(ktikz)
cmake_minimum_required(VERSION 2.8.12)

set(KTIKZ_VERSION "0.11")

add_definitions(-DORGNAME=\"Florian_Hackenberger\" -DAPPNAME=\"ktikz\")
add_definitions(-DAPPVERSION=\"${KTIKZ_VERSION}\")
add_definitions(-DKTIKZ_USE_KDE)
add_definitions(-DKTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\"/usr/share/doc/texmf/pgf/pgfmanual.pdf.gz\")
if(WIN32 AND NOT UNIX)
	add_definitions(-DKTIKZ_TEMPLATE_EDITOR_DEFAULT=\"notepad\")
else(WIN32 AND NOT UNIX)
	add_definitions(-DKTIKZ_TEMPLATE_EDITOR_DEFAULT=\"kwrite\")
endif(WIN32 AND NOT UNIX)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include(ECMSetupVersion)
include(ECMInstallIcons)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml PrintSupport LinguistTools )
add_definitions (${QT_DEFINITIONS})
add_definitions (-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(${Qt5Core_DEFINITIONS} ${Qt5Gui_DEFINITIONS} ${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINITIONS})

find_package(KF5 REQUIRED TextEditor KDELibs4Support WidgetsAddons KIO XmlGui CoreAddons IconThemes I18n JobWidgets )
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)
include(KDEFrameworkCompilerSettings)

find_package(Poppler "0.12.1")
set_package_properties("Poppler-Qt5" PROPERTIES
        DESCRIPTION "A PDF rendering library"
        URL "http://poppler.freedesktop.org"
        TYPE RECOMMENDED
        PURPOSE "Support for PDF files in KTikZ.")


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
