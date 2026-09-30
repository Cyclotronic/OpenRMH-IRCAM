# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Cyclotronic
"""Unit tests for the reference snapshot decoder.  python -m unittest discover -s tests/tools

These use synthetic snapshots built the way the program writes them, so they test the decoder, not a camera.
Real captures are checked by tests/check_fixtures.py.
"""
import math
import struct
import unittest

import ircam_raw as raw


def build_snapshot(width, height, pool, metadata_rows, pixels, height_offset=0, settings=None, fill=None):
    """A RAW snapshot PNG: `pixels` (16-bit values) at the pool's offset in the buffer, metadata row last."""
    s = dict(temperature_correction=0.0, ambient=25.0, reflected=25.0, humidity=0.45, emissivity=0.95, distance=1)
    s.update(settings or {})
    buffer = bytearray(width * height * 3)
    start = height_offset * width * 2
    for i, value in enumerate(pixels):
        struct.pack_into("<H", buffer, start + 2 * i, value)
    for index, value in (fill or {}).items():
        struct.pack_into("<H", buffer, start + 2 * index, value)
    row = raw.metadata_row_bytes(width, pool, metadata_rows, 0, height_offset, **s)
    buffer[(height - 1) * width * 3:] = row
    return raw.write_png_rgb(width, height, bytes(buffer))


class PngTests(unittest.TestCase):
    def test_round_trip(self):
        data = bytes(range(256)) * 3
        width, height, out = raw.read_png_rgb(raw.write_png_rgb(16, 16, data))
        self.assertEqual((width, height, out), (16, 16, data))

    def test_rejects_non_png(self):
        with self.assertRaises(raw.SnapshotError):
            raw.read_png_rgb(b"not a png")


class MetadataTests(unittest.TestCase):
    def test_round_trip(self):
        row = raw.metadata_row_bytes(256, 2, 192, 0, 192, 1.5, 22.0, 23.0, 0.5, 0.9, 3)
        meta = raw.read_metadata_row(256, 1, row)
        self.assertEqual(meta["id"], raw.ID_STRING)
        self.assertEqual((meta["pool"], meta["metadata_rows"], meta["height_offset"], meta["distance"]), (2, 192, 192, 3))
        self.assertAlmostEqual(meta["emissivity"], 0.9, places=6)
        self.assertEqual(raw.metadata_problems(meta, 256, 384), [])

    def test_pool_outside_range_is_refused(self):
        meta = raw.read_metadata_row(256, 1, raw.metadata_row_bytes(256, 5, 2, 0, 0, 0, 25, 25, 0.45, 0.95, 1))
        self.assertTrue(any("pool 5" in p for p in raw.metadata_problems(meta, 256, 194)))

    def test_bang_byte_in_a_setting_truncates_like_the_program(self):
        # 0x21 inside the ambient float stops the C++ parse; every later field keeps its default of 0
        ambient = struct.unpack(">f", b"\x41\x21\x00\x00")[0]
        meta = raw.read_metadata_row(256, 1, raw.metadata_row_bytes(256, 2, 192, 0, 192, 0, ambient, 25, 0.45, 0.95, 1))
        self.assertEqual(meta["end_marker_at"], raw._IDX_AMBIENT + 1)
        self.assertEqual(meta["emissivity"], 0.0)
        self.assertTrue(raw.metadata_warnings(meta))


