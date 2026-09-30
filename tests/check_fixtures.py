# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Cyclotronic
"""Check the hardware captures in tests/fixtures/ against the reference decoder.

    python tests/check_fixtures.py              # check every fixture
    python tests/check_fixtures.py --update     # (re)write expected.json from the decoder; review the diff

Each fixture is one capture from one real camera, contributed by someone who owns it (docs/HARDWARE-VALIDATION.md):

    tests/fixtures/<camera>/<capture>/
        capture.json        who, which camera, how it was set up, what a reference thermometer read
        snapshot.png        the SnapShotRAW_*.png the program saved
        frame-temps.csv     optional: the program's own Full Frame Temperature CSV, exported after opening
                            snapshot.png in Snapshot Analysis mode - the program's answer for the same data
        expected.json       the decoder's summary, pinned (written by --update)

What is checked, per fixture:
  1. capture.json has the required fields and they agree with the file (pool, frame size).
  2. The snapshot is accepted by the program's loader rules, and decodes.
  3. The decode matches expected.json: raw pixels exactly (by hash), temperatures within 0.001 degrees.
     A change here means the decode of a real camera's data changed - intended or not, a reviewer decides.
  4. If frame-temps.csv is present: every pixel of the decode agrees with the program's own export.
     This is what ties the reference decoder to the real program.
  5. If reference readings are given: the decoded temperature at each spot is within the stated tolerance.
     This is the only check against physical truth; it says how accurate the camera + program are.

Standard library only, Python 3.9+.
"""
import argparse
import json
import math
import os
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "tests" / "fixtures"
sys.path.insert(0, str(ROOT / "tests" / "tools"))

import ircam_raw as raw  # noqa: E402

TEMPERATURE_TOLERANCE = 1e-3     # expected.json vs decode, degrees C
CSV_TOLERANCE = 2e-3             # program export (5 decimals) vs decode, in the export's unit
REQUIRED = {
    "contributor": str,
    "date": str,
    "camera": dict,
    "program": dict,
    "settings": dict,
}
REQUIRED_CAMERA = ("model", "list_entry", "windows_device_name", "usb_id")
REQUIRED_PROGRAM = ("version",)
REQUIRED_SETTINGS = ("temperature_range",)
UNITS = {"C": (1.0, 0.0), "F": (1.8, 32.0), "K": (1.0, 273.15)}


def display(path):
    try:
        return path.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


class Report:
    def __init__(self):
        self.errors = 0
        self.messages = []
        self.github = os.environ.get("GITHUB_ACTIONS") == "true"

    def error(self, fixture, message):
        self.errors += 1
        self.messages.append(("error", message))
        rel = display(fixture)
        print(f"::error file={rel}/capture.json::{message}" if self.github else f"ERROR: {rel}: {message}")

    def note(self, fixture, message):
        self.messages.append(("note", message))
        rel = display(fixture)
        print(f"::notice file={rel}/capture.json::{message}" if self.github else f"  {rel}: {message}")


def load_capture(fixture, report):
    path = fixture / "capture.json"
    try:
        capture = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError) as exc:
        report.error(fixture, f"capture.json unreadable: {exc}")
        return None
    ok = True
    for key, kind in REQUIRED.items():
        if not isinstance(capture.get(key), kind):
            report.error(fixture, f"capture.json: '{key}' missing or not a {kind.__name__}")
            ok = False
    if not ok:
        return None
    for section, keys in (("camera", REQUIRED_CAMERA), ("program", REQUIRED_PROGRAM), ("settings", REQUIRED_SETTINGS)):
        for key in keys:
            if capture[section].get(key) in (None, ""):
                report.error(fixture, f"capture.json: '{section}.{key}' is required")
                ok = False
    if capture["settings"].get("temperature_range") not in ("low", "high"):
        report.error(fixture, "capture.json: settings.temperature_range must be 'low' or 'high'")
        ok = False
    return capture if ok else None


