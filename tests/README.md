# Tests

Everything here runs on any machine with Python 3.9+ and no extra packages. None of it is wired into CI yet; run it
by hand.

```
python tests/static/check_source.py --base origin/main   # source consistency
python -m unittest discover -s tests/tools               # reference decoder unit tests
python tests/check_fixtures.py                           # replay hardware captures
```

| Path | What |
|---|---|
| `static/check_source.py` | Camera table is consistent; every `switch` on a camera pool or camera index covers every pool/camera; the snapshot loader accepts every pool; every `.cpp` is in the project; no binaries; and, against `--base`: no encoding damage (U+FFFD), no Windows-1252 → UTF-8 conversions, SPDX headers on new files, size limits. |
| `tools/ircam_raw.py` | Reference decoder for RAW snapshots, written from the C++ decode path (pools 1–4). |
| `tools/test_*.py` | Unit tests for the decoder and the fixture checker, on synthetic snapshots. |
| `check_fixtures.py` | Decodes every capture in `fixtures/` and compares with its pinned `expected.json`, the program's own CSV export, and any reference-thermometer readings. |
| `fixtures/` | Captures from real cameras. How to make one: [`docs/HARDWARE-VALIDATION.md`](../docs/HARDWARE-VALIDATION.md). |

A `switch` that deliberately has no case for a pool says so in a comment, which `check_source.py` reads:

```cpp
// pool-coverage: skip 5 - the P3 has no DirectShow device to open; Pool 5 opens through P3WinUsbCamera
switch (SupportedCameraPool) {
```

The same form with `camera-coverage:` and camera names covers a switch on the camera index.
