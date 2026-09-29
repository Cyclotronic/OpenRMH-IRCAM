# Supported cameras

The program does **not** identify a camera by USB ID. You pick a model from the camera list; the program then looks for a
DirectShow device whose *friendly name* exactly equals one of the names listed for that model, and applies that model's
protocol ("pool"). Many cameras register with Windows as the generic name **"USB Camera"**, so choosing the right list
entry matters — a wrong choice simply reports that no device was found.

Device names come from `src/RMH_SupportedIRCameras_Resources.h`.

List entries below are in the same alphabetical order as the camera selection ComboBox.

| List entry | Windows device name(s) matched | Protocol pool |
|---|---|---|
| HTI HT-301 | `T3`, `T3-317-13`, `T3-317-68`, `HT-301` | 1 |
| InfiRay DV-DL13 | `VirtualBox Webcam - DV-DL13`, `DV-DL13` | 1 |
| **InfiRay Or Thermal Master P2Pro** | `USB Camera`, `Camera` | 2 |
| InfiRay P2 | `USB Camera`, `Camera` | 2 |
| InfiRay S0 Series | `S0-90W`, `S0-40`, `S0-68`, `S0-90` | 1 |
| InfiRay T2-Search / V2 | `T2-Search`, `T2`, `T2S`, … | 1 / 3 |
| InfiRay T2L / T2L V2 | `T2L-A4L`, `T2L-A6L`, `T2L-A8L`, `T2L`, … | 1 / 3 |
| InfiRay T2Pro and T2SPro / V2 | `T2Pro`, `T2+`, `T2p`, `T2P`, `T2SPro`, … | 1 / 3 |
| InfiRay T2S+ / V2 | `T2S+`, `T2Sp`, `T2SPro`, … | 1 / 3 |
| InfiRay T3-Search | `T3-Search`, `T3` | 1 |
| InfiRay T3Pro | `T3Pro-A13`, `T3Pro-A68`, `T3Pro`, … | 1 |
| InfiRay T3S | `Xtherm-T3S`, `T3S-A68`, `T3S-A13`, `T3S`, … | 1 |
| InfiRay Tiny1-C | `USB Camera`, `Tiny1C` | 2 |
| LODESTAR L2 | `USB Camera` | 2 |
| Thermal Master P2 | `Camera` | 4 |
| **Thermal Master P3** | *(not DirectShow — see below)* | 5 |
| **Thermal Master THOR001** | `THOR001` | 6 |
| TOPDON TC001 / TS001 | `USB Camera`, `TC001` | 2 |
| TOPDON TC002 / TC003 | `USB Camera`, `TC002`, `TC003` | 2 |
| UNI-T UTi260M | `USB Camera` | 2 |
| Victor 328B | `USB Camera` | 2 |
| Snapshot Analysis / Recording Analysis | (no camera — works on saved data) | — |

### Thermal Master P3 (Pool 5): not a DirectShow camera

The P3 (VID `3474`, PID `45A2`) has no USB Video Class interface at all — Windows binds WinUSB to it
(`P3.inf`), and the vendor's own app talks to it with a private protocol, not DirectShow. Pool 5 is a
second camera backend, entirely separate from the DirectShow path every other entry in this table uses:
`src/p3_winusb_camera.h`/`.cpp` open the device directly via WinUSB, replicate the vendor's control
protocol (reverse engineered from a USB capture — see that file's header comment for the wire format),
and pull raw frames over a bulk endpoint. The frame format is byte-for-byte the same as Pool 4 (Thermal
Master P2), so it is fed into the same conversion/temperature code once acquired; only the acquisition
(`RMH_IRThermalCamera_OpenIRCameraDevice` / `_CloseIRCameraDevice` / `_ReadFrameRaw` / etc. in
`RMH_ThermalCameraSupport_Library.cpp`) branches on `_SupportedThermalCameras_Pool_5` to call into
`P3WinUsbCamera` instead of the shared `IRThermalCamera` DirectShow object.

Two quirks specific to this camera, both handled in `RMH_ThermalCameraSupport_Library.cpp`/
`p3_winusb_camera.cpp` and not applicable to Pool 4's other camera (Thermal Master P2):
`FrameWidthPixelOffset` is set to 6 to skip a fixed 6-pixel dead zone at the start of the display row
(and the resulting 6-pixel read past the end of the raw buffer is filled with duplicated data in
`P3WinUsbCamera::getFrame()`); live view rotation is left at the camera's native orientation (rotate it
physically, or use *Rotate Live View CW/CCW* in the live view menu).

