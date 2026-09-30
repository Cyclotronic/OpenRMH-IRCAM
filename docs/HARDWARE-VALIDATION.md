# Hardware validation: what to capture from a real camera

> **Status: draft.** The tools under `tests/` work and can be run by hand, but they are not yet part of CI or the
> pull request process. Nothing here is a requirement yet.

Nobody can keep one of every camera. What we can do is keep data that each camera produced and replay it, so that a
change which alters how a camera's frames are decoded shows up even when nobody with that camera is around. This page
says what to capture so that your camera becomes one of those replayable test cases.

If you own a supported camera, a capture is one of the most useful things you can contribute: most models in
[`SUPPORTED-CAMERAS.md`](SUPPORTED-CAMERAS.md) have never been checked in this repository.

## What gets captured, and why

| File | What it is | What the checker does with it |
|---|---|---|
| `snapshot.png` | A RAW snapshot (`SnapShotRAW_....png`) saved by the program | Decoded and compared with the pinned result. The file holds the camera's raw frame, the sensor/calibration metadata and your settings, so it replays the whole decode for that camera's pool. |
| `frame-temps.csv` | The program's *Full Frame Temperature Data* export of that same snapshot | Compared pixel by pixel with the reference decoder. This is what ties the decoder to the real program. |
| `capture.json` | Who, which camera, how it was set up, and any reference-thermometer readings | Checked for completeness; reference readings are checked against the decoded temperatures. |
| `expected.json` | The decoded summary, pinned | Written by `tests/check_fixtures.py --update`. You don't write this by hand. |

A snapshot is small: a PNG of a few hundred KB for a 256 × 192 camera.

## How to capture

1. **Set up a scene with a known temperature in it**, if you can: a mug of hot water with a thermocouple or a kitchen
   thermometer in it, a matt-black tape patch on something warm, a heater block at a set point. Something that reads
   clearly different from the background. A capture without a reference is still useful (it pins the decode); one with
   a reference also says whether the temperatures are *right*.
2. Connect the camera, pick your model from the list, and let it run for **at least 5 minutes** so the sensor settles.
   Press *Calibrate* once just before capturing.
3. Leave the camera settings (emissivity, distance, ambient, reflected, humidity) at their defaults unless you are
   deliberately testing one. Note them either way; they are also stored in the snapshot.
4. Enable *Save RAW Snapshot* (Live View Tools → snapshot settings) and take a snapshot. Two files appear:
   `SnapShot_....png` (the picture) and `SnapShotRAW_....png` (the raw data). **Use the RAW one.**
5. Write down the reference temperature, and roughly where the reference object is in the image (the mouse-cursor
   temperature readout gives you pixel coordinates).
6. **Close the camera**, choose *Snapshot Analysis Mode*, open the `SnapShotRAW_....png` you just saved, start the live
   view, and use *Save Full Frame Temperature Data*. Keep the `FrameTempData_....txt` it writes. Don't change any
   setting between opening the snapshot and exporting. Leave the unit on °C, or say which unit you used.
7. If your camera has a high range, repeat with the high range selected; that is a separate capture.

## capture.json

```json
{
  "contributor": "your GitHub handle",
  "date": "2026-09-29",
  "camera": {
    "model": "Thermal Master P2 Pro",
    "list_entry": "InfiRay Or Thermal Master P2Pro",
    "windows_device_name": "USB Camera",
    "usb_id": "0BDA:5830",
    "pool": 2,
    "firmware": "if the vendor app shows it",
    "serial_or_note": "optional; anything that distinguishes your unit"
  },
  "program": {
    "version": "v3.0.0-community.4, or the commit you built",
    "csv_unit": "C"
  },
  "settings": {
    "temperature_range": "low",
    "warm_up_minutes": 10,
    "calibrated_before_capture": true
  },
  "scene": "mug of water at ~60 C on a desk, room ~22 C, camera ~40 cm away",
  "reference_readings": [
    {
      "what": "water surface, K-type thermocouple on a Fluke 87V",
      "x": 128, "y": 96, "radius": 2,
      "temperature_c": 58.4,
      "tolerance_c": 3.0
    }
  ]
}
```

- `usb_id`: *Device Manager → Cameras → your camera → Properties → Details → Hardware Ids* (`VID_xxxx&PID_yyyy`).
- `windows_device_name`: the name shown under *Cameras* in Device Manager.
- `reference_readings[].x/y`: image pixel coordinates (0,0 top left). `radius` averages a (2r+1)² square around it.
- `tolerance_c`: what you'd accept for that reading — the camera's spec (often ±2 °C or ±2 %) plus your reference's
  uncertainty. Be honest; a generous tolerance that passes says less than a tight one that fails.
- Leave `reference_readings` out if you had no reference.

## Where the files go

```
tests/fixtures/<camera-slug>/<capture-slug>/
    capture.json
    snapshot.png          (the SnapShotRAW_....png, renamed)
    frame-temps.csv       (the FrameTempData_....txt, renamed)
    expected.json         (generated)
```

For example `tests/fixtures/thermal-master-p2pro/2026-09-29-mug-low-range/`. Then run

```
python tests/check_fixtures.py --update tests/fixtures/thermal-master-p2pro/2026-09-29-mug-low-range
python tests/check_fixtures.py
```

and commit all four files. If you can't run Python, attach the files to your pull request or an issue and a maintainer
will add them.

## Privacy

The snapshot is a thermal image of whatever the camera was pointed at. Point it at a test object, not at people or
anything you'd rather not publish; captures are committed to a public repository and stay in its history.

## What the checks catch, and what they don't

Caught without hardware:

- a change to frame conversion, metadata reading, calibration parsing, the look-up table or the temperature formula
  that changes the result for a real camera's data (`expected.json` differs);
- a snapshot the loader would now refuse (for example a pool limit that wasn't raised with a new pool);
- a pool or camera added without every `switch` that needs it (`tests/static/check_source.py`).

Not caught — still needs someone with the camera:

- connecting: device name matching, DirectShow/WinUSB open, video format selection;
- vendor commands: shutter calibration, range switching, writing settings to the camera;
- anything timing- or driver-related, and the live GUI.

So a pull request for a new camera should also say which of those were exercised on the real camera.

## When `expected.json` changes

A change that alters the decode for existing captures fails `check_fixtures.py` until `expected.json` is
regenerated. That is deliberate: the reviewer then sees exactly which cameras' results moved and by how much. A bug fix that moves results is
fine — say so in the description, regenerate with `--update`, and let the diff show the effect. A refactor that moves
results is a bug.

The reference decoder (`tests/tools/ircam_raw.py`) is a second implementation of the program's decode, written from
the C++. When `frame-temps.csv` and the decoder disagree, one of them is wrong; find out which before changing either.
Captures from a pool the decoder does not implement (currently anything above 4) cannot be checked until the decoder
is extended to cover it.
