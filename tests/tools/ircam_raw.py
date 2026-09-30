# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Cyclotronic
"""Reference decoder for IRCAM RAW snapshot files (SnapShotRAW_*.png).

A RAW snapshot is the camera's raw frame buffer stored byte-for-byte in a 24-bit RGB PNG: the PNG's RGB byte stream is
exactly the buffer the program holds (YUY2 samples, two bytes per sensor pixel), and the last pixel row carries an
identification/metadata row beginning "IRCAM.RAM". Opening one in Snapshot Analysis mode runs the same decode as a live
frame, so a snapshot captured on real hardware is a replayable test vector for that camera's pool.

This module re-implements that decode path from the C++ so that snapshots can be checked on any machine, without a
Windows build. It follows the C++ step by step, including its quirks; each function names the routine it mirrors:

    RMH_AnalysisMode_ReadRAWMetaData            -> read_metadata_row
    RMH_AnalysisMode_IsRAWMetaDataValid         -> metadata_problems
    RMH_IRThermalCamera_InitIRCameraConstants   -> init_constants
    RMH_IRThermalCamera_ConvertYUY2To14Bit...   -> convert_to_thermal
    RMH_IRThermalCamera_ReadCalFrameMetaData    -> read_frame_metadata
    RMH_IRThermalCamera_ReadCalibrationParameters, _GenerateThermoGrapicLookUpTable, _ReadPixelTemperature
                                                -> TemperatureModel

It is a second implementation, so a disagreement with the program means one of the two is wrong - find out which
before changing either. Pools 1-4 are implemented; a snapshot from another pool raises UnsupportedPool.

Standard library only, Python 3.9+.
"""
import hashlib
import math
import struct
import zlib

# The writer and the loader both use only the first 8 characters of "IRCAM.RAM" (loop bound
# _RAWRecordingFileMetaDataIndex_IDStringStop = 8); byte 8 of the row is whatever was in the frame buffer.
ID_STRING = b"IRCAM.RA"
POOLS = (1, 2, 3, 4)

# Byte offsets inside the metadata row (RMH_AnalysisMode_Routines.h)
_IDX_POOL = 9
_IDX_META_SIZE = 10
_IDX_WIDTH_OFFSET = 12
_IDX_HEIGHT_OFFSET = 14
_IDX_TEMP_CORRECTION = 16
_IDX_AMBIENT = 20
_IDX_REFLECTED = 24
_IDX_HUMIDITY = 28
_IDX_EMISSIVITY = 32
_IDX_DISTANCE = 36
_IDX_END = 38

# RMH_FrameBufferLimits.h
MAXIMUM_FRAME_DATA_ARRAY_SIZE = 1600 * 1200 * 3
_MAXIMUM_FRAME_PIXELS = MAXIMUM_FRAME_DATA_ARRAY_SIZE // 12
_MAXIMUM_FRAME_DIMENSION = 4096

_RESOLUTION_14BIT = 16383


class SnapshotError(ValueError):
    """The file is not a RAW snapshot the program would accept."""


class UnsupportedPool(SnapshotError):
    """The snapshot is from a pool this reference decoder does not implement."""


def f32(x):
    """Round a Python float to the nearest single-precision value (C++ float arithmetic)."""
    return struct.unpack("<f", struct.pack("<f", x))[0]


# ---------------------------------------------------------------------------------------------------------------------
# PNG (8-bit RGB, non-interlaced: what System::Drawing::Bitmap::Save writes for Format24bppRgb)
# ---------------------------------------------------------------------------------------------------------------------

_PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"


def _paeth(a, b, c):
    p = a + b - c
    pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
    if pa <= pb and pa <= pc:
        return a
    return b if pb <= pc else c


