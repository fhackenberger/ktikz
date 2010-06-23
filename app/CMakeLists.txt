set(KTIKZ_DATA_INSTALL_DIR ${DATA_INSTALL_DIR}/ktikz)
set(KTIKZ_TRANSLATIONS_INSTALL_DIR ${KTIKZ_DATA_INSTALL_DIR}/locale)
add_definitions(-DKTIKZ_TRANSLATIONS_INSTALL_DIR=\\\"${KTIKZ_TRANSLATIONS_INSTALL_DIR}\\\")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../common
	${CMAKE_CURRENT_SOURCE_DIR}/../common/utils
)

set(ktikz_SRCS
	configappearancewidget.cpp
	configeditorwidget.cpp
	configgeneralwidget.cpp
	configdialog.cpp
	editgotolinewidget.cpp
	editindentdialog.cpp
	editreplacewidget.cpp
	editreplacecurrentwidget.cpp
	ktikzapplication.cpp
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
	../common/templatewidget.cpp
	../common/tikzpreview.cpp
	../common/tikzpreviewthread.cpp
	../common/tikzpreviewcontroller.cpp
	../common/tikzpreviewgenerator.cpp
	../common/utils/action.cpp
	../common/utils/colorbutton.cpp
	../common/utils/filedialog.cpp
	../common/utils/lineedit.cpp
	../common/utils/recentfilesaction.cpp
	../common/utils/selectaction.cpp
	../common/utils/standardaction.cpp
	../common/utils/toggleaction.cpp
	../common/utils/url.cpp
)
# dirty hack: use qt4_wrap_ui to let Qt translate the .ui files instead of KDE (to have less duplication in the translation effort)
#kde4_add_ui_files(ktikz_SRCS
qt4_wrap_ui(ktikz_UI_FILES
	configappearancewidget.ui
	configeditorwidget.ui
	configgeneralwidget.ui
	editgotolinewidget.ui
	editindentdialog.ui
	editreplacewidget.ui
	../common/templatewidget.ui
)
kde4_add_kcfg_files(ktikz_SRCS ../common/settings.kcfgc)
qt4_add_resources(ktikz_SRCS ktikz.qrc)

file(GLOB ktikz_TRANSLATION_FILES ktikz_*.ts)
qt4_add_translation(ktikz_QM_FILES ${ktikz_TRANSLATION_FILES})

# dirty hack: use qt4_wrap_ui to let Qt translate the .ui files instead of KDE
#kde4_add_executable(ktikz ${ktikz_SRCS} ${ktikz_QM_FILES})
kde4_add_executable(ktikz ${ktikz_SRCS} ${ktikz_UI_FILES} ${ktikz_QM_FILES})
target_link_libraries(ktikz ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KTEXTEDITOR_LIBS} ${POPPLER_QT4_LIBRARIES})

install(TARGETS ktikz DESTINATION ${BIN_INSTALL_DIR})
install(FILES ktikzui.rc DESTINATION ${DATA_INSTALL_DIR}/ktikz)
install(FILES ../common/ktikz.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES ktikz.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ${ktikz_QM_FILES} DESTINATION ${KTIKZ_TRANSLATIONS_INSTALL_DIR})
install(FILES ../examples/template_example.pgs DESTINATION ${KTIKZ_DATA_INSTALL_DIR})

if(NOT APPLE AND NOT WIN32)
	install(FILES images/ktikz-128.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/128x128/apps/ RENAME ktikz.png)
	install(FILES images/ktikz-64.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/64x64/apps/ RENAME ktikz.png)
	install(FILES images/ktikz-48.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/48x48/apps/ RENAME ktikz.png)
	install(FILES images/ktikz-32.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/32x32/apps/ RENAME ktikz.png)
	install(FILES images/ktikz-22.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/22x22/apps/ RENAME ktikz.png)
	install(FILES images/ktikz-16.png DESTINATION ${ICON_INSTALL_DIR}/hicolor/16x16/apps/ RENAME ktikz.png)
	install(FILES images/ktikz.svg DESTINATION ${ICON_INSTALL_DIR}/hicolor/scalable/apps/)
endif(NOT APPLE AND NOT WIN32)
