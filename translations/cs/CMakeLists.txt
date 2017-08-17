file(GLOB _po_files *.po)
gettext_process_po_files(cs ALL INSTALL_DESTINATION ${KDE_INSTALL_LOCALEDIR} PO_FILES ${_po_files})