class Pool2Tests(unittest.TestCase):
    """Pool 2 (P2 Pro and friends): 256 x 384 frame, image in the first 192 rows after a 192-row offset."""

    def setUp(self):
        self.w, self.h, self.meta_rows, self.offset = 256, 384, 192, 192
        self.image = [int((30.0 + 273.15) / 0.015625)] * (self.w * self.meta_rows)

    def decode(self, **settings):
        png = build_snapshot(self.w, self.h, 2, self.meta_rows, self.image, self.offset, settings)
        return raw.decode_snapshot(png)

    def test_max_min_center_found(self):
        hot = 10 * self.w + 20
        cold = 100 * self.w + 200
        self.image[hot] = int((80.0 + 273.15) / 0.015625)
        self.image[cold] = int((5.0 + 273.15) / 0.015625)
        s = self.decode()
        self.assertEqual((s["maximum"]["x"], s["maximum"]["y"]), (20, 10))
        self.assertEqual((s["minimum"]["x"], s["minimum"]["y"]), (200, 100))
        self.assertEqual(s["center"]["raw"], self.image[96 * self.w + 128])

    def test_emissivity_one_short_distance_gives_raw_kelvin(self):
        # With emissivity 1 and tau ~1 the environment terms vanish apart from the shutter/core term, which is 0
        # because pool 2 reports shutter == core == 1
        s = self.decode(emissivity=1.0, distance=0, humidity=0.0)
        self.assertAlmostEqual(s["center"]["temperature"], self.image[0] * 0.015625 - 273.15, places=3)

    def test_summary_shape(self):
        s = self.decode()
        self.assertEqual(s["frame"]["image_height"], 192)
        self.assertEqual(len(s["grid"]), raw.GRID * raw.GRID)
        self.assertEqual(s["image_temperature"]["non_finite_pixels"], 0)


class Pool1Tests(unittest.TestCase):
    """Pool 1 (InfiRay T2/T3 family): calibration floats and sensor temperatures live in the metadata rows."""

    W, H = 256, 196          # 256 x 192 image + 4 metadata rows

    def build(self):
        w, h = self.W, self.H
        meta_rows = h - round(w / 1.33333333)
        self.assertEqual(meta_rows, 4)
        image = [7000] * (w * (h - meta_rows))
        meta1 = w * (h - meta_rows) - 1
        meta2 = meta1 + w
        fill = {meta1 + 2: 8617,                              # detector raw -> fpa = 20 degC
                meta2 + 2: int((30.0 + 273.15) * 10),         # shutter raw -> 30 degC
                meta2 + 3: int((35.0 + 273.15) * 10),         # core raw
                meta2 + 1: 7000}                              # CalValue0
        cal = [1.0e-3, 2.0, 0.0, 0.0, 1.0]
        for k, value in enumerate(cal, start=1):
            hi, lo = struct.unpack(">HH", struct.pack(">f", value))
            fill[meta2 + 1 + 2 * k + 2] = hi
            fill[meta2 + 2 + 2 * k + 2] = lo
        fill.update({meta1 + 3: 5, meta1 + 4: 6, meta1 + 5: 7100, meta1 + 6: 7, meta1 + 7: 8, meta1 + 8: 6900,
                     meta1 + 13: 7000})
        return build_snapshot(w, h, 1, meta_rows, image, fill=fill), cal

    def test_sensor_and_calibration_read(self):
        png, cal = self.build()
        s = raw.decode_snapshot(png)
        self.assertAlmostEqual(s["sensor"]["fpa"], 20.0, places=6)
        self.assertAlmostEqual(s["sensor"]["shutter"], 30.0, places=1)
        self.assertEqual(s["calibration"][0], 7000.0)
        for got, want in zip(s["calibration"][1:], cal):
            self.assertAlmostEqual(got, want, places=6)
        self.assertEqual((s["maximum"]["x"], s["maximum"]["y"], s["maximum"]["raw"]), (5, 6, 7100))

    def test_lut_is_monotonic_around_the_operating_point(self):
        png, _ = self.build()
        s = raw.decode_snapshot(png)
        self.assertLess(s["minimum"]["temperature"], s["center"]["temperature"])
        self.assertLess(s["center"]["temperature"], s["maximum"]["temperature"])
        self.assertTrue(all(math.isfinite(s[k]["temperature"]) for k in ("maximum", "minimum", "center")))

    def test_unknown_sensor_width_is_an_error_not_garbage(self):
        png = build_snapshot(200, 154, 1, 4, [7000] * (200 * 150))
        with self.assertRaises(raw.SnapshotError):
            raw.decode_snapshot(png)


if __name__ == "__main__":
    unittest.main()
