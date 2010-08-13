;NSIS Modern User Interface
;QTikZ installation script
;Written by Glad Deschrijver

!define ORGNAME "Florian_Hackenberger"
!define APPNAME "QTikZ"
!define FILENAME "qtikz"
; qtikz.nsh file is created by "make install" and it contains VERSION
!include "qtikz.nsh"

; Compression type
SetCompressor lzma

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "${APPNAME} ${VERSION}"
  OutFile "${FILENAME}-${VERSION}-install.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\${APPNAME}"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\${ORGNAME}\${APPNAME}" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "data\LICENSE.GPL2"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${ORGNAME}\${APPNAME}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

; TODO: do we really need the following?
; Support of storing installed filenames
; for easy uninstallation

!define UninstLog "${FILENAME}_uninstall.log"
Var UninstLog

; File macro
!macro File FilePath FileName
	IfFileExists "$OUTDIR\${FileName}" +2
		FileWrite $UninstLog "$OUTDIR\${FileName}$\r$\n"
	File "${FilePath}${FileName}"
!macroend
!define File "!insertmacro File"

; SetOutPath macro
!macro SetOutPath Path
	SetOutPath "${Path}"
	FileWrite $UninstLog "${Path}$\r$\n"
!macroend
!define SetOutPath "!insertmacro SetOutPath"

Section "Main Section" SecMain

  SectionIn RO

  ; Set output path to the installation directory
  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  ${File} "" "${FILENAME}.exe"
  ;${File} "" "${FILENAME}_readme.txt"
  ${SetOutPath} $INSTDIR\data\documentation
  ${File} "data\documentation\" "qtikz.qch"
  ${File} "data\documentation\" "qtikz.qhc"
  ${SetOutPath} $INSTDIR\data\locale
  ${File} "data\locale\" "qtikz_de.qm"
  ${File} "data\locale\" "qtikz_es.qm"
  ${File} "data\locale\" "qtikz_fr.qm"
  ${SetOutPath} $INSTDIR\data\templates
  ${File} "data\templates\" "beamer-example-template.pgs"
  ${File} "data\templates\" "template_example.pgs"
  ${File} "data\templates\" "template_example2.pgs"
  
  ;Store installation folder
  WriteRegStr HKCU "Software\${ORGNAME}\${APPNAME}" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "Poppler Section" SecPoppler

	; Set output path to the installation directory
	SetOutPath "$INSTDIR"

	; Put files here
	${File} "" "iconv.dll"
	${File} "" "jpeg62.dll"
	${File} "" "libfontconfig.dll"
	${File} "" "libfreetype.dll"
	${File} "" "liblcms-1.dll"
	${File} "" "libopenjpeg.dll"
	${File} "" "libpng12.dll"
	${File} "" "libpoppler.dll"
	${File} "" "libpoppler-qt4.dll"
	${File} "" "libxml2.dll"
	${File} "" "zlib1.dll"

SectionEnd

Section "Qt4 Section" SecQt4

	; Set output path to the installation directory
	SetOutPath "$INSTDIR"

	; Put files here
	${File} "" "assistant.exe"
	${File} "" "libgcc_s_dw2-1.dll"
	${File} "" "mingwm10.dll"
	${File} "" "QtCore4.dll"
	${File} "" "QtGui4.dll"
	${File} "" "QtXml4.dll"

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecMain ${LANG_ENGLISH} "Main application and data files."
  LangString DESC_SecPoppler ${LANG_ENGLISH} "Poppler runtime files."
  LangString DESC_SecQt4 ${LANG_ENGLISH} "Qt4 runtime files."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPoppler} $(DESC_SecPoppler)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecQt4} $(DESC_SecQt4)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;TODO
  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty HKCU "Software\${ORGNAME}\${APPNAME}"

SectionEnd
