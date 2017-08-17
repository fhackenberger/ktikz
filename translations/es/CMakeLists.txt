file(GLOB _po_files *.po)
gettext_process_po_files(es ALL INSTALL_DESTINATION ${KDE_INSTALL_LOCALEDIR} PO_FILES ${_po_files})
