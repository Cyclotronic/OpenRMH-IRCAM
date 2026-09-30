# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Cyclotronic
"""Tests for tests/check_fixtures.py, on a synthetic fixture in a temporary folder."""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))

import check_fixtures as cf  # noqa: E402
from test_ircam_raw import build_snapshot  # noqa: E402

CAPTURE = {
    "contributor": "test",
    "date": "2026-09-29",
    "camera": {"model": "Synthetic", "list_entry": "InfiRay Or Thermal Master P2Pro", "windows_device_name": "USB Camera",
               "usb_id": "0000:0000", "pool": 2},
    "program": {"version": "test", "csv_unit": "C"},
    "settings": {"temperature_range": "low"},
}


class CheckFixturesTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.fixture = pathlib.Path(self.tmp.name) / "synthetic" / "capture-1"
        self.fixture.mkdir(parents=True)
        image = [int((30.0 + 273.15) / 0.015625)] * (256 * 192)
        image[96 * 256 + 128] = int((60.0 + 273.15) / 0.015625)
        (self.fixture / "snapshot.png").write_bytes(build_snapshot(256, 384, 2, 192, image, 192))
        self.write_capture(CAPTURE)

    def tearDown(self):
        self.tmp.cleanup()

    def write_capture(self, capture):
        (self.fixture / "capture.json").write_text(json.dumps(capture), encoding="utf-8")

    def run_check(self, update=False):
        report = cf.Report()
        cf.check_fixture(self.fixture, update, report)
        return report

    def test_update_then_check_passes(self):
        self.assertEqual(self.run_check(update=True).errors, 0)
        self.assertEqual(self.run_check().errors, 0)

    def test_missing_expected_is_an_error(self):
        self.assertEqual(self.run_check().errors, 1)

    def test_changed_decode_is_caught(self):
        self.run_check(update=True)
        expected = json.loads((self.fixture / "expected.json").read_text())
        expected["center"]["temperature"] += 0.5
        (self.fixture / "expected.json").write_text(json.dumps(expected))
        report = self.run_check()
        self.assertEqual(report.errors, 1)
        self.assertIn("center.temperature", report.messages[-1][1])

    def test_wrong_declared_pool(self):
        capture = json.loads(json.dumps(CAPTURE))
        capture["camera"]["pool"] = 4
        self.write_capture(capture)
        self.run_check(update=True)
        self.assertTrue(any("says pool 4" in m for _, m in self.run_check().messages))

    def test_program_csv_agreement_and_disagreement(self):
        self.run_check(update=True)
        temps = cf.pixel_temperatures((self.fixture / "snapshot.png").read_bytes(), 1)
        lines = ["Time And Data For Captured Data: 12:00:00.000 29-09-2026", ""]
        lines += [",".join(f"{t:.5f}" for t in row) for row in temps]
        (self.fixture / "frame-temps.csv").write_text("\n".join(lines) + "\n")
        self.assertEqual(self.run_check().errors, 0)
        lines[2 + 10] = ",".join(["99.00000"] * 256)
        (self.fixture / "frame-temps.csv").write_text("\n".join(lines) + "\n")
        self.assertEqual(self.run_check().errors, 1)

    def test_reference_reading(self):
        capture = json.loads(json.dumps(CAPTURE))
        capture["reference_readings"] = [{"what": "hot spot", "x": 128, "y": 96, "radius": 0,
                                          "temperature_c": 60.0, "tolerance_c": 2.0}]
        self.write_capture(capture)
        self.run_check(update=True)
        self.assertEqual(self.run_check().errors, 0)
        capture["reference_readings"][0]["temperature_c"] = 70.0
        self.write_capture(capture)
        self.assertEqual(self.run_check().errors, 1)

    def test_missing_required_field(self):
        capture = json.loads(json.dumps(CAPTURE))
        del capture["camera"]["usb_id"]
        self.write_capture(capture)
        self.assertEqual(self.run_check(update=True).errors, 1)


if __name__ == "__main__":
    unittest.main()
