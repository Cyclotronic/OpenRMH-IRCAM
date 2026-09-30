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
| `static/check_source.py` | Camera table is consistent; every pool is used by a camera, listed in the docs, and does something no other pool does (see below); every `switch` on a camera pool or camera index covers every pool/camera; the snapshot loader accepts every pool; every `.cpp` is in the project; no binaries; and, against `--base`: no encoding damage (U+FFFD), no Windows-1252 → UTF-8 conversions, SPDX headers on new files, size limits. |
| `tools/ircam_raw.py` | Reference decoder for RAW snapshots, written from the C++ decode path (pools 1–4). |
| `tools/test_*.py` | Unit tests for the decoder and the fixture checker, on synthetic snapshots. |
| `check_fixtures.py` | Decodes every capture in `fixtures/` and compares with its pinned `expected.json`, the program's own CSV export, and any reference-thermometer readings. |
| `fixtures/` | Captures from real cameras. How to make one: [`docs/HARDWARE-VALIDATION.md`](../docs/HARDWARE-VALIDATION.md). |

A `switch` that deliberately has no case for a pool says so in a comment, which `check_source.py` reads:

```cpp
// pool-coverage: skip 5 - pool 5 cameras are opened by their own backend before this switch is reached
switch (SupportedCameraPool) {
```

The same form with `camera-coverage:` and camera names covers a switch on the camera index.

## Pool uniqueness

A pool is justified only by what the program does differently for it. `check_source.py` finds every place the program
branches on the pool (a `switch` on it, or an `if` that compares it), normalises the code each pool gets there
(comments and whitespace dropped, constant macros replaced by their values, per-pool `...Pool<n>()` functions replaced by
a fingerprint of their bodies), and compares pools place by place. A pool identical to another at every place is an
error: map its cameras to the existing pool. For every pool, and for each pool that is new since `--base`, it prints
the nearest other pool and where they differ, so a reviewer can see whether a small difference would be better as a
setting inside an existing pool. Places where a pool has no case at all are integration gaps and are reported
separately; they don't count as differences.
