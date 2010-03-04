add_definitions(-DORGNAME=\\\"Florian\\\ Hackenberger\\\" -DAPPNAME=\\\"ktikz\\\")
add_definitions(-DKTIKZ_USE_KDE)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../common
)

set(ktikzpart_SRCS
	configdialog.cpp
	configgeneralwidget.cpp
	part.cpp
	../common/templatewidget.cpp
	../common/tikzpreview.cpp
	../common/tikzpreviewthread.cpp
	../common/tikzpreviewcontroller.cpp
	../common/tikzpreviewgenerator.cpp
	../common/utils/action.cpp
	../common/utils/lineedit.cpp
	../common/utils/recentfilesaction.cpp
	../common/utils/selectaction.cpp
	../common/utils/standardaction.cpp
	../common/utils/toggleaction.cpp
	../common/utils/url.cpp
)

kde4_add_ui_files(ktikzpart_SRCS
	configgeneralwidget.ui
	../common/templatewidget.ui
)

add_definitions(-DKTIKZ_KPART)

kde4_add_kcfg_files(ktikzpart_SRCS ../common/settings.kcfgc)

kde4_add_plugin(ktikzpart ${ktikzpart_SRCS})

target_link_libraries(ktikzpart ${KDE4_KPARTS_LIBS} -lpoppler-qt4)

install(TARGETS ktikzpart DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ktikzpart.rc DESTINATION ${DATA_INSTALL_DIR}/ktikzpart)
install(FILES ktikzpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
