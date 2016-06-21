;Forked from NSIS Modern User Interface, Basic Example Script by Joost Verburg

;--------------------------------
;Include Modern UI

!include "MUI2.nsh"

;--------------------------------
;General

;Name and file
Name "<%=APPNAMEGUI%>"
OutFile "../<%=APPNAME%>-win-setup.exe"
;Default installation folder
InstallDir "$PROGRAMFILES\<%=APPNAME%>"
;Get installation folder from registry if available
InstallDirRegKey HKLM "Software\<%=APPNAME%>" ""
;Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "../../<%=LICENSE%>" #temp
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

Section "<%=APPNAMEGUI%>" SecMain
	SectionIn RO
	SetOutPath "$INSTDIR"
	;ADD YOUR OWN FILES HERE...
	File "<%=APPLAUNCHERPATH%>"
	;Store installation folder
	WriteRegStr HKLM "Software\<%=APPNAME%>" "" $INSTDIR
	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

;--------------------------------
;Descriptions

;Language strings
LangString DESC_SecMain ${LANG_ENGLISH} "<%=APPNAMEGUI%> core files."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
	;ADD YOUR OWN FILES HERE...
	Delete "$INSTDIR\applauncher.exe"
	Delete "$INSTDIR\Uninstall.exe"
	RMDir "$INSTDIR"
	DeleteRegKey /ifempty HKLM "Software\<%=APPNAME%>"
SectionEnd
