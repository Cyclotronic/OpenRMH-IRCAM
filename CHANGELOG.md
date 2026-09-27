# Changelog

Tags look like `v<upstream version>-community.<n>`: the first part is the version of IRCAM Thermal Viewer the source came from,
the number counts builds of this repository.

## Unreleased

- User manual: the author's manual, converted from the Word original in his source release to Markdown
  (`docs/manual/`) and updated for 3.0.0 as a community edition. The sections on buying and activating a licence and
  the proprietary Terms of Use (which applied to the earlier commercial release and contradicted the MIT licence) are
  removed; the camera list, MATLAB examples and several settings are updated. GitHub Actions builds the PDF
  (`scripts/build-manual.py`, pinned pandoc + Typst). It ships in the installer and the zip as
  `IRCAMSoftwareManual.pdf`, the file the User Guide window opens, and each release attaches it.

## v3.0.0-community.3 - 2026-09-27

- Windows installer (`OpenRMH-IRCAM-<version>-setup.exe`, Inno Setup) built alongside the portable zip. It installs the
  Microsoft Visual C++ Redistributable (x64) if needed, as the original author's installer did.
- No change to the program's behaviour.

## v3.0.0-community.2 - 2026-09-27

- Source comments translated from Danish to English (about 7,000 comment lines in 52 files). Code is unchanged: for every file
  the source with comments removed is identical to the original import. See `NOTICE.md`.
- New documentation: `docs/DEVELOPER-GUIDE.md` (how a frame flows from camera to screen, how to add a camera model) and
  `CONTRIBUTING.md`. `docs/ARCHITECTURE.md` updated.
- No change to the program's behaviour.

## v3.0.0-community.1 - 2026-09-26

- First public build: IRCAM Thermal Viewer 3.0.0 (September 2026 source release by Rune Mark Hansen), MIT licensed.
- Source-only repository with a scripted dependency fetch (`scripts/fetch-deps.ps1`), a build script, and a GitHub Actions build.
- Removed the unused TurboActivate link dependency; dependency and icon paths made portable.
- Verified with a Thermal Master P2 Pro (list entry "InfiRay Or Thermal Master P2Pro").
