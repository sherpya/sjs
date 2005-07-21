; Sherpya JavaScript Shell NSIS Installer
; Copyright (C) 2005 Gianluigi Tiesi <sherpya@netfarm.it>
; Copyright (C) 2005 NetFarm S.r.l.  [http://www.netfarm.it]
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by the
; Free Software Foundation; either version 2, or (at your option) any later
; version.
;
; This program is distributed in the hope that it will be useful, but
; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTIBILITY
; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
; for more details.

SetCompressor /solid lzma

; Modern interface settings
!include "MUI.nsh"

!define MUI_ICON "sjs.ico"
!define MUI_UNICON "sjs.ico"

; Used to refresh the display of file association
!define SHCNE_ASSOCCHANGED 0x08000000
!define SHCNF_IDLIST 0

; Library Function
!include "Library.nsh"

!define APPNAME "Sherpya JavaScript Shell"
!define APPNAMEANDVERSION "Sherpya JavaScript Shell 1.0"

Name "${APPNAME}"
OutFile "../dist/sjs-setup.exe"

InstallDir "$PROGRAMFILES\SJS"
InstallDirRegKey HKLM "Software\Netfarm\${APPNAME}" "InstallDir"

!packhdr tmp.dat "upx --best tmp.dat"
SetDateSave on
SetDatablockOptimize on
CRCCheck on
SilentInstall normal
InstallColors FF8080 000030
WindowIcon on
XPStyle on

; ============================================================================
; Install page configuration
; ============================================================================
ShowInstDetails show

; ============================================================================
; Functions and macros - from ethereal installation script
; ============================================================================
!macro UpdateIcons
    Push $R0
    Push $R1
    Push $R2

    !define UPDATEICONS_UNIQUE ${__LINE__}

    IfFileExists "$SYSDIR\shell32.dll" UpdateIcons.next1_${UPDATEICONS_UNIQUE} UpdateIcons.error1_${UPDATEICONS_UNIQUE}
    UpdateIcons.next1_${UPDATEICONS_UNIQUE}:
    GetDllVersion "$SYSDIR\shell32.dll" $R0 $R1
    IntOp $R2 $R0 / 0x00010000
    IntCmp $R2 4 UpdateIcons.next2_${UPDATEICONS_UNIQUE} UpdateIcons.error2_${UPDATEICONS_UNIQUE}
UpdateIcons.next2_${UPDATEICONS_UNIQUE}:
    System::Call 'shell32.dll::SHChangeNotify(i, i, i, i) v (${SHCNE_ASSOCCHANGED}, ${SHCNF_IDLIST}, 0, 0)'
    Goto UpdateIcons.quit_${UPDATEICONS_UNIQUE}

UpdateIcons.error1_${UPDATEICONS_UNIQUE}:
    DetailPrint "Can't find 'shell32.dll' library. Impossible to update icons"
    Goto UpdateIcons.quit_${UPDATEICONS_UNIQUE}
UpdateIcons.error2_${UPDATEICONS_UNIQUE}:
    DetailPrint "You should install the free 'Microsoft Layer for Unicode' to update JS icons"
    Goto UpdateIcons.quit_${UPDATEICONS_UNIQUE}
UpdateIcons.quit_${UPDATEICONS_UNIQUE}:
    !undef UPDATEICONS_UNIQUE
    Pop $R2
    Pop $R1
    Pop $R0
!macroend

; Vars
Var STARTMENU_FOLDER

; Interface
!define MUI_ABORTWARNING

; Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Netfarm\${APPNAME}" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_LICENSE "copyright.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES
;!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

; Setup Versioning
VIProductVersion "1.0.0.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${APPNAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "${APPNAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Netfarm"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2005 Gianluigi Tiesi"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${APPNAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "1.0"

SetOverwrite on

Section "JavaScript Shell" Main
    ; Main executable in sys dir
    SetOutPath "$SYSDIR"
    File "Release\sjs.exe"

    !insertmacro InstallLib DLL SHARED NOREBOOT_PROTECTED "..\js\src\Release\js32.dll" $SYSDIR\js32.dll $SYSDIR

    ; Help File
    SetOutPath "$INSTDIR\"
    File "..\dist\sjs.chm"

    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\SJS Reference.lnk" "$INSTDIR\sjs.chm"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\uninstall.exe"
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

SectionGroup "Plugins" SecPluginsPlugins

Section "Curl Plugin" SecPluginsCurl

    SetOutPath "$INSTDIR\plugins\"
    File "plugins\Release\curl.dll"
    File "..\curl\libcurl.dll"
    File "..\ssl\ssleay32.dll"
    File "..\ssl\libeay32.dll"
    File "..\zlib\zlib1.dll"
SectionEnd

Section "Korax Plugin" SecPluginsKorax
    SetOutPath "$INSTDIR\plugins\"
    File "plugins\Release\korax.dll"
SectionEnd

Section "Win32 Registry Plugin" SecPluginsRegistry
    SetOutPath "$INSTDIR\plugins\"
    File "plugins\Release\registry.dll"
SectionEnd

Section "Zlib Plugin" SecPluginsZlib
    SetOutPath "$INSTDIR\plugins\"
    File "plugins\Release\zlib.dll"
    File "..\zlib\zlib1.dll"
SectionEnd

SectionGroupEnd

Section "Example Scripts" Examples
    SetOutPath "$INSTDIR\scripts\"
    File "scripts\*.js"
SectionEnd
    
Section -FinishSection
    ; Backup Default Icon
    ReadRegStr $0 HKCR "JSFile\DefaultIcon" ""
    WriteRegExpandStr HKCR "JSFile\DefaultIcon.sjs" "" "$0"

    WriteRegExpandStr HKCR "JSFile\DefaultIcon" "" "$SYSDIR\sjs.exe,0"

    ; Open With SJS
    WriteRegStr HKCR "JSFile\Shell\SJS" "" "Open with SJS"
    WriteRegExpandStr HKCR "JSFile\Shell\SJS\Command" "" '"$SYSDIR\sjs.exe" -w "%1"'

    !insertmacro UpdateIcons

    WriteRegExpandStr HKLM "Software\Netfarm\${APPNAME}" "InstallDir" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${Main} "Sherpya JavaScript Shell"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsPlugins} "Bundled Plugins"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsCurl} "Plugin for libcurl, handles internet downloads"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsKorax} "Plugin for Korax's utils, includes a parses class and other functions"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsRegistry} "Plugin to handle win32 registry"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPluginsZlib} "Plugin to zip compressed files"
    !insertmacro MUI_DESCRIPTION_TEXT ${Examples} "Example Scripts"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall
    ; Restore Default Icon
    ReadRegStr $0 HKCR "JSFile\DefaultIcon.sjs" ""
    WriteRegExpandStr HKCR "JSFile\DefaultIcon" "" "$0"

    ; Remove from registry...
    DeleteRegKey HKCR "JSFile\DefaultIcon.sjs"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
    DeleteRegKey HKLM "Software\Netfarm\${APPNAME}"
    DeleteRegKey /ifempty HKLM "Software\Netfarm"

    !insertmacro UpdateIcons

    ; Delete self
    Delete "$INSTDIR\uninstall.exe"

    ; Delete Shortcuts
    !insertmacro MUI_STARTMENU_GETFOLDER Application $STARTMENU_FOLDER

    Delete "$SMPROGRAMS\$STARTMENU_FOLDER\SJS Reference.lnk"
    Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk"

    ; Delete empty start menu parent diretories
    StrCpy $STARTMENU_FOLDER "$SMPROGRAMS\$STARTMENU_FOLDER"

    startMenuDeleteLoop:
        RMDir $STARTMENU_FOLDER
        GetFullPathName $STARTMENU_FOLDER "$STARTMENU_FOLDER\.."

        IfErrors startMenuDeleteLoopDone

        StrCmp $STARTMENU_FOLDER $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
    startMenuDeleteLoopDone:

    ; Clean up Sherpya JavaScript Shell
    Delete "$SYSDIR\sjs.exe"
    !insertmacro UnInstallLib DLL SHARED NOREBOOT_PROTECTED $SYSDIR\js32.dll

    Delete "$INSTDIR\sjs.chm"

    ; Clean up Curl
    Delete "$INSTDIR\plugins\curl.dll"
    Delete "$INSTDIR\plugins\libcurl"
    Delete "$INSTDIR\plugins\ssleay32.dll"
    Delete "$INSTDIR\plugins\libeay32.dll"

    Delete "$INSTDIR\plugins\korax.dll"
    Delete "$INSTDIR\plugins\registry.dll"

    ; Clean up Zlib
    Delete "$INSTDIR\plugins\zlib.dll"
    Delete "$INSTDIR\plugins\zlib1.dll"

    ; Examples
    RMDir /r "$INSTDIR/scripts"

    ; Remove remaining directories
    RMDir /r "$INSTDIR"

SectionEnd
