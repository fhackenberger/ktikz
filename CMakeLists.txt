cmake_minimum_required(VERSION 3.16.0)
project(ktikz)

set(KTIKZ_VERSION "0.13.2")

set(KTIKZ_USE_KTEXTEDITOR TRUE CACHE BOOL "Use KTextEditor framework" )

add_definitions(-DORGNAME=\"Florian_Hackenberger\" -DAPPNAME=\"ktikz\")
add_definitions(-DAPPVERSION=\"${KTIKZ_VERSION}\")
add_definitions(-DKTIKZ_USE_DESKTOP_ICONS)
add_definitions(-DKTIKZ_USE_KDE)
if( KTIKZ_USE_KTEXTEDITOR )
  add_definitions(-DKTIKZ_USE_KTEXTEDITOR)
endif()
add_definitions(-DKTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\"/usr/share/doc/texmf/pgf/pgfmanual.pdf.gz\")

find_package(ECM 1.1.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
include(ECMSetupVersion)
include(ECMAddAppIcon)
include(ECMInstallIcons)

find_package(Qt5 5.2 CONFIG REQUIRED Core Gui Widgets Xml PrintSupport LinguistTools )

find_package(KF5 5.0 REQUIRED DocTools XmlGui TextEditor Parts IconThemes )

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
# include(KDEFrameworkCompilerSettings)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DQT_STRICT_ITERATORS)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wlogical-op" )
   endif()
endif()


find_package(Poppler "0.12.1" REQUIRED Qt5 Core)
set_package_properties("Poppler-Qt5" PROPERTIES
        DESCRIPTION "A PDF rendering library"
        URL "http://poppler.freedesktop.org"
        TYPE REQUIRED
        PURPOSE "Support for PDF files in KTikZ.")
include_directories( ${Poppler_INCLUDE_DIRS} )

add_subdirectory(app)
add_subdirectory(part)

add_subdirectory(doc)
add_subdirectory(translations)
add_subdirectory(data)

# Remove directories
add_custom_target(uninstalldirs)
add_dependencies(uninstalldirs uninstalldirs_app uninstalldirs_part uninstalldirs_doc)
# add_dependencies(uninstalldirs uninstalldirs_app uninstalldirs_doc)

# Make packages
include(KtikzCPackOptions.cmake)
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
