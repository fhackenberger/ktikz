project(ktikz)
cmake_minimum_required(VERSION 2.6)

add_definitions(-DORGNAME=\"Florian\ Hackenberger\" -DAPPNAME=\"ktikz\")
add_definitions(-DAPPVERSION=\"0.10\")
add_definitions(-DKTIKZ_USE_KDE)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} /usr/include/poppler/qt4)

add_subdirectory(app)
add_subdirectory(part)

#add_subdirectory(doc)
#add_subdirectory(po)

install(FILES common/text-x-pgf.xml DESTINATION /usr/share/mime/packages)
install(FILES common/x-pgf.xml DESTINATION /usr/share/mime/text)
