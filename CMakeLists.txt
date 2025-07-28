cmake_minimum_required(VERSION 3.22.0)
project(ktikz)

if(POLICY CMP0160)
    # NOTE: silence warnings in ECM (policy since cmake 3.29)
    cmake_policy(SET CMP0160 OLD)
endif()

set(KTIKZ_VERSION "0.13.2")
set(KTIKZ_USE_KTEXTEDITOR FALSE CACHE BOOL "Use KTextEditor framework")

set(QT_MIN_VERSION "6.5.0")
set(KF6_DEP_VERSION "6.0.0")
set(QT_MAJOR_VERSION "6")

add_definitions(-DORGNAME=\"Florian_Hackenberger\")
add_definitions(-DAPPNAME=\"ktikz\")
add_definitions(-DAPPVERSION=\"${KTIKZ_VERSION}\")
add_definitions(-DKTIKZ_USE_DESKTOP_ICONS)
add_definitions(-DKTIKZ_USE_KDE)
add_definitions(-DKTIKZ_TIKZ_DOCUMENTATION_DEFAULT=\"/usr/share/doc/texmf/pgf/pgfmanual.pdf.gz\")

if(KTIKZ_USE_KTEXTEDITOR)
    add_definitions(-DKTIKZ_USE_KTEXTEDITOR)
endif()

find_package(ECM 1.1.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

include(ECMSetupVersion)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED
    Core
    Gui
    Widgets
    Xml
    PrintSupport
    LinguistTools
    Core5Compat
)

find_package(KF6 ${KF6_DEP_VERSION} REQUIRED
    DocTools
    XmlGui
    TextEditor
    Parts
    IconThemes
    KIO
    WidgetsAddons
    I18n
)

option(BUILD_WITH_SANITIZERS "Build with sanitizers" OFF)
if(BUILD_WITH_SANITIZERS)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -fsanitize=leak")
  else()
    # TODO @JC check if MSVC supports sanitizers
  endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DQT_STRICT_ITERATORS)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wlogical-op")
    endif()
endif()

find_package(Poppler "24.06" REQUIRED Qt6 Core)
set_package_properties(
    "Poppler-Qt6"
    PROPERTIES
    DESCRIPTION "A PDF rendering library"
    URL "http://poppler.freedesktop.org"
    TYPE REQUIRED
    PURPOSE "Support for PDF files in KTikZ."
)

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
