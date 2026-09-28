# Developer guide: how a frame gets from the camera to the screen

This is a walkthrough of the runtime path, written from the source (function names are real; grep for them).
For a file-level map see [`ARCHITECTURE.md`](ARCHITECTURE.md); for supported models see
[`SUPPORTED-CAMERAS.md`](SUPPORTED-CAMERAS.md).

## 1. Choosing and opening a camera

The user picks a model from the camera list (`SupportedCamerasModelNames`, in `RMH_SupportedIRCameras_Resources.h`).
`RMH_IRThermalCamera_ConnectToThermalCamera()` (`RMH_ThermalCameraSupport_Library.cpp`):

1. maps the selected list index to a **pool** (protocol family 1-4), a frame rate, and a list of accepted DirectShow names;
2. enumerates DirectShow devices and looks for one whose friendly name **exactly equals** an accepted name;
3. opens it with `RMH_IRThermalCamera_OpenIRCameraDevice()`, which differs by pool:
   pools 1 and 3 open the device and switch it into raw mode with a vendor control write (`setZoom(0x8004)` / `0x8005`);
   pools 2 and 4 open it with a frame size that includes the extra rows carrying thermal data;
4. sets the default temperature range, runs a calibration, then reads the pool's frame geometry and metadata size.

The low-level camera access (DirectShow graph, UVC controls) is in `ds_camera*`, `uvc_camera*`, `camera_device.*`.

## 2. The three loops that run while a camera is connected

Defined in `MainGUI.h`:

| Loop | Type | Does |
|---|---|---|
| `VideoStreamThread_DoWork` | `BackgroundWorker` | While connected: if the previous frame has been consumed (`ThreadDataReadyFlag == false`), run `RMH_ThermalViewer_ImageProcessingSequence()` and set the flag; sleep 1 ms |
| `SecondaryProcessingThread_DoWork` | `BackgroundWorker` | While connected: `RMH_ThermalViewer_SecondaryProcessingSequence()` (extra processing such as the bilinear interpolation for the ultra-resolution mode); sleep 1 ms |
| `MainGUIUpdateTimer_Tick` | WinForms timer, 5 ms | If `ThreadDataReadyFlag` is set: draw the live view, colorbar and histogram, write recording data, update the surface plot, 2D plot, alarm labels and statistics, then clear the flag |

So processing (worker thread) and drawing (UI thread) are decoupled by one flag: a frame is produced, drawn, then the
next one is produced. Everything that touches WinForms controls or OpenGL contexts happens in the timer tick.

## 3. `RMH_ThermalViewer_ImageProcessingSequence()` step by step

`RMH_Application_ThermalViewer.cpp`. It runs only if live view is running or a single-frame trigger is pending.

1. **Get raw data.** From the camera: `RMH_IRThermalCamera_ReadFrameRaw()`. In *Recording Analysis* mode: the frame is read from the AVI
   (`RMH_VideoFileReading_ReadVideoFileFrame`). *Snapshot Analysis* works from the saved snapshot data instead of a live camera.
2. **Raw YUY2 to a 16-bit thermal array** (14-bit full scale, `RMH_IRThermalCamera_ConvertYUY2To14BitThermalDataArray`), keeping the frame's average. Pool 3 also applies
   non-uniformity correction here. Pools 2, 3 and 4 append max/min/center values after the frame data; pool 1 already carries them in the
   frame metadata.
3. **Frame metadata.** `RMH_IRThermalCamera_ReadCalFrameMetaData()` reads detector/shutter/core temperatures and the
   max/min/center raw values and coordinates; `RMH_ThermalViewer_ReadMaxMinCentTemperatures()` turns them into temperatures.
4. **Sharpening kernel** regenerated if its parameters changed (`RMH_ImageProcessing_GenerateUnsharpKernelMask`).
5. **Colorbar range** decided: automatic, manual, manual-high or manual-low.
6. **Grayscale by automatic gain control.**
   Manual range: `RMH_IRThermalCamera_LinearAutomaticGainControlTemp`. Adaptive range: `RMH_ImageProcessing_LinearAutomaticGainControlRaw`.
   Optionally followed by Gaussian unsharp sharpening (`RMH_ImageProcessing_2DUnsharpMaskKernelImageSharpening`).
7. **Colour palette.** The palette is fitted inside the colorbar's min/max tags
   (`RMH_ImageProcessing_FormatColorPaletteRangeInsideBackgroundPalette`) and applied to the grayscale image
   (`RMH_ImageProcessing_ApplyColorPaletteToGrayscaleImageData`, or `RMH_ImageProcessing_ApplyOverlayedPaletteToGrayScaleImageData` for the dual palette).
