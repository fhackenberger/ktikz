set(KTIKZPART_DATA_INSTALL_DIR ${DATA_INSTALL_DIR}/ktikzpart)
#add_definitions(-DORGNAME=\\\"Florian_Hackenberger\\\" -DAPPNAME=\\\"ktikz\\\")
#add_definitions(-DKTIKZ_USE_KDE)

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
	../common/utils/filedialog.cpp
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

kde4_add_kcfg_files(ktikzpart_SRCS ../common/settings.kcfgc)

kde4_add_plugin(ktikzpart ${ktikzpart_SRCS})

target_link_libraries(ktikzpart ${KDE4_KPARTS_LIBS} ${POPPLER_QT4_LIBRARIES})

install(TARGETS ktikzpart DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ktikzpart.rc DESTINATION ${KTIKZPART_DATA_INSTALL_DIR})
install(FILES ktikzpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})

add_custom_target(uninstalldirs_part)
add_custom_command(TARGET uninstalldirs_part
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${KTIKZPART_DATA_INSTALL_DIR}
	COMMENT "Removing directory ${KTIKZPART_DATA_INSTALL_DIR}"
	VERBATIM)
