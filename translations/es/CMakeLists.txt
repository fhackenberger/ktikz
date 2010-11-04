file(GLOB _po_files *.po)
gettext_process_po_files(es ALL INSTALL_DESTINATION ${LOCALE_INSTALL_DIR} ${_po_files})