8. **Histogram bins** for the chosen source (whole frame, zoom ROI, ROI or line), in manual or auto range.
9. **Measurements.** Max/min/center labels, mouse-cursor temperature, ROIs, spot measurements, lines, 2D plot samples,
   data logging, and alarm evaluation.

### From raw counts to temperature

Which method is used depends on the pool (`RMH_IRThermalCamera_ReadPixelTemperature` and `RMH_IRThermalCamera_ReadFramePixelTemperature` in
`RMH_ThermalCameraSupport_Library.cpp`): pools 1 and 3 read the temperature from a **look-up table** built by
`RMH_IRThermalCamera_GenerateThermoGrapicLookUpTable` from the camera's calibration parameters (atmospheric transmission and
emissivity/reflected/ambient/humidity/distance settings are applied);
pools 2 and 4 **calculate per pixel** from the raw value. Results are then compensated for the selected unit
(°C / °F / K) via a scale factor and offset.

## 4. Drawing

Each drawing surface is a WinForms panel hosting an OpenGL texture, wrapped by a class in `RMH_OpenGL_*.h`
(`RMH_OpenGL_Winforms.h` for the live view with movable rectangles, crosshairs and lines; `_ColorBar`, `_Histogram`, `_2DPlot`,
`_SurfacePlot`, `RMH_LiveView_ZoomWindow.h`). A transparent overlay panel on top handles mouse interaction. Rendering ends with a
buffer swap; every frame follows the same shape: clear, bind texture, draw, draw objects, swap.

## 5. Recording and analysis

- **Recording** writes AVI files through OpenCV's `VideoWriter` (`RMH_AnalysisMode_Routines.cpp`). RAW recordings add one extra pixel row of
  identification/metadata (camera pool, geometry, correction/ambient/reflected/humidity/emissivity/distance settings) so the file
  can be recognised and re-analysed later.
- **Recording Analysis** replays such an AVI; **Snapshot Analysis** reads a PNG snapshot the same way. Neither needs a camera.
- **Data logging** and **full-frame temperature export** write CSV; see `examples/matlab` for readers.

## 6. Adding a camera model (checklist)

1. `RMH_SupportedIRCameras_Resources.h`: add a `_SupportedThermalCamera_<Name>` index macro, add the display name to
   `SupportedCamerasModelNames` (position must match the index), add a `<Name>DeviceNames` list with the DirectShow friendly name(s).
2. `RMH_ThermalCameraSupport_Library.cpp`: in `RMH_IRThermalCamera_ConnectToThermalCamera()` add the `case` that sets its pool,
   device-name list and frame rate; add the `...SupportsHighRange` flag where the other models are handled.
3. Pick the pool whose frame layout and controls match. A new layout means a new pool: it touches the frame conversion, metadata
   reading, calibration, temperature calculation and range-switching `switch` statements (search for `_SupportedThermalCameras_Pool_4`
   to see every place a pool is handled).
4. Add the model to `docs/SUPPORTED-CAMERAS.md` with its Windows device name, and say how you tested it.

### If the camera has no DirectShow/UVC interface at all

Some cameras (e.g. Thermal Master P3, Pool 5) only expose a vendor-specific WinUSB interface — no
amount of DirectShow device-name matching will ever find them. `p3_winusb_camera.h`/`.cpp` is a
self-contained example of an alternative backend for this case: it opens the device directly via the
Win32 WinUSB API and implements the vendor's own control/streaming protocol (reverse engineered from a
USB capture of the vendor's app — see that file's header comment). To wire in a new backend of this
kind: give the camera its own pool number, have `RMH_IRThermalCamera_ConnectToThermalCamera()` bypass
the DirectShow enumeration entirely for that pool (early-return once your backend's `open()` succeeds),
and branch the handful of acquisition functions (`OpenIRCameraDevice`, `CloseIRCameraDevice`,
`StartCapturing`, `StopCapturing`, `CheckForCameraDisconnection`, `ReadFrameRaw`, `ReadCameraFPS`) on
that pool number to call your backend instead of the shared `IRThermalCamera` object. If the raw frame
layout matches an existing pool once acquired (check the total byte count against
`width*height*bytesPerPixel`), every other pool-keyed switch statement can just add your pool number
as a fallthrough case alongside the matching one instead of duplicating its logic.

## Things worth knowing

- Camera identification is by **name only**, and several models share the generic name `USB Camera`. That is why the user must pick the model.
- A few flags coordinate threads (`ThreadDataReadyFlag`, `IRCamera.ConnectedFlag`); there are no locks, so keep new shared state simple
  or follow the same produce-then-consume pattern.
- Identifiers use an `RMH_` prefix and a very explicit `Module_Action` naming style. Match it.
