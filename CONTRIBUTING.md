# Contributing to OpenRMH-IRCAM

Thanks for helping. This project is a Windows (C++/CLI, .NET Framework 4.7.2) thermal-camera viewer that descends from
Rune Mark Hansen's IRCAM Thermal Viewer. Contributions are accepted under the [MIT licence](LICENSE).

## The quickest useful contributions

- **Test another camera** from [`docs/SUPPORTED-CAMERAS.md`](docs/SUPPORTED-CAMERAS.md) and open an issue with the model, the Windows
  device name (Device Manager, Cameras), which list entry you picked, and whether live view, calibration and range switching work.
- **Report a camera that isn't found**: include the Windows device name and USB ID. Many models show up as "USB Camera".
- **Fix a translation**: the comments were translated from Danish to English; if one is wrong or unclear, a pull request is welcome.
- **Read [`docs/DEVELOPER-GUIDE.md`](docs/DEVELOPER-GUIDE.md)** for how a frame flows through the program, and how to add a model.

## Build

Follow [`docs/BUILDING.md`](docs/BUILDING.md): `scripts\fetch-deps.ps1`, then `scripts\build.ps1` (Release|x64, Visual Studio 2022).
Every push and pull request is built by GitHub Actions (`.github/workflows/build.yml`); it must pass.
You can run the program without a camera using *Snapshot Analysis* and *Recording Analysis* mode on saved files.

## Pull requests

- Keep a change to **one purpose**; separate cleanups from behaviour changes.
- Say **how you tested it** (which camera/OS, or that it is comment/doc-only).
- Don't commit binaries, downloaded libraries, build output, licence keys, or personal paths. `.gitignore` covers the usual ones;
  third-party libraries come from `scripts\fetch-deps.ps1`, never from the repository.
- New files get an SPDX header (`SPDX-License-Identifier: MIT` and a copyright line).
- If you add or change third-party code, add it to [`NOTICE.md`](NOTICE.md) with its licence.

## Code style

Match the surrounding code; it is unusually consistent.

- **Names.** `RMH_` prefix, explicit `Module_Action` names (`RMH_IRThermalCamera_ReadFrameRaw`), `_Macro_Style` constants.
- **Comments.** English. The house style is a comment above every step, and a block above every routine saying what it does and
  what it returns. Keep that up rather than commenting less.
- **Encoding and line endings.** Source files are a mix of UTF-8 and Windows-1252 with CRLF. Keep each file's encoding and line
  endings as they are; don't re-save whole files in another encoding (it makes the diff unreadable). Prefer ASCII in new comments.
- **Threads.** Shared state is coordinated by simple flags (see the developer guide). Do all WinForms/OpenGL work on the UI thread.
- **No networking, no licence checks, no telemetry.** The program is meant to stay stand-alone. The only outward action is opening a
  link in the user's browser when the user clicks it.

## Behaviour of the maintainers

Small, focused pull requests are reviewed quickly; big ones may get "please split this". If you plan something large (new
protocol pool, UI rework, port to another platform), open an issue first so we can agree on the approach.
