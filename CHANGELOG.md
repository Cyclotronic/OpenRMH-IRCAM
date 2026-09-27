# Changelog

Tags look like `v<upstream version>-community.<n>`: the first part is the version of IRCAM Thermal Viewer the source came from,
the number counts builds of this repository.

## v3.0.0-community.4 - 2026-09-27

- User manual: the author's manual, converted from the Word original in his source release to Markdown
  (`docs/manual/`) and updated for 3.0.0 as a community edition. The sections on buying and activating a licence and
  the proprietary Terms of Use (which applied to the earlier commercial release and contradicted the MIT licence) are
  removed; the camera list, MATLAB examples and several settings are updated. GitHub Actions builds the PDF
  (`scripts/build-manual.py`, pinned pandoc + Typst). It ships in the installer and the zip as
  `IRCAMSoftwareManual.pdf`, and each release attaches it.
- The User Guide button opens the manual in the system's default PDF viewer. The embedded viewer it replaces worked
  only with Adobe Reader installed. If the PDF is missing or no viewer is installed, the status panel says so.
- Input validation: a frame from a file or a camera is now checked against the size of the frame buffers before it is copied
  or indexed (`src/RMH_FrameBufferLimits.h`). Previously an oversized or corrupt snapshot PNG, recording AVI, or camera
  video format could write past the end of the global frame buffers.
  - Snapshot PNGs must be 24-bit RGB and fit the buffers; recordings must have frames that fit and a readable first frame.
  - Metadata stored in snapshot and recording files (camera pool, metadata rows, pixel offsets) is checked before use;
    an invalid file is reported as corrupt instead of being loaded.
  - A camera whose video format is larger than the buffers is refused at connect with an error message.
  - The DirectShow layer takes an optional destination size and refuses to copy a larger frame.
- Fixed the centre-pixel index overflowing for 640-pixel-wide sensors (pool 3).
- Saved-session values are range-checked: an out-of-range list index is ignored and the recording frame rate is clamped.
- Verified on hardware with a Thermal Master P2 Pro: camera connect, snapshot, recording, saving and loading a session,
  the user manual button, and rejecting a file that isn't a valid snapshot or recording.

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