Shutter (NUC) calibration is supported: the *Calibrate* button sends the camera's own vendor command
(`P3WinUsbCamera::triggerShutterCalibration()`) rather than doing any NUC processing on this side - the
camera performs the calibration internally, the same as pressing the button in the vendor's app.
A freshly plugged-in camera can take several seconds before it answers any command at all (observed
up to ~9s in a real capture); both `startCapture()` and the shutter trigger retry with a few seconds
of patience rather than failing on the first attempt.

### Thermal Master THOR001 (Pool 6): a real UVC camera with a mislabeled format

Unlike P3, THOR001 *is* a standard UVC device — Windows binds its inbox `usbvideo.sys` driver to it, and it
shows up as a normal DirectShow capture device named `THOR001`. Its VideoStreaming interface declares a
single format tagged `MEDIASUBTYPE_H264` (640 x 480), but the bytes actually delivered over that pipe are
**not** real H.264 - they are raw 16 bit sensor values (confirmed by analysing a USB capture and then real
hardware: the payload is a constant, low-entropy byte pattern, not compressed video). Pool 6 still goes
through the normal DirectShow pipeline (`RMH_IRThermalCamera_OpenIRCameraDevice`'s Pool 6 case in
`RMH_ThermalCameraSupport_Library.cpp`), it just has to pick that H264-labelled format explicitly, since the
normal automatic format selection only accepts RGB/monochrome-convertible types.

Camera also streams a second, real, variable-size H.264 elementary stream (likely a separate day/visible
sensor) interleaved on the *same* pin. Because DirectShow's grabber only ever sees one pin/sample size at a
time, and the two streams' sizes never repeat back-to-back, the library's automatic sample-size detection in
`SampleGrabberCallback::SampleCB()` (`ds_grabber_callback.cpp`) never settles on either one. Pool 6 works
around this with `forceExpectedFrameBufferSize()` (`ds_camera.h`/`.cpp`), pinning the grabber to the raw
sample's known, constant size (98314 bytes) right after `startCapture()` - this has the side effect of making
the grabber silently ignore the interleaved H.264 samples, since they never match that fixed size.

The raw sample itself is a fixed 10 byte header (`ff 00` x5, a sync marker) followed directly by
256 x 192 raw 16 bit pixel values with no other padding - `FrameWidthPixelOffset` is set to 5 (pixels) to
skip it. There is no second (metadata/display) block like Pool 2/4's sensors, so unlike Pool 4/5,
`RMH_IRThermalCamera_ConvertYUY2To14BitThermalDataArray()`'s pool-2/4/5/6 branch computes the frame's
max/min/center pixel values by scanning it directly (Pool 2/4/5 also do this; there is no camera-embedded
metadata being read there either, despite the name "ReadCalFrameMetaData").

## Verification status

| Camera | Status |
|---|---|
| Thermal Master P2 Pro (USB `0BDA:5830`, shows as "USB Camera") | **works** — select *InfiRay Or Thermal Master P2Pro* |
| Thermal Master P3 (USB `3474:45A2`) | **works** — select *Thermal Master P3*. Not a DirectShow camera; see the Pool 5 section above. Requires the stock WinUSB driver from the vendor installer (`P3.inf`) to be bound — no driver changes needed beyond what the vendor's own installer already sets up. |
| Thermal Master THOR001 (USB `1D6B:1102`, composite device, shows as "THOR001") | **works** — select *Thermal Master THOR001*. A real UVC camera with a mislabeled format; see the Pool 6 section above. |
| all others | untested in this repository |

## If your camera is not found

1. Check the exact list entry (table above). The Thermal Master **P2** entry is a different model from the **P2 Pro**.
2. Confirm Windows sees it: Device Manager → *Cameras*. Note the name shown.
3. Close other programs that may hold the camera; check *Settings → Privacy → Camera → desktop apps* is on.
4. If the Windows name is not in the table, the model needs a device-name entry — open an issue with the name from step 2.
