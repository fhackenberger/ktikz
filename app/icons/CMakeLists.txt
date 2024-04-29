# this installs the icons with a file name of the form <size>-<group>-<name>.<extension>, e.g. hi22-apps-ktikz.png
ecm_install_icons(ICONS
    16-apps-ktikz.png
    22-apps-ktikz.png
    32-apps-ktikz.png
    48-apps-ktikz.png
    64-apps-ktikz.png
    128-apps-ktikz.png
    sc-apps-ktikz.svgz
    sc-apps-ktikz.svg
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)