def read_program_csv(path, width, height):
    """The program's Full Frame Temperature export: a header line, a blank line, then one row per image row.

    Values are written with the Windows locale, so a semicolon-delimited file may use a decimal comma.
    """
    lines = [l for l in path.read_text(encoding="utf-8", errors="replace").splitlines()[2:] if l.strip()]
    rows = []
    for line in lines:
        fields = line.split(";") if ";" in line else line.split(",")
        rows.append([float(f.replace(",", ".")) for f in fields])
    if len(rows) != height or any(len(r) != width for r in rows):
        raise ValueError(f"expected {height} rows of {width} values, found {len(rows)} rows"
                         + (f" of {len(rows[0])}" if rows else ""))
    return rows


def compare(expected, actual, path=""):
    """Differences between two summaries: exact for integers/strings, TEMPERATURE_TOLERANCE for floats."""
    diffs = []
    if isinstance(expected, dict) and isinstance(actual, dict):
        for key in sorted(set(expected) | set(actual)):
            if key not in actual or key not in expected:
                diffs.append(f"{path}{key}: {'missing' if key not in actual else 'unexpected'}")
            else:
                diffs += compare(expected[key], actual[key], f"{path}{key}.")
    elif isinstance(expected, list) and isinstance(actual, list) and len(expected) == len(actual):
        for i, (e, a) in enumerate(zip(expected, actual)):
            diffs += compare(e, a, f"{path}{i}.")
    elif isinstance(expected, float) or isinstance(actual, float):
        if expected is None or actual is None or not (
                (math.isnan(expected) and math.isnan(actual)) or abs(expected - actual) <= TEMPERATURE_TOLERANCE
                * max(1.0, abs(expected) / 100.0)):
            diffs.append(f"{path.rstrip('.')}: expected {expected}, got {actual}")
    elif expected != actual:
        diffs.append(f"{path.rstrip('.')}: expected {expected}, got {actual}")
    return diffs


def check_fixture(fixture, update, report):
    capture = load_capture(fixture, report)
    snapshot = fixture / "snapshot.png"
    if not snapshot.exists():
        report.error(fixture, "snapshot.png is missing")
        return
    if capture is None:
        return
    png = snapshot.read_bytes()

    width, height, buffer = raw.read_png_rgb(png)
    meta = raw.read_metadata_row(width, height, buffer)
    for warning in raw.metadata_warnings(meta):
        report.note(fixture, warning)

    range_flag = 2 if capture["settings"]["temperature_range"] == "high" else 1
    try:
        summary = raw.decode_snapshot(png, range_flag=range_flag)
    except raw.UnsupportedPool as exc:
        report.error(fixture, f"{exc}. A pull request that adds a pool also extends tests/tools/ircam_raw.py, "
                              "so that captures from it can be checked")
        return
    except raw.SnapshotError as exc:
        report.error(fixture, f"the program would refuse this snapshot: {exc}")
        return

    declared_pool = capture["camera"].get("pool")
    if declared_pool is not None and declared_pool != summary["pool"]:
        report.error(fixture, f"capture.json says pool {declared_pool}, the file says pool {summary['pool']}")
    if summary["image_temperature"]["non_finite_pixels"]:
        report.error(fixture, f"{summary['image_temperature']['non_finite_pixels']} pixels decode to NaN/inf")

    expected_path = fixture / "expected.json"
    if update:
        expected_path.write_text(json.dumps(summary, indent=1, sort_keys=True) + "\n", encoding="utf-8")
        report.note(fixture, "expected.json written")
    elif not expected_path.exists():
        report.error(fixture, "expected.json is missing (run tests/check_fixtures.py --update and commit it)")
    else:
        diffs = compare(json.loads(expected_path.read_text(encoding="utf-8")), summary)
        if diffs:
            shown = "; ".join(diffs[:8]) + (f"; ... {len(diffs) - 8} more" if len(diffs) > 8 else "")
            report.error(fixture, f"decode differs from expected.json ({len(diffs)} values): {shown}")

    csv_path = fixture / "frame-temps.csv"
    if csv_path.exists():
        check_program_csv(fixture, csv_path, capture, png, range_flag, summary, report)

    for reading in capture.get("reference_readings", []):
        check_reference(fixture, reading, png, range_flag, summary, report)


