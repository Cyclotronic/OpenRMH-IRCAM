# OpenRMH-IRCAM

An open-source Windows viewer and analysis tool for low-cost USB thermal cameras (InfiRay, Thermal Master,
TOPDON, UNI-T, HTI and others). It shows a live radiometric image, measures temperatures (spots, regions, lines),
draws histograms and 3D surface plots, records and replays sessions, and exports temperature data for
analysis (CSV, with MATLAB examples).

> **Status:** early. Builds with Visual Studio 2022 and has been run against a Thermal Master P2 Pro.
> Other cameras on the supported list are untested by this project — reports welcome.

## Origin, status and intent

OpenRMH-IRCAM is an **unofficial community repository** of the source code of **IRCAM Thermal Viewer**, written by
**Rune Mark Hansen**. On 24-25 September 2026 the author announced that the complete source is released for everyone to
use, copy, modify and own for free, and shared it publicly. He did not name a licence, so this repository applies the
[MIT licence](LICENSE) with him as copyright holder ([`NOTICE.md`](NOTICE.md) has the details).

What this repository is for:

- **Keeping the author's work identifiable.** His code and headers are unchanged; changes made here are small, separate and
  listed in [`NOTICE.md`](NOTICE.md). Git history shows who changed what.
- **Making it easy to build.** The original share is about 9 GB, most of it Visual Studio build output and caches. This
  repository is source only (~16 MB); `scripts/fetch-deps.ps1` downloads the third-party libraries and a GitHub Actions
  workflow builds it on every push.
- **Staying stand-alone.** The program has no licence check, and its source contains no networking code. Its only
  outward action is opening a web link in your default browser when you click one.
- **Coordinating with the author.** If he publishes his own repository, this one will be aligned with it rather than compete.

Author: Rune Mark Hansen - <https://rmg-engineering.com/>

## Features

- Live view from USB thermal cameras (DirectShow/UVC) with temperature range switching and shutter calibration
- Cursor, spot, ROI and line temperature measurement; temperature alarms
- Emissivity table, colour palettes (custom palette creator), colour bar, histogram, 3D surface plot (OpenGL)
- Snapshot and recording analysis modes (work on saved data — no camera needed)
- Full-frame temperature export and data logging to CSV

## Supported cameras

See [`docs/SUPPORTED-CAMERAS.md`](docs/SUPPORTED-CAMERAS.md). **Choose your exact model in the camera list** — many
cameras appear to Windows as the generic name "USB Camera", so the list entry, not auto-detection, tells the
program which protocol to use. (Thermal Master P2 Pro → *"InfiRay Or Thermal Master P2Pro"*.)

## Build

Windows 10/11, Visual Studio 2022 (C++ desktop, C++/CLI, ATL, .NET Framework 4.7.2 targeting pack).

```powershell
git clone https://github.com/<owner>/OpenRMH-IRCAM.git
cd OpenRMH-IRCAM
.\scripts\fetch-deps.ps1      # downloads OpenCV 4.9.0, GLFW 3.4, GLEW 2.3.1, C++/WinRT (~200 MB, not stored in git)
.\scripts\build.ps1           # Release|x64  ->  src\x64\Release\IRCAM Thermal Viewer.exe
```

Details, prerequisites and troubleshooting: [`docs/BUILDING.md`](docs/BUILDING.md).

## Repository layout

```
src/          Visual Studio solution and all source (C++/CLI WinForms + OpenGL)
  res/          icons/images the build needs
scripts/      fetch-deps.ps1, build.ps1
docs/         building, supported cameras, architecture
examples/     MATLAB scripts that read the CSV exports
deps/         (git-ignored) third-party libraries fetched by the script
```

## Licence

[MIT](LICENSE) - copyright Rune Mark Hansen. Third-party libraries keep their own licences; see [`NOTICE.md`](NOTICE.md).

## Contributing

Issues and pull requests are welcome (contributions are accepted under the MIT licence). Please do not commit binaries, downloaded
dependencies, licence keys or personal paths (see `.gitignore`).