def read_png_rgb(data):
    """Return (width, height, bytes) with the RGB byte stream of an 8-bit RGB PNG."""
    if not data.startswith(_PNG_SIGNATURE):
        raise SnapshotError("not a PNG file")
    pos, header, idat = 8, None, []
    while pos + 8 <= len(data):
        length, kind = struct.unpack(">I4s", data[pos:pos + 8])
        body = data[pos + 8:pos + 8 + length]
        if len(body) != length:
            raise SnapshotError("PNG truncated")
        if kind == b"IHDR":
            header = struct.unpack(">IIBBBBB", body)
        elif kind == b"IDAT":
            idat.append(body)
        elif kind == b"IEND":
            break
        pos += 12 + length
    if header is None:
        raise SnapshotError("PNG has no IHDR")
    width, height, depth, colour, _, _, interlace = header
    if depth != 8 or colour != 2:
        # The program refuses anything but 24-bit RGB (GUID_WICPixelFormat24bppBGR)
        raise SnapshotError(f"PNG is not 24-bit RGB (bit depth {depth}, colour type {colour})")
    if interlace:
        raise SnapshotError("interlaced PNG is not supported")
    raw = zlib.decompress(b"".join(idat))
    stride = width * 3
    if len(raw) != height * (stride + 1):
        raise SnapshotError("PNG image data has the wrong length")
    out = bytearray(height * stride)
    previous = bytearray(stride)
    for y in range(height):
        kind = raw[y * (stride + 1)]
        line = bytearray(raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        if kind == 1:
            for x in range(3, stride):
                line[x] = (line[x] + line[x - 3]) & 0xFF
        elif kind == 2:
            for x in range(stride):
                line[x] = (line[x] + previous[x]) & 0xFF
        elif kind == 3:
            for x in range(stride):
                left = line[x - 3] if x >= 3 else 0
                line[x] = (line[x] + ((left + previous[x]) >> 1)) & 0xFF
        elif kind == 4:
            for x in range(stride):
                left = line[x - 3] if x >= 3 else 0
                upper_left = previous[x - 3] if x >= 3 else 0
                line[x] = (line[x] + _paeth(left, previous[x], upper_left)) & 0xFF
        elif kind != 0:
            raise SnapshotError(f"PNG uses unknown filter type {kind}")
        out[y * stride:(y + 1) * stride] = line
        previous = line
    return width, height, bytes(out)


def write_png_rgb(width, height, rgb):
    """Encode an RGB byte stream as an 8-bit RGB PNG (used by the tests to build synthetic snapshots)."""
    stride = width * 3
    if len(rgb) != height * stride:
        raise ValueError("RGB data has the wrong length")
    raw = b"".join(b"\x00" + rgb[y * stride:(y + 1) * stride] for y in range(height))

    def chunk(kind, body):
        return struct.pack(">I", len(body)) + kind + body + struct.pack(">I", zlib.crc32(kind + body) & 0xFFFFFFFF)

    return (_PNG_SIGNATURE + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b""))


# ---------------------------------------------------------------------------------------------------------------------
# Metadata row
# ---------------------------------------------------------------------------------------------------------------------

def frame_size_supported(width, height):
    """RMH_FrameBuffer_IsFrameSizeSupported"""
    if width == 0 or height == 0 or width > _MAXIMUM_FRAME_DIMENSION or height > _MAXIMUM_FRAME_DIMENSION:
        return False
    return width * height <= _MAXIMUM_FRAME_PIXELS


def read_metadata_row(width, height, buffer):
    """RMH_AnalysisMode_ReadRAWMetaData: parse the identification row (the last pixel row of the file)."""
    row = buffer[(height - 1) * width * 3:height * width * 3]
    if len(row) < _IDX_END + 1:
        raise SnapshotError("frame is too narrow to hold the metadata row")

    def u16(i):
        return (row[i] << 8) | row[i + 1]

    def flt(i):
        return struct.unpack(">f", bytes(row[i:i + 4]))[0]

    # The C++ loop reads each field when it reaches the field's first byte, and stops at the first '!' (0x21) byte
    # anywhere in the row - including inside a float or the pool byte. Fields starting after that byte keep their
    # default of 0. Reproduce that, and report where it stopped.
    end = row.find(b"!")
    stop = end if end >= 0 else len(row)
    fields = (("pool", _IDX_POOL, lambda i: row[i]), ("metadata_rows", _IDX_META_SIZE, u16),
              ("width_offset", _IDX_WIDTH_OFFSET, u16), ("height_offset", _IDX_HEIGHT_OFFSET, u16),
              ("temperature_correction", _IDX_TEMP_CORRECTION, flt), ("ambient", _IDX_AMBIENT, flt),
              ("reflected", _IDX_REFLECTED, flt), ("humidity", _IDX_HUMIDITY, flt),
              ("emissivity", _IDX_EMISSIVITY, flt), ("distance", _IDX_DISTANCE, u16))
    meta = {"id": bytes(row[:len(ID_STRING)]), "end_marker_at": end}
    for name, index, read in fields:
        meta[name] = read(index) if index <= stop else (0.0 if read is flt else 0)
    return meta


