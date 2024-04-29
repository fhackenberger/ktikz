### Install KDE Help Center documentation
kdoctools_create_handbook(
    index.docbook
    INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en
    SUBDIR ktikz
)

### Install man files
install(FILES ktikz.1 DESTINATION ${KDE_INSTALL_MANDIR}/man1)

### Remove installation directories
add_custom_target(uninstalldirs_doc)
add_custom_command(
    TARGET uninstalldirs_doc
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove ${KDE_INSTALL_DOCBUNDLEDIR}/en/ktikz/common
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${KDE_INSTALL_DOCBUNDLEDIR}/en/ktikz
    COMMENT "Removing directory ${KDE_INSTALL_DOCBUNDLEDIR}/en/ktikz"
    VERBATIM
)
