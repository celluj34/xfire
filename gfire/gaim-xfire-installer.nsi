; NSIS Script For XFIRE Plugin
; Author Wolf Beat
; Based on the Gaim installer by Herman Bloggs and the Gaim-Encryption installer by Bill Tompkins
; Uses NSIS v2.0

Name "Gaim-XFIRE ${XFIRE_VERSION}"

; Registry keys:
!define XFIRE_REG_KEY        "SOFTWARE\gaim-xfire"
!define XFIRE_UNINSTALL_KEY  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\gaim-xfire"
!define XFIRE_UNINST_EXE     "gaim-xfire-uninst.exe"
!define XFIRE_DLL            "libxfire.dll"
!define XFIRE_UNINSTALL_LNK  "Gaim-XFIRE Uninstall.lnk"


!include "MUI.nsh"

;Do A CRC Check
CRCCheck On

;Output File Name
OutFile "gaim-xfire-${XFIRE_VERSION}.exe"

ShowInstDetails show
ShowUnInstDetails show
SetCompressor lzma

; Translations
!include "nsis\translations\english.nsh"


; Modern UI Configuration

!define MUI_ICON .\nsis\install.ico
!define MUI_UNICON .\nsis\install.ico
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "nsis\header.bmp"


; Pages
!define MUI_WELCOMEPAGE_TITLE $(WELCOME_TITLE)
!define MUI_WELCOMEPAGE_TEXT $(WELCOME_TEXT)
!insertmacro MUI_PAGE_WELCOME

!insertmacro MUI_PAGE_LICENSE  ".\COPYING"

!define MUI_DIRECTORYPAGE_TEXT_TOP $(DIR_SUBTITLE)
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION $(DIR_INNERTEXT)
!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_TITLE $(FINISH_TITLE)
!define MUI_FINISHPAGE_TEXT $(FINISH_TEXT)
!insertmacro MUI_PAGE_FINISH

; MUI Config

!define MUI_CUSTOMFUNCTION_GUIINIT checkGaimVersion
!define MUI_ABORTWARNING
!define MUI_UNINSTALLER
!define MUI_PROGRESSBAR smooth
!define MUI_INSTALLCOLORS /windows

!insertmacro MUI_LANGUAGE "English"

!define MUI_LICENSEPAGE_RADIOBUTTONS


;The Default Installation Directory
InstallDir "$PROGRAMFILES\gaim"
InstallDirRegKey HKLM SOFTWARE\gaim ""

Section -SecUninstallOldPlugin
  ; Check install rights..
  Call CheckUserInstallRights
  Pop $R0

  StrCmp $R0 "HKLM" rights_hklm
  StrCmp $R0 "HKCU" rights_hkcu done

  rights_hkcu:
      ReadRegStr $R1 HKCU "${XFIRE_REG_KEY}" ""
      ReadRegStr $R2 HKCU "${XFIRE_REG_KEY}" "Version"
      ReadRegStr $R3 HKCU "${XFIRE_UNINSTALL_KEY}" "UninstallString"
      Goto try_uninstall

  rights_hklm:
      ReadRegStr $R1 HKLM "${XFIRE_REG_KEY}" ""
      ReadRegStr $R2 HKLM "${XFIRE_REG_KEY}" "Version"
      ReadRegStr $R3 HKLM "${XFIRE_UNINSTALL_KEY}" "UninstallString"

  ; If previous version exists .. remove
  try_uninstall:
    StrCmp $R1 "" done
      StrCmp $R2 "" uninstall_problem
        IfFileExists $R3 0 uninstall_problem
          ; Have uninstall string.. go ahead and uninstall.
          SetOverwrite on
          ; Need to copy uninstaller outside of the install dir
          ClearErrors
          CopyFiles /SILENT $R3 "$TEMP\${XFIRE_UNINST_EXE}"
          SetOverwrite off
          IfErrors uninstall_problem
            ; Ready to uninstall..
            ClearErrors
            ExecWait '"$TEMP\${XFIRE_UNINST_EXE}" /S _?=$R1'
            IfErrors exec_error
              Delete "$TEMP\${XFIRE_UNINST_EXE}"
              Goto done

            exec_error:
              Delete "$TEMP\${XFIRE_UNINST_EXE}"
              Goto uninstall_problem

        uninstall_problem:
            ; Just delete the plugin and uninstaller, and remove Registry key
             MessageBox MB_YESNO $(XFIRE_PROMPT_WIPEOUT) IDYES do_wipeout IDNO cancel_install
          cancel_install:
            Quit

          do_wipeout:
            StrCmp $R0 "HKLM" del_lm_reg del_cu_reg
            del_cu_reg:
              DeleteRegKey HKCU ${XFIRE_REG_KEY}
              Goto uninstall_prob_cont
            del_lm_reg:
              DeleteRegKey HKLM ${XFIRE_REG_KEY}

            uninstall_prob_cont:
              ; plugin DLL
              Delete "$R1\plugins\${XFIRE_DLL}"
              ; pixmaps
              Delete "$R1\pixmaps\gaim\status\default\xfire.png"
              Delete "$R3"

  done:

SectionEnd


Section "Install"
  Call CheckUserInstallRights
  Pop $R0

  StrCmp $R0 "NONE" instrights_none
  StrCmp $R0 "HKLM" instrights_hklm instrights_hkcu

  instrights_hklm:
    ; Write the version registry keys:
    WriteRegStr HKLM ${XFIRE_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKLM ${XFIRE_REG_KEY} "Version" "${XFIRE_VERSION}"

    ; Write the uninstall keys for Windows
    WriteRegStr HKLM ${XFIRE_UNINSTALL_KEY} "DisplayName" "$(XFIRE_UNINSTALL_DESC)"
    WriteRegStr HKLM ${XFIRE_UNINSTALL_KEY} "UninstallString" "$INSTDIR\${XFIRE_UNINST_EXE}"
    SetShellVarContext "all"
    Goto install_files

  instrights_hkcu:
    ; Write the version registry keys:
    WriteRegStr HKCU ${XFIRE_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKCU ${XFIRE_REG_KEY} "Version" "${XFIRE_VERSION}"

    ; Write the uninstall keys for Windows
    WriteRegStr HKCU ${XFIRE_UNINSTALL_KEY} "DisplayName" "$(XFIRE_UNINSTALL_DESC)"
    WriteRegStr HKCU ${XFIRE_UNINSTALL_KEY} "UninstallString" "$INSTDIR\${XFIRE_UNINST_EXE}"
    Goto install_files
  
  instrights_none:
    ; No registry keys for us...
    
  install_files:
    SetOutPath "$INSTDIR\plugins"
    SetCompress Auto
    SetOverwrite on
    File "src\${XFIRE_DLL}"
    
    SetOutPath "$INSTDIR\pixmaps\gaim\status\default"
    File "pixmaps\xfire.png"
    
    StrCmp $R0 "NONE" done
    CreateShortCut "$SMPROGRAMS\Gaim\${XFIRE_UNINSTALL_LNK}" "$INSTDIR\${XFIRE_UNINST_EXE}"
    WriteUninstaller "$INSTDIR\${XFIRE_UNINST_EXE}"
    SetOverWrite off

  done:
SectionEnd

Section Uninstall
  Call un.CheckUserInstallRights
  Pop $R0
  StrCmp $R0 "NONE" no_rights
  StrCmp $R0 "HKCU" try_hkcu try_hklm

  try_hkcu:
    ReadRegStr $R0 HKCU "${XFIRE_REG_KEY}" ""
    StrCmp $R0 $INSTDIR 0 cant_uninstall
      ; HKCU install path matches our INSTDIR.. so uninstall
      DeleteRegKey HKCU "${XFIRE_REG_KEY}"
      DeleteRegKey HKCU "${XFIRE_UNINSTALL_KEY}"
      Goto cont_uninstall

  try_hklm:
    ReadRegStr $R0 HKLM "${XFIRE_REG_KEY}" ""
    StrCmp $R0 $INSTDIR 0 try_hkcu
      ; HKLM install path matches our INSTDIR.. so uninstall
      DeleteRegKey HKLM "${XFIRE_REG_KEY}"
      DeleteRegKey HKLM "${XFIRE_UNINSTALL_KEY}"
      ; Sets start menu and desktop scope to all users..
      SetShellVarContext "all"

  cont_uninstall:
    ; plugin 
    Delete "$INSTDIR\plugins\${XFIRE_DLL}"
    ; pixmaps
    Delete "$INSTDIR\pixmaps\gaim\status\default\xfire.png"
    ; uninstaller
    Delete "$INSTDIR\${XFIRE_UNINST_EXE}"
    ; uninstaller shortcut
    Delete "$SMPROGRAMS\Gaim\${XFIRE_UNINSTALL_LNK}"
    
    ; try to delete the Gaim directories, in case it has already uninstalled
    RMDir "$INSTDIR\plugins"
    RMDir "$INSTDIR"
    RMDir "$SMPROGRAMS\Gaim"

    Goto done

  cant_uninstall:
    MessageBox MB_OK $(un.XFIRE_UNINSTALL_ERROR_1) IDOK
    Quit

  no_rights:
    MessageBox MB_OK $(un.XFIRE_UNINSTALL_ERROR_2) IDOK
    Quit

  done:
SectionEnd

Function .onVerifyInstDir
  IfFileExists $INSTDIR\gaim.exe Good1
    Abort
  Good1:
FunctionEnd

Function checkGaimVersion
  Push $R0
  Push $R1
  Push $R2

  ClearErrors
  ReadRegStr $R0 HKLM "SOFTWARE\gaim" "version"
  IfErrors readHKCUGaimVersion compareVersion

  readHKCUGaimVersion:
  ReadRegStr $R0 HKCU "SOFTWARE\gaim" "version"
  IfErrors noGaimInstallFound compareVersion

  noGaimInstallFound:
    MessageBox MB_OK|MB_ICONSTOP "$(NO_GAIM_VERSION)"
    Quit

  compareVersion:
  StrCmp $R0 ${GAIM_VERSION} GaimVersionOK
    ; Versions are in the form of X.Y.Z
    ; If X is different, then we shouldn't install
    Push $R0
    Call GetMajorVersion
    IfErrors BadVersion
    Pop $R2
    Push ${GAIM_VERSION}
    Call GetMajorVersion
    IfErrors noGaimInstallFound
    Pop $R1
    ;Check that both version's X is the same
    IntCmp $R1 $R2 GaimVersionOK
    BadVersion:
    MessageBox MB_OK|MB_ICONSTOP "$(BAD_GAIM_VERSION_1) $R0 $(BAD_GAIM_VERSION_2)"
    Quit
  GaimVersionOK:
  Pop $R2
  Pop $R1
  Pop $R0
FunctionEnd

; Extract the part of a string prior to "." (or the whole string if there is no ".")
; If no "." was found, the ErrorFlag will be set
Function GetMajorVersion
    ClearErrors
    Pop $0
    StrCpy $1 "0"

    startGetMajorVersionLoop:
    ;avoid an infinite loop, if we have gotten the whole initial string, exit the loop and set the error flag
    StrCmp $2 $0 GetMajorVersionSetErrorFlag
    IntOp $1 $1 + 1
    ;Append the next character in $0 to $2
    StrCpy $2 $0 $1
    ;store the next character in $3
    StrCpy $3 $0 1 $1
    ;if the next character is ".", $2 will contain the string prior to "."
    StrCmp $3 "." endGetMajorVersion startGetMajorVersionLoop

    GetMajorVersionSetErrorFlag:
    SetErrors

    endGetMajorVersion:

    Push $2
FunctionEnd

Function CheckUserInstallRights
        ClearErrors
        UserInfo::GetName
        IfErrors Win9x
        Pop $0
        UserInfo::GetAccountType
        Pop $1

        StrCmp $1 "Admin" 0 +3
                StrCpy $1 "HKLM"
                Goto done
        StrCmp $1 "Power" 0 +3
                StrCpy $1 "HKLM"
                Goto done
        StrCmp $1 "User" 0 +3
                StrCpy $1 "HKCU"
                Goto done
        StrCmp $1 "Guest" 0 +3
                StrCpy $1 "NONE"
                Goto done
        ; Unknown error
        StrCpy $1 "NONE"
        Goto done

        Win9x:
                StrCpy $1 "HKLM"

        done:
        Push $1
FunctionEnd

Function un.CheckUserInstallRights
        ClearErrors
        UserInfo::GetName
        IfErrors Win9x
        Pop $0
        UserInfo::GetAccountType
        Pop $1
        StrCmp $1 "Admin" 0 +3
                StrCpy $1 "HKLM"
                Goto done
        StrCmp $1 "Power" 0 +3
                StrCpy $1 "HKLM"
                Goto done
        StrCmp $1 "User" 0 +3
                StrCpy $1 "HKCU"
                Goto done
        StrCmp $1 "Guest" 0 +3
                StrCpy $1 "NONE"
                Goto done
        ; Unknown error
        StrCpy $1 "NONE"
        Goto done

        Win9x:
                StrCpy $1 "HKLM"

        done:
        Push $1
FunctionEnd