def metadata_row_bytes(width, pool, metadata_rows, width_offset, height_offset, temperature_correction, ambient,
                       reflected, humidity, emissivity, distance):
    """RMH_AnalysisMode_AddIDAndMetaDataToFrameArray: the identification row as the program writes it."""
    row = bytearray(width * 3)
    row[:len(ID_STRING)] = ID_STRING
    row[_IDX_POOL] = pool
    struct.pack_into(">HHH", row, _IDX_META_SIZE, metadata_rows, width_offset, height_offset)
    struct.pack_into(">fffff", row, _IDX_TEMP_CORRECTION, temperature_correction, ambient, reflected, humidity, emissivity)
    struct.pack_into(">H", row, _IDX_DISTANCE, int(distance) & 0xFFFF)
    row[_IDX_END] = ord("!")
    return bytes(row)


def metadata_problems(meta, width, height):
    """RMH_AnalysisMode_IsRAWMetaDataValid, plus the ID-string check the loader does first. Empty list = accepted."""
    problems = []
    if meta["id"] != ID_STRING:
        problems.append("identification string is not IRCAM.RAM (not a RAW snapshot)")
        return problems
    if not frame_size_supported(width, height):
        problems.append(f"frame {width}x{height} does not fit the frame buffers")
    if not 1 <= meta["pool"] <= 4:
        problems.append(f"camera pool {meta['pool']} is outside 1-4 (the loader refuses it)")
    if meta["metadata_rows"] >= height:
        problems.append("metadata rows >= frame height")
    if meta["width_offset"] >= width:
        problems.append("width pixel offset outside the frame")
    if meta["height_offset"] >= height:
        problems.append("height pixel offset outside the frame")
    return problems


def metadata_warnings(meta):
    """Things the program accepts but that make the decoded result differ from what was captured."""
    warnings = []
    if 0 <= meta["end_marker_at"] < _IDX_END:
        warnings.append(f"a 0x21 ('!') byte at offset {meta['end_marker_at']} of the metadata row ends it early: the "
                        "program reads every later setting as 0 (a setting whose float encoding contains 0x21 "
                        "triggers this)")
    return warnings


# ---------------------------------------------------------------------------------------------------------------------
# Frame decode
# ---------------------------------------------------------------------------------------------------------------------

class Camera:
    """The fields of ThermalCameraDevice::IRCameraDeviceFormat that the decode path uses."""

    def __init__(self, pool, width, height, metadata_rows, width_offset=0, height_offset=0):
        self.pool = pool
        self.width = width
        self.height = height
        self.metadata_rows = metadata_rows
        self.width_offset = width_offset
        self.height_offset = height_offset
        self.fpa_off = self.fpa_div = self.cal0_offset = self.cal0_fpamul = 0.0
        self.meta1 = self.meta2 = self.meta3 = 0
        self.range_flag = 1          # CurrentIRTempRangeFlag: 1 low range (the default), 2 high range
        self.settings = dict(temperature_correction=0.0, ambient=0.0, reflected=0.0, humidity=0.0, emissivity=0.0,
                             distance=0)