def pixel_temperatures(png, range_flag):
    """Per-pixel temperatures of the image area, in degrees C, as the decoder computes them."""
    width, height, buffer = raw.read_png_rgb(png)
    meta = raw.read_metadata_row(width, height, buffer)
    cam = raw.Camera(meta["pool"], width, height, meta["metadata_rows"], meta["width_offset"], meta["height_offset"])
    cam.range_flag = range_flag
    for key in cam.settings:
        cam.settings[key] = meta[key]
    raw.init_constants(cam)
    thermal, _ = raw.convert_to_thermal(cam, buffer)
    model = raw.TemperatureModel(cam, thermal, raw.read_frame_metadata(cam, thermal))
    w, h = cam.width, cam.height - cam.metadata_rows
    return [[model.temperature(thermal[y * w + x]) for x in range(w)] for y in range(h)]


def check_program_csv(fixture, csv_path, capture, png, range_flag, summary, report):
    unit = capture.get("program", {}).get("csv_unit", "C")
    if unit not in UNITS:
        report.error(fixture, f"program.csv_unit must be one of {sorted(UNITS)}")
        return
    scale, offset = UNITS[unit]
    w, h = summary["frame"]["image_width"], summary["frame"]["image_height"]
    try:
        program = read_program_csv(csv_path, w, h)
    except ValueError as exc:
        report.error(fixture, f"frame-temps.csv: {exc}")
        return
    decoded = pixel_temperatures(png, range_flag)
    worst, where, bad = 0.0, None, 0
    for y in range(h):
        for x in range(w):
            diff = abs(decoded[y][x] * scale + offset - program[y][x])
            if diff > CSV_TOLERANCE:
                bad += 1
            if diff > worst:
                worst, where = diff, (x, y)
    if bad:
        x, y = where
        report.error(fixture, f"reference decoder disagrees with the program's own export at {bad} of {w * h} pixels "
                              f"(worst {worst:.4f} {unit} at x={x} y={y}: program {program[y][x]}, decoder "
                              f"{decoded[y][x] * scale + offset:.5f}). One of the two is wrong - find out which")
    else:
        report.note(fixture, f"matches the program's export at all {w * h} pixels (worst {worst:.5f} {unit})")


def check_reference(fixture, reading, png, range_flag, summary, report):
    try:
        x, y = int(reading["x"]), int(reading["y"])
        truth, tolerance = float(reading["temperature_c"]), float(reading["tolerance_c"])
    except (KeyError, TypeError, ValueError):
        report.error(fixture, "each reference_readings entry needs x, y, temperature_c and tolerance_c")
        return
    w, h = summary["frame"]["image_width"], summary["frame"]["image_height"]
    if not (0 <= x < w and 0 <= y < h):
        report.error(fixture, f"reference reading at x={x} y={y} is outside the {w}x{h} image")
        return
    radius = int(reading.get("radius", 1))
    temps = pixel_temperatures(png, range_flag)
    spot = [temps[j][i] for j in range(max(0, y - radius), min(h, y + radius + 1))
            for i in range(max(0, x - radius), min(w, x + radius + 1))]
    value = sum(spot) / len(spot)
    label = reading.get("what", f"x={x} y={y}")
    if abs(value - truth) > tolerance:
        report.error(fixture, f"{label}: decoded {value:.2f} C, reference {truth:.2f} C, "
                              f"outside +/-{tolerance} C")
    else:
        report.note(fixture, f"{label}: decoded {value:.2f} C, reference {truth:.2f} C (within +/-{tolerance} C)")


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--update", action="store_true", help="write expected.json from the current decoder")
    parser.add_argument("fixtures", nargs="*", type=pathlib.Path, help="fixture folders (default: all)")
    args = parser.parse_args()

    report = Report()
    fixtures = [p.resolve() for p in args.fixtures] or sorted(p.parent for p in FIXTURES.glob("*/*/capture.json"))
    orphans = sorted(p.parent for p in FIXTURES.glob("*/*/snapshot.png") if not (p.parent / "capture.json").exists())
    for fixture in orphans:
        report.error(fixture, "snapshot.png without a capture.json")
    for fixture in fixtures:
        print(display(fixture))
        check_fixture(fixture, args.update, report)
    print(f"\n{len(fixtures)} fixture(s); {report.errors} error(s)")
    return 1 if report.errors else 0


if __name__ == "__main__":
    sys.exit(main())
