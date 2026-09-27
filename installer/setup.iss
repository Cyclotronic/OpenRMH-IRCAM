; SPDX-License-Identifier: MIT
; Copyright (c) 2026 Cyclotronic
;
; Inno Setup script for OpenRMH-IRCAM, adapted from the original author's installer script
; (assets of the recovered download; not included in this repository - see NOTICE.md) to use
; portable, CI-relative paths instead of the author's own desktop folders.
;
; Built by .github/workflows/build.yml on a tagged release. To build locally:
;   1. scripts\fetch-deps.ps1 && scripts\build.ps1        (produces src\x64\Release\*)
;   2. Copy the runtime DLLs next to the exe (the build script's CopyRuntimeDeps target does this)
;   3. Download the VC++ redistributable to installer\payload\vc_redist.x64.exe (see docs\BUILDING.md
;      for the link) - or pass /DSkipRedist=1 to ISCC to build without it
;   4. "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer\setup.iss /DAppVersion=3.0.0-community.local

#ifndef AppVersion
  #define AppVersion "0.0.0-dev"
#endif
#define RepoRoot ".."
#define PayloadDir "payload"
#define BuildOut RepoRoot + "\src\x64\Release"

[Setup]
AppId={{AD5A507E-12AB-4E47-962E-DBA9061FB641}
AppName=OpenRMH-IRCAM
AppVersion={#AppVersion}
AppPublisher=OpenRMH-IRCAM contributors
AppPublisherURL=https://github.com/Cyclotronic/OpenRMH-IRCAM
AppSupportURL=https://github.com/Cyclotronic/OpenRMH-IRCAM/issues
VersionInfoDescription=IRCAM Thermal Viewer (community build)
DefaultDirName={autopf}\OpenRMH-IRCAM
DefaultGroupName=OpenRMH-IRCAM
UninstallDisplayIcon={app}\IRCAM Thermal Viewer.exe
OutputDir=..\installer-out
OutputBaseFilename=OpenRMH-IRCAM-{#AppVersion}-setup
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin
ArchitecturesInstallIn64BitMode=x64
LicenseFile=..\LICENSE
WizardStyle=modern
DisableWelcomePage=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
Source: "{#BuildOut}\IRCAM Thermal Viewer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildOut}\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\NOTICE.md"; DestDir: "{app}"; Flags: ignoreversion
; User manual (scripts\build-manual.py); the User Guide button opens it from the program folder
Source: "..\build\manual\IRCAMSoftwareManual.pdf"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "..\deps\opencv\LICENSE.txt"; DestDir: "{app}\licenses"; DestName: "OpenCV-LICENSE.txt"; Flags: ignoreversion skipifsourcedoesntexist
Source: "..\deps\opencv\LICENSE_FFMPEG.txt"; DestDir: "{app}\licenses"; DestName: "OpenCV-FFmpeg-LGPL-LICENSE.txt"; Flags: ignoreversion skipifsourcedoesntexist
Source: "..\deps\glfw\LICENSE.md"; DestDir: "{app}\licenses"; DestName: "GLFW-LICENSE.md"; Flags: ignoreversion skipifsourcedoesntexist
Source: "..\deps\glew\LICENSE.txt"; DestDir: "{app}\licenses"; DestName: "GLEW-LICENSE.txt"; Flags: ignoreversion skipifsourcedoesntexist
#ifndef SkipRedist
Source: "{#PayloadDir}\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall skipifsourcedoesntexist
#endif

[Icons]
Name: "{group}\OpenRMH-IRCAM"; Filename: "{app}\IRCAM Thermal Viewer.exe"
Name: "{group}\OpenRMH-IRCAM User Manual"; Filename: "{app}\IRCAMSoftwareManual.pdf"; Check: FileExists(ExpandConstant('{app}\IRCAMSoftwareManual.pdf'))
Name: "{group}\{cm:UninstallProgram,OpenRMH-IRCAM}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\OpenRMH-IRCAM"; Filename: "{app}\IRCAM Thermal Viewer.exe"; Tasks: desktopicon

[Run]
#ifndef SkipRedist
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing the Microsoft Visual C++ Redistributable..."; Flags: waituntilterminated skipifdoesntexist
#endif
Filename: "{app}\IRCAM Thermal Viewer.exe"; Description: "Launch OpenRMH-IRCAM"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}\licenses"