def init_constants(cam):
    """RMH_IRThermalCamera_InitIRCameraConstants"""
    if cam.pool not in POOLS:
        raise UnsupportedPool(f"pool {cam.pool} is not implemented by the reference decoder")
    minus_meta = cam.height - cam.metadata_rows
    if cam.pool in (1, 3):
        cam.cal0_offset, cam.cal0_fpamul = 390.0, 7.05
        cam.meta1 = cam.width * minus_meta
        constants = {  # width: (fpa_off, fpa_div, rows between metadata 1 and 2)
            640: (6867, 33.8, 3), 384: (7800, 36.0, 3), 256: (8617, 37.682 if cam.pool == 1 else 13.9, 1),
            240: (7800, 36.0, 1)}
        if cam.width not in constants:
            # The C++ leaves MetaData2Index at 0 and then subtracts 1 (unsigned wrap): garbage, not a result
            raise SnapshotError(f"pool {cam.pool} has no constants for a {cam.width}-pixel-wide sensor")
        cam.fpa_off, cam.fpa_div, rows = constants[cam.width]
        cam.meta2 = cam.meta1 + cam.width * rows
        if cam.pool == 1 and cam.width == 256:
            cam.cal0_offset, cam.cal0_fpamul = 170.0, 0.0
        cam.meta1 -= 1
        cam.meta2 -= 1
        if cam.pool == 3:
            cam.meta3 = cam.width * minus_meta
    elif cam.pool == 2:
        cam.meta1 = cam.width * minus_meta
    elif cam.pool == 4:
        cam.meta1 = cam.width * minus_meta
        cam.meta2 = cam.meta1 + cam.width


