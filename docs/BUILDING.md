# Building OpenRMH-IRCAM

## Prerequisites

Windows 10/11 x64 and **Visual Studio 2022** (Community, Professional or Build Tools) with these components
(Visual Studio Installer → Modify → *Individual components* if you use Build Tools):

| Component | Why |
|---|---|
| MSVC v143 x64/x86 build tools | compiler |
| Windows 10/11 SDK | headers/libs |
| **C++/CLI support for v143 build tools** | every source file is compiled with `/clr` (WinForms UI) |
| **C++ ATL for latest v143 build tools** | `atlconv.h` used by the DirectShow layer |
| **.NET Framework 4.7.2 targeting pack** | project targets .NET Framework 4.7.2 |

Command-line quick check (should list the C++/CLI component):

```powershell
& "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -all -products * `
  -requires Microsoft.VisualStudio.Component.VC.CLI.Support -property installationPath
```

## Steps

```powershell
.\scripts\fetch-deps.ps1     # once: OpenCV 4.9.0, GLFW 3.4, GLEW 2.3.1, C++/WinRT into deps\ and src\packages\
.\scripts\build.ps1          # Release|x64
```

Output: `src\x64\Release\IRCAM Thermal Viewer.exe`, with the runtime DLLs (OpenCV, GLEW, GLFW) copied beside it.

Or open `src\IRCAM Thermal Viewer.sln` in Visual Studio, choose **Release | x64**, and build.
(`scripts\fetch-deps.ps1` must have run first.)

## Notes

- **Toolset.** The project file requests platform toolset **v145** (Visual Studio 2026). `scripts\build.ps1` passes `v143` on
  Visual Studio 2022. In the IDE on 2022, change *Project → Properties → Platform Toolset* to v143, or *Retarget*.
- **Windows SDK.** The project targets "10.0" (latest installed). Pass `-WindowsSdk 10.0.22621.0` to pin one.
- **Where dependencies live.** `src\OpenRMH-IRCAM.deps.props` points at `..\deps`. To use libraries elsewhere:
  `msbuild ... /p:OpenRMHDepsDir=D:\libs\` (folder layout as `fetch-deps.ps1` produces).
- **Build from a local drive**, not a network share (slow; ownership checks can fail).
- **Debug|x64 is unmaintained**: it still references `opencv_world460d.lib`. Use Release until it is fixed.
- **Licensing code.** No licence check is compiled in; the original TurboActivate dependency was removed.

## Building the user manual (optional)

The user manual is written in Markdown (`docs/manual/manual.md`) and built into `build\manual\IRCAMSoftwareManual.pdf`,
the file the program's User Guide window opens. GitHub Actions builds it on every push. To build it locally (Python 3.9+):

```powershell
python -m pip install -r docs\manual\requirements.txt   # pinned pandoc (pypandoc_binary) and typst
python scripts\build-manual.py
```

Run it before `scripts\build.ps1` and the build copies the PDF next to the exe; the installer picks it up too.
Page setup and title are in `docs/manual/metadata.yaml`; `docs/manual/pdf.lua` sizes the images for the page.

## Building the installer (optional)

The GitHub Actions release build produces a Windows installer with [Inno Setup](https://jrsoftware.org/isinfo.php) 6
(preinstalled on GitHub's Windows runners) from `installer/setup.iss`. To build it locally:

```powershell
.\scripts\fetch-deps.ps1
.\scripts\build.ps1
# The redistributable is optional locally; the installer skips it if the file isn't there.
Invoke-WebRequest https://aka.ms/vc14/vc_redist.x64.exe -OutFile installer\payload\vc_redist.x64.exe
& "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer\setup.iss /DAppVersion=3.0.0-community.local
```

Output: `installer-out\OpenRMH-IRCAM-<version>-setup.exe`. The script is adapted from the original author's own
Inno Setup script (not included in this repository) to use paths relative to this repo instead of his desktop folders.

## Troubleshooting

| Symptom | Cause / fix |
|---|---|
| `MSB8020: build tools for v145 cannot be found` | You have VS 2022: use `scripts\build.ps1` or set the toolset to v143 |
| `C1083: cannot open include file 'atlconv.h'` | Install the ATL component |
| `error C1190` / `/clr` errors, or `MSB8042/8041` | C++/CLI component missing |
| `C1083: 'opencv2/opencv.hpp'` | `deps\` missing — run `fetch-deps.ps1`, or check `OpenRMHDepsDir` |
| `LINK1104` / cannot open `opencv_world490.lib` | same as above |
| Program starts but shows an OpenCV DLL error | build used `Release|x64`? DLLs are copied into the output folder by the `CopyRuntimeDeps` target |
