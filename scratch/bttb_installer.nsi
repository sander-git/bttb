!ifndef VERSION
  !define VERSION "4.2.1"
!endif
!ifndef SUFFIX
  !define SUFFIX ""
!endif
!ifndef COMPAT
  !define EXE_FILE "bttb_win32.exe"
  !define APP_NAME "Burn to the Brim"
!else
  !define EXE_FILE "bttb_win32_compat.exe"
  !define APP_NAME "Burn to the Brim (Compat)"
!endif

Unicode true

!include "MUI2.nsh"

Name "${APP_NAME} ${VERSION}"
OutFile "../build/bttb-cpp-${VERSION}-Win64${SUFFIX}-Installer.exe"
InstallDir "$LOCALAPPDATA\BurnToTheBrim"
InstallDirRegKey HKCU "Software\BurnToTheBrim" "InstallDir"

RequestExecutionLevel user

!define MUI_ABORTWARNING
!define MUI_ICON "../src/bttb.ico"
!define MUI_UNICON "../src/bttb.ico"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "../LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "Burn to the Brim (Required)" SecCore
  SectionIn RO
  SetOutPath "$INSTDIR"
  
  File "../build/${EXE_FILE}"
  File "../LICENSE"
  File "../README.md"
  
  SetOutPath "$INSTDIR\src"
  File "../src/bttb_embed.py"
  SetOutPath "$INSTDIR"
  
  WriteRegStr HKCU "Software\BurnToTheBrim" "InstallDir" "$INSTDIR"
  
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "DisplayName" "${APP_NAME}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "DisplayIcon" "$INSTDIR\${EXE_FILE},0"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "DisplayVersion" "${VERSION}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "Publisher" "BurnToTheBrim Team"
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "NoRepair" 1
  
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Start Menu Shortcuts" SecShortcuts
  CreateDirectory "$SMPROGRAMS\${APP_NAME}"
  CreateShortcut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${EXE_FILE}" "" "$INSTDIR\${EXE_FILE}" 0
  CreateShortcut "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Desktop Shortcut" SecDesktop
  CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${EXE_FILE}" "" "$INSTDIR\${EXE_FILE}" 0
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\${EXE_FILE}"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\src\bttb_embed.py"
  Delete "$INSTDIR\uninstall.exe"
  
  RMDir "$INSTDIR\src"
  RMDir "$INSTDIR"
  
  Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
  Delete "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk"
  RMDir "$SMPROGRAMS\${APP_NAME}"
  
  Delete "$DESKTOP\${APP_NAME}.lnk"
  
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB"
  DeleteRegKey HKCU "Software\BurnToTheBrim"
  
  ; Remove Explorer Context Menu integration if registered
  DeleteRegKey HKCU "Software\Classes\Directory\shell\BTTB"
SectionEnd