def convert_to_thermal(cam, buffer):
    """RMH_IRThermalCamera_ConvertYUY2To14BitThermalDataArray -> (thermal list, average raw value).

    The pool 3 non-uniformity map is taken as all zeros, which is its state when no camera has been calibrated in the
    session (Snapshot Analysis straight after start-up). Bytes read past the end of the file read as 0, as the C++
    reads the rest of a zeroed global buffer.
    """
    w, h = cam.width, cam.height
    if not frame_size_supported(w, h) or cam.metadata_rows >= h:
        raise SnapshotError("frame geometry refused by the converter")
    start = (cam.height_offset * w + cam.width_offset) * 2
    count = w * h
    data = bytes(buffer[start:start + count * 2])
    data += bytes(count * 2 - len(data))
    thermal = list(struct.unpack(f"<{count}H", data))

    # The C++ works in groups of four pixels and tests only the first index of each group against a limit, so a
    # limit that is not a multiple of four takes in the whole last group
    size_minus_meta = w * (h - cam.metadata_rows)
    groups_end = min(count, -(-size_minus_meta // 4) * 4)
    if cam.pool == 3:
        for i in range(groups_end):
            if thermal[i] > _RESOLUTION_14BIT:
                thermal[i] = _RESOLUTION_14BIT
    total = sum(thermal[:groups_end])

    if cam.pool in (2, 3, 4):
        center = int(((h - cam.metadata_rows) * 0.5) * w + (w * 0.5))
        max_v, min_v, max_xy, min_xy = 0, 65535, (0, 0), (0, 0)
        x = y = 0
        for i in range(groups_end):
            value = thermal[i]
            if value > max_v:
                max_v, max_xy = value, (x, y)
            if value < min_v:
                min_v, min_xy = value, (x, y)
            x += 1
            if x >= w:
                x, y = 0, y + 1
                if y >= h:
                    y = 0
        # Appended after the image: [Max_X Max_Y Max_Raw Min_X Min_Y Min_Raw Center_Raw], written in that order
        base = size_minus_meta
        for k, value in enumerate((max_xy[0], max_xy[1], max_v, min_xy[0], min_xy[1], min_v)):
            thermal[base + k] = value
        thermal[base + 6] = thermal[center]
    return thermal, total / size_minus_meta


def read_frame_metadata(cam, thermal):
    """RMH_IRThermalCamera_ReadCalFrameMetaData -> sensor temperatures and the max/min/centre raw values."""
    if cam.metadata_rows >= cam.height:
        raise SnapshotError("metadata rows >= frame height")
    t = thermal
    out = {}
    if cam.pool in (1, 3):
        out["fpa_raw"] = t[cam.meta1 + 2]
        out["fpa"] = 20.0 - (out["fpa_raw"] - cam.fpa_off) / cam.fpa_div
        out["shutter_raw"] = t[cam.meta2 + 2]
        out["core_raw"] = t[cam.meta2 + 3]
        if cam.pool == 1:
            out["shutter"] = out["shutter_raw"] / 10.0 - 273.15
            out["core"] = out["core_raw"] / 10.0 - 273.15
        else:
            out["shutter"] = ((out["shutter_raw"] * 0.625) + 2731.5) / 10.0 - 273.15
            out["core"] = ((out["core_raw"] * 0.625) + 2731.5) / 10.0 - 273.15
    else:
        for key in ("fpa", "shutter", "core"):
            out[key + "_raw"], out[key] = 1, 1.0
    if cam.pool == 1:
        i = cam.meta1
        idx = (i + 3, i + 4, i + 5, i + 6, i + 7, i + 8, i + 13)
    else:
        i = cam.meta3 if cam.pool == 3 else cam.meta1
        idx = tuple(i + k for k in range(7))
    for key, index in zip(("max_x", "max_y", "max_raw", "min_x", "min_y", "min_raw", "center_raw"), idx):
        out[key] = t[index]
    return out


class TemperatureModel:
    """Calibration read-out, look-up table (pools 1/3) or environment correction (pools 2/4), and pixel temperature."""

    SIGMA = 0.0000000567

    def __init__(self, cam, thermal, sensor):
        self.cam = cam
        s = cam.settings
        self.cal = [0.0] * 6
        if cam.pool in (1, 3):
            m2 = cam.meta2
            self.cal[0] = float(thermal[m2 + 1])
            for k in range(1, 6):
                hi, lo = thermal[m2 + 1 + 2 * k + 2], thermal[m2 + 2 + 2 * k + 2]
                self.cal[k] = struct.unpack("<f", struct.pack("<I", (hi << 16) | lo))[0]

        humidity, ambient, reflected = s["humidity"], s["ambient"], s["reflected"]
        emissivity, correction = s["emissivity"], s["temperature_correction"]
        distance_ushort = int(s["distance"]) & 0xFFFF
        omega = humidity * math.exp(ambient ** 3 * 0.68455e-6 + ambient ** 2 * -0.000278160 + ambient * 0.069390 + 1.5587)
        root_d = math.sqrt(distance_ushort)
        tau = 1.9 * math.exp(-root_d * (0.0066 + -0.0023 * math.sqrt(omega))) \
            + (1 - 1.9) * math.exp(-root_d * (0.0126 + -0.0067 * math.sqrt(omega)))
        self.tau = tau
        self.lut = None

        if cam.pool in (1, 3):
            c0, c1, c2, c3, c4, c5 = self.cal
            numerator = (1.0 - emissivity) * tau * (reflected + 273.15) ** 4 + (1.0 - tau) * (ambient + 273.15) ** 4
            denominator = emissivity * tau
            # float / float and float * float round to single precision in the C++
            cal_a = f32(c2 / f32(c1 + c1))
            cal_b = f32(c2 * c2) / (f32(c1 * c1) * 4.0)
            cal_c = c1 * sensor["shutter"] ** 2 + sensor["shutter"] * c2
            cal_d = c3 * sensor["fpa"] ** 2 + c4 * sensor["fpa"] + c5
            correction0 = 0.0
            if cam.range_flag == 1:
                correction0 = max(0.0, cam.cal0_offset - sensor["fpa"] * cam.cal0_fpamul)
            offset = c0 - correction0
            distance = float(s["distance"])
            self.lut = []
            for i in range(16384):
                arg = ((i - offset) * cal_d + cal_c) / c1 + cal_b if c1 != 0 else float("nan")
                n = math.sqrt(-arg) if arg < 0 else math.sqrt(arg)
                wtot = (n - cal_a + 273.15) ** 4
                ratio = (wtot - numerator) / denominator if denominator else float("nan")
                ttot = (ratio ** 0.25 if ratio >= 0 else float("nan")) - 273.15
                self.lut.append(ttot + (distance * 0.85 - 1.125) * (ttot - ambient) / 100.0 + correction)
        else:
            sigma = self.SIGMA
            refl = (1.0 - emissivity) * tau * sigma * reflected
            amb = (1.0 - tau) * sigma * ambient
            core = (1.0 - sensor["shutter"] / sensor["core"]) * sigma * sensor["shutter"]
            ets = 1.0 / (emissivity * tau * sigma)
            self.env_offset = -(refl * ets) - (amb * ets) - (core * ets)
            self.env_factor = ets * (sigma * emissivity)
            self.correction = correction

    def temperature(self, raw):
        """RMH_IRThermalCamera_ReadPixelTemperature (raw is an unsigned short)"""
        raw = int(raw) & 0xFFFF
        if self.lut is not None:
            return self.lut[raw & 0x3FFF]
        return (raw * 0.015625 - 273.15) * self.env_factor + self.env_offset + self.correction


# ---------------------------------------------------------------------------------------------------------------------
# Whole-snapshot decode and summary
# ---------------------------------------------------------------------------------------------------------------------

GRID = 5    # temperatures are also sampled on a GRID x GRID lattice of image pixels


def decode_snapshot(png_bytes, range_flag=1):
    """Decode a RAW snapshot as Snapshot Analysis mode does. Returns a summary dict (see summarise)."""
    width, height, buffer = read_png_rgb(png_bytes)
    meta = read_metadata_row(width, height, buffer)
    problems = metadata_problems(meta, width, height)
    if problems:
        if meta["id"] == ID_STRING and not 1 <= meta["pool"] <= 4 and len(problems) == 1:
            raise UnsupportedPool(problems[0])
        raise SnapshotError("; ".join(problems))
    cam = Camera(meta["pool"], width, height, meta["metadata_rows"], meta["width_offset"], meta["height_offset"])
    cam.range_flag = range_flag
    for key in cam.settings:
        cam.settings[key] = meta[key]
    init_constants(cam)
    thermal, average = convert_to_thermal(cam, buffer)
    sensor = read_frame_metadata(cam, thermal)
    model = TemperatureModel(cam, thermal, sensor)
    return summarise(cam, meta, thermal, average, sensor, model)


def summarise(cam, meta, thermal, average, sensor, model):
    w, image_h = cam.width, cam.height - cam.metadata_rows
    image = thermal[:w * image_h]
    temps = [model.temperature(v) for v in image]
    finite = [t for t in temps if math.isfinite(t)]
    grid = []
    for gy in range(GRID):
        for gx in range(GRID):
            x = round(gx * (w - 1) / (GRID - 1))
            y = round(gy * (image_h - 1) / (GRID - 1))
            grid.append({"x": x, "y": y, "raw": image[y * w + x], "temperature": temps[y * w + x]})
    return {
        "frame": {"width": cam.width, "height": cam.height, "metadata_rows": cam.metadata_rows,
                  "width_offset": cam.width_offset, "height_offset": cam.height_offset,
                  "image_width": w, "image_height": image_h},
        "pool": cam.pool,
        "range_flag": cam.range_flag,
        "settings": {k: meta[k] for k in ("temperature_correction", "ambient", "reflected", "humidity", "emissivity",
                                          "distance")},
        "sensor": {"fpa": sensor["fpa"], "shutter": sensor["shutter"], "core": sensor["core"]},
        "calibration": model.cal,
        "raw_sha256": hashlib.sha256(struct.pack(f"<{len(image)}H", *image)).hexdigest(),
        "raw_average": average,
        "maximum": {"x": sensor["max_x"], "y": sensor["max_y"], "raw": sensor["max_raw"],
                    "temperature": model.temperature(sensor["max_raw"])},
        "minimum": {"x": sensor["min_x"], "y": sensor["min_y"], "raw": sensor["min_raw"],
                    "temperature": model.temperature(sensor["min_raw"])},
        "center": {"raw": sensor["center_raw"], "temperature": model.temperature(sensor["center_raw"])},
        "average_temperature": model.temperature(int(average)),
        "image_temperature": {"min": min(finite) if finite else None, "max": max(finite) if finite else None,
                              "mean": sum(finite) / len(finite) if finite else None,
                              "non_finite_pixels": len(temps) - len(finite)},
        "grid": grid,
    }
