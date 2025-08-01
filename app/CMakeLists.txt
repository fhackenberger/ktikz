include(../KtikzConfig.cmake)
add_definitions(-DKTIKZ_TRANSLATIONS_INSTALL_DIR=\"${KTIKZ_TRANSLATIONS_INSTALL_DIR}\")
add_definitions(-DKTIKZ_TEMPLATES_INSTALL_DIR=\"${KTIKZ_TEMPLATES_INSTALL_DIR}\")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/utils
)

set(ktikz_SRCS
    configappearancewidget.cpp
    configeditorwidget.cpp
    configgeneralwidget.cpp
    configpreviewwidget.cpp
    configdialog.cpp
    editgotolinewidget.cpp
    editindentwidget.cpp
    editreplacewidget.cpp
    editreplacecurrentwidget.cpp
    ktikzapplication.cpp
    linenumberwidget.cpp
    loghighlighter.cpp
    logtextedit.cpp
    main.cpp
    mainwindow.cpp
    tikzcommandinserter.cpp
    tikzcommandwidget.cpp
    tikzdocumentationcontroller.cpp
    tikzeditor.cpp
    tikzeditorhighlighter.cpp
    tikzeditorview.cpp
    usercommandeditdialog.cpp
    usercommandinserter.cpp
    ../common/templatewidget.cpp
    ../common/tikzpreview.cpp
    ../common/tikzpreviewmessagewidget.cpp
    ../common/tikzpreviewrenderer.cpp
    ../common/tikzpreviewcontroller.cpp
    ../common/tikzpreviewgenerator.cpp
    ../common/utils/action.cpp
    ../common/utils/colorbutton.cpp
    ../common/utils/combobox.cpp
    ../common/utils/file.cpp
    ../common/utils/filedialog.cpp
    ../common/utils/globallocale.cpp
    ../common/utils/lineedit.cpp
    ../common/utils/messagebox.cpp
    ../common/utils/pagedialog.cpp
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

if(KTIKZ_USE_KTEXTEDITOR)
    list(APPEND ktikz_SRCS tikzktexteditorview.cpp tikzktexteditorcompletion.cpp)
endif()

# dirty hack: use ki18n_wrap_ui to let Qt translate the .ui files instead of
# KDE (to have less duplication in the translation effort)
ki18n_wrap_ui(ktikz_UI_FILES
    configappearancewidget.ui
    configeditorwidget.ui
    configgeneralwidget.ui
    configpreviewwidget.ui
    editgotolinewidget.ui
    editindentwidget.ui
    editreplacewidget.ui
    usercommandeditdialog.ui
    ../common/templatewidget.ui
)
kconfig_add_kcfg_files(ktikz_SRCS ../common/settings.kcfgc)
qt_add_resources(ktikz_SRCS ktikz.qrc)
qt_add_resources(ktikz_SRCS qtikz.qrc)

file(GLOB icons_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-ktikz.png")
# add icons for win32 and mac; this command assumes that the first arg is of the
# form <appname>_SRCS
ecm_add_app_icon(ktikz_SRCS ICONS ${icons_SRCS})

add_executable(ktikz ${ktikz_SRCS} ${ktikz_UI_FILES})
target_link_libraries(
    ktikz
    KF6::XmlGui
    KF6::TextEditor
    KF6::IconThemes
    KF6::KIOWidgets
    KF6::KIOFileWidgets
    KF6::WidgetsAddons
    KF6::I18n
    Qt6::Core5Compat  # QTextCodec
    Qt6::PrintSupport
    Poppler::Qt6
)

if(KTIKZ_USE_KTEXTEDITOR)
    target_link_libraries(ktikz KF6::TextEditor)
endif()

install(TARGETS ktikz DESTINATION ${KDE_INSTALL_BINDIR})
if(KTIKZ_USE_KTEXTEDITOR)
    install(FILES kxmlgui5/ktikz/ktikzui_frameworks.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/ktikz)
endif()

install(FILES kxmlgui5/ktikz/ktikzui.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/ktikz)
install(FILES ../common/ktikz.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})

add_subdirectory(icons)

add_custom_target(uninstalldirs_app)
add_custom_command(
    TARGET uninstalldirs_app
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${KTIKZ_TRANSLATIONS_INSTALL_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${KTIKZ_TEMPLATES_INSTALL_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${KTIKZ_DATA_INSTALL_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${KDE_INSTALL_KXMLGUIDIR}/ktikz
    COMMENT
        "Removing directories ${KTIKZ_TRANSLATIONS_INSTALL_DIR} ${KTIKZ_TEMPLATES_INSTALL_DIR} ${KTIKZ_DATA_INSTALL_DIR} ${KDE_INSTALL_KXMLGUIDIR}/ktikz"
    VERBATIM
)
