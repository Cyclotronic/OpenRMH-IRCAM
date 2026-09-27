# Source map

A single C++/CLI executable: WinForms windows for the UI, native C++ (OpenCV, OpenGL, DirectShow) for the work.
Every source file is compiled with `/clr`. The code comments were **translated from Danish to English** (the original
author's language) in a comment-only change: the code is byte-for-byte identical once comments are removed. A number of
identifiers and a few variable names are still Danish-influenced; the original wording is in git history.

This is a file-level map from the file names and headers; it is not a design document.

| Area | Files (`src/`) |
|---|---|
| Entry point | `main.cpp` |
| Shared state | `GlobalObjectsAndVariables.{h,cpp}`, `ApplicationResource.h` |
| Camera access (DirectShow/UVC) | `ds_camera*.{h,cpp}`, `abstract_ds_camera.*`, `camera_device.*`, `uvc_camera*.{h,cpp}`, `ds_grabber_callback.*`, `ds_video_format.*`, `ds_guid.h`, `ds_libs_setting.h`, `qedit.h`, `cv_mat_convertor.*` |
| Supported cameras, protocols, temperature ranges | `RMH_SupportedIRCameras_Resources.h`, `RMH_ThermalCameraSupport_Library.{h,cpp}` |
| Image processing and temperature maths | `RMH_ImageProcessing_Library.*`, `RMH_MathConversions_Library.*` |
| Snapshot / recording analysis | `RMH_AnalysisMode_Routines.*` |
| Application logic | `RMH_Application_ThermalViewer.*`, `RMH_Application_SaveSession.*`, `RMH_Application_ColorBarAndPalette.*`, `RMH_Application_Information.h` |
| OpenGL rendering (2D plot, histogram, colour bar, 3D surface) | `RMH_OpenGL_*.h` |
| WinForms helpers | `RMH_Winforms_Library.*`, `RMH_LiveView_ZoomWindow.h` |
| Windows (each has a `.resx`) | `MainGUI`, `LiveViewStream`, `LiveViewTools`, `ThermalCameraGUI`, `TempMeasGUI`, `TempAlarmsGUI`, `SurfacePlotGUI`, `StatisticsWindow`, `EmissivityTableGUI`, `ColorPaletteCreator`, `ColorBarRangeDialog`, `InputValueDialog`, `PopUpDialog`, `VideoPlayBackTools`, `UserGuideViewerGUI`, `WelcomeScreen`, `SplashScreen` |
| Data tables | `RMH_*_Resources.h` (emissivity, palettes, alarms, data logging, triggers, full-frame data) |
| Build | `IRCAM Thermal Viewer.sln/.vcxproj`, `OpenRMH-IRCAM.deps.props`, `.rc`, `app.manifest.xml` |

## Build layout decisions

- Third-party libraries are **not** stored in the repository; `scripts/fetch-deps.ps1` downloads pinned versions into `deps/`.
- The project imports its dependency locations from `OpenRMH-IRCAM.deps.props`, so no absolute paths remain in the project file.
- The solution/project keep their original names (`IRCAM Thermal Viewer.*`) to stay close to upstream; the
  published name is OpenRMH-IRCAM.
