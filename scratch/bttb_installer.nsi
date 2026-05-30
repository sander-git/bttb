Unicode true

!include "MUI2.nsh"

Name "Burn to the Brim 4.1.0"
OutFile "../build/bttb-cpp-4.1.0-Win64-Installer.exe"
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
  
  File "../build/bttb_win32.exe"
  File "../LICENSE"
  File "../README.md"
  
  SetOutPath "$INSTDIR\src"
  File "../src/bttb_embed.py"
  SetOutPath "$INSTDIR"
  
  WriteRegStr HKCU "Software\BurnToTheBrim" "InstallDir" "$INSTDIR"
  
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "DisplayName" "Burn to the Brim"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "DisplayIcon" "$INSTDIR\bttb_win32.exe,0"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "DisplayVersion" "4.1.0"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "Publisher" "BurnToTheBrim Team"
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB" "NoRepair" 1
  
  WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Start Menu Shortcuts" SecShortcuts
  CreateDirectory "$SMPROGRAMS\Burn to the Brim"
  CreateShortcut "$SMPROGRAMS\Burn to the Brim\Burn to the Brim.lnk" "$INSTDIR\bttb_win32.exe" "" "$INSTDIR\bttb_win32.exe" 0
  CreateShortcut "$SMPROGRAMS\Burn to the Brim\Uninstall Burn to the Brim.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Desktop Shortcut" SecDesktop
  CreateShortcut "$DESKTOP\Burn to the Brim.lnk" "$INSTDIR\bttb_win32.exe" "" "$INSTDIR\bttb_win32.exe" 0
SectionEnd

Section "Uninstall"
  Delete "$INSTDIR\bttb_win32.exe"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\src\bttb_embed.py"
  Delete "$INSTDIR\uninstall.exe"
  
  RMDir "$INSTDIR\src"
  RMDir "$INSTDIR"
  
  Delete "$SMPROGRAMS\Burn to the Brim\Burn to the Brim.lnk"
  Delete "$SMPROGRAMS\Burn to the Brim\Uninstall Burn to the Brim.lnk"
  RMDir "$SMPROGRAMS\Burn to the Brim"
  
  Delete "$DESKTOP\Burn to the Brim.lnk"
  
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\BTTB"
  DeleteRegKey HKCU "Software\BurnToTheBrim"
  
  ; Remove Explorer Context Menu integration if registered
  DeleteRegKey HKCU "Software\Classes\Directory\shell\BTTB"
SectionEnd
