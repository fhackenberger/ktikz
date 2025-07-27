include(../KtikzConfig.cmake)

set(KTIKZPART_DATA_INSTALL_DIR ${KDE_INSTALL_DATADIR}/ktikzpart)
add_definitions(-DKTIKZ_KPART)
add_definitions(-DKTIKZ_TRANSLATIONS_INSTALL_DIR=\"${KTIKZ_TRANSLATIONS_INSTALL_DIR}\")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)

set(ktikzpart_SRCS
    configdialog.cpp
    configgeneralwidget.cpp
    part.cpp
    ../common/templatewidget.cpp
    ../common/tikzpreview.cpp
    ../common/tikzpreviewmessagewidget.cpp
    ../common/tikzpreviewrenderer.cpp
    ../common/tikzpreviewcontroller.cpp
    ../common/tikzpreviewgenerator.cpp
    ../common/utils/action.cpp
    ../common/utils/combobox.cpp
    ../common/utils/file.cpp
    ../common/utils/filedialog.cpp
    ../common/utils/globallocale.cpp
    ../common/utils/lineedit.cpp
    ../common/utils/messagebox.cpp
    ../common/utils/printpreviewdialog.cpp
    ../common/utils/recentfilesaction.cpp
    ../common/utils/selectaction.cpp
    ../common/utils/standardaction.cpp
    ../common/utils/tempdir.cpp
    ../common/utils/toggleaction.cpp
    ../common/utils/toolbar.cpp
    ../common/utils/url.cpp
    ../common/utils/zoomaction.cpp
)

ki18n_wrap_ui(
    ktikzpart_SRCS
    configgeneralwidget.ui
    ../common/templatewidget.ui
)

kconfig_add_kcfg_files(ktikzpart_SRCS ../common/settings.kcfgc)

add_library(ktikzpart MODULE ${ktikzpart_SRCS})
target_link_libraries(
    ktikzpart
    KF6::Parts
    KF6::IconThemes
    KF6::CoreAddons
    KF6::KIOCore
    KF6::KIOFileWidgets
    KF6::KIOWidgets
    KF6::I18n
    Qt6::PrintSupport
    Poppler::Qt6
)

install(TARGETS ktikzpart DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES ktikzpart.rc DESTINATION ${KTIKZPART_DATA_INSTALL_DIR})
#install(FILES ktikzpart.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

add_custom_target(uninstalldirs_part)
add_custom_command(
    TARGET uninstalldirs_part
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${KTIKZPART_DATA_INSTALL_DIR}
    COMMENT "Removing directory ${KTIKZPART_DATA_INSTALL_DIR}"
    VERBATIM
)
