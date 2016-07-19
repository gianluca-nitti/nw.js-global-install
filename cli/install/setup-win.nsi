;Forked from NSIS Modern User Interface, Basic Example Script by Joost Verburg

;--------------------------------
;Includes

!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "LogicLib.nsh"

;--------------------------------
;General

;Name and file
Name "{{guiName}}"
OutFile "..\{{appName}}-win-setup.exe"
;Default installation folder
InstallDir "$PROGRAMFILES\{{appName}}"
;Get installation folder from registry if available
InstallDirRegKey HKLM "Software\{{appName}}" ""
;Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\{{license}}"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "{{guiName}}" SecMain
	SectionIn RO
	; Install nwjsmanager if it's not already installed
	SetOverWrite off
	SetShellVarContext all
	SetOutPath "$APPDATA\nwjs"
	File "{{nwjsmanager}}"
	SetOverWrite on
	; Install application's files
	SetOutPath "$INSTDIR"
	File "{{appName}}.exe"
	{% for dir in appDirs %}
		SetOutPath "$INSTDIR\{{dir.name}}"
		{% for file in dir.files %}
			File "..\..\{{file}}"
		{% endfor %}
	{% endfor %}
	;Store installation folder
	WriteRegStr HKLM "Software\{{appName}}" "" $INSTDIR
	; Show in add/romove programs
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\{{appName}}" "DisplayName" "{{guiName}}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\{{appName}}" "DisplayVersion" "{{appVersion}}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\{{appName}}" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\{{appName}}" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
	${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2 ; Compute EstimatedSize
	IntFmt $0 "0x%08X" $0
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\{{appName}}" "EstimatedSize" "$0"
	;Add to the nwjsmanager list of installed applications
	ExecWait "$APPDATA\nwjs\nwjsmanager.exe --install {{appName}}"
	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	;Use nwjsmanager to download the right nw.js version
	ExecWait '$APPDATA\nwjs\nwjsmanager.exe "$INSTDIR" --nwjsmanager-prepare'
SectionEnd

Section "Start menu shortcut" SecStartShortcut
	SetOutPath "$INSTDIR"
	CreateDirectory "$SMPROGRAMS\{{guiName}}"
	CreateShortCut "$SMPROGRAMS\{{guiName}}\{{guiName}}.lnk" "$INSTDIR\{{appName}}.exe"
	CreateShortCut "$SMPROGRAMS\{{guiName}}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
SectionEnd

Section "Desktop shortcut" SecDesktopShortcut
	SetOutPath "$INSTDIR"
	CreateShortCut "$DESKTOP\{{guiName}}.lnk" "$INSTDIR\{{appName}}.exe"
SectionEnd

;--------------------------------
;Descriptions

;Language strings
LangString DESC_SecMain ${LANG_ENGLISH} "{{guiName}} core files."
LangString DESC_SecStartShortcut ${LANG_ENGLISH} "Folder in the start menu with links to launch {{guiName}} and to uninstall it."
LangString DESC_SecDesktopShortcut ${LANG_ENGLISH} "Shortcut to launch {{guiName}} from your desktop."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
!insertmacro MUI_DESCRIPTION_TEXT ${SecStartShortcut} $(DESC_SecStartShortcut)
!insertmacro MUI_DESCRIPTION_TEXT ${SecDesktopShortcut} $(DESC_SecDesktopShortcut)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
	SetShellVarContext all
	Delete "$INSTDIR\{{appName}}.exe"
	{% for dir in appDirs %}
		{% for file dir.files %}
			Delete "$INSTDIR\{{file}}"
		{% endfor %}
		RMDir "$INSTDIR\{{dir.name}}"
	{% endfor %}
	Delete "$INSTDIR\Uninstall.exe"
	RMDir "$INSTDIR"
	DeleteRegKey /ifempty HKLM "Software\{{appName}}"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\{{appName}}"
	Delete "$SMPROGRAMS\{{guiName}}\{{guiName}}.lnk"
	Delete "$SMPROGRAMS\{{guiName}}\Uninstall.lnk"
	RMDir "$SMPROGRAMS\{{guiName}}"
	Delete "$DESKTOP\{{guiName}}.lnk"
	; TODO: allow user to choose if delete the data directory
	ExecWait "$APPDATA\nwjs\nwjsmanager.exe --uninstall {{appName}}" $0
	${If} $0 == "1"
		MessageBox MB_YESNO "The uninstalled application was based on the nw.js framework. There aren't other nw.js-based applications installed on this system, so these files are no longer necessary. Do you want to remove them? This will delete the whole $APPDATA\nwjs directory. Choose No if you want to reinstall {{guiName}}." IDYES yes IDNO no
		yes:
			RMDir /r "$APPDATA\nwjs"
		no:
	${EndIf}
SectionEnd
