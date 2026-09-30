# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Cyclotronic
"""Source consistency checks for OpenRMH-IRCAM.

    python tests/static/check_source.py                 # checks on the working tree
    python tests/static/check_source.py --base origin/main   # also checks what changed since that ref

The program is organised around camera "pools" (protocol families) and a camera table. Adding a pool or a camera means
touching many `switch` statements and lists by hand, and a missed one compiles cleanly and fails only on a user's
camera. These checks find those misses without a Windows build, on any machine.

For pools specifically:
  - Integration: every switch on the pool handles every pool (or says why not), at least one camera uses each pool,
    the snapshot/recording loader accepts every pool, and docs/SUPPORTED-CAMERAS.md lists it.
  - Uniqueness: a pool exists only through what the program does differently for it. Every place the program
    branches on the pool (a switch on it, or an `if` comparing it) is compared across pools, after resolving
    constant macros and the per-pool functions those places call. A pool that does the same as another pool at
    every such place is a duplicate: its cameras belong in the existing pool. The count and list of places where
    each pool differs from its nearest neighbour is printed, so a reviewer can judge whether a small difference
    would be better handled inside the existing pool.

Each check prints its findings; the exit status is 1 if any check reported an error. Under GitHub Actions the
findings are also emitted as annotations on the pull request.

Opting out: a pool `switch` that deliberately has no case for a pool carries a comment inside the switch (or on the
line before it) of the form
    // pool-coverage: skip 5 6 - <reason>
and a camera-index `switch` likewise
    // camera-coverage: skip InfiRayT2L InfiRayT3S - <reason>
The reason is for the reviewer; the check only reads the numbers/names.

Standard library only, Python 3.9+.
"""
import argparse
import hashlib
import os
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
SRC = ROOT / "src"
CAMERA_TABLE = SRC / "RMH_SupportedIRCameras_Resources.h"
CAMERA_LIBRARY = SRC / "RMH_ThermalCameraSupport_Library.cpp"
ANALYSIS_ROUTINES = SRC / "RMH_AnalysisMode_Routines.cpp"
PROJECT_FILE = SRC / "IRCAM Thermal Viewer.vcxproj"

# Files that must never be committed (third-party libraries come from scripts/fetch-deps.ps1)
FORBIDDEN_EXTENSIONS = {".exe", ".dll", ".lib", ".pdb", ".obj", ".ilk", ".exp", ".zip", ".7z", ".msi", ".nupkg"}
# Size limits in bytes; fixtures and manual images have their own, larger allowance
MAX_FILE_SIZE = 1_000_000
MAX_FIXTURE_FILE_SIZE = 4_000_000
LARGE_FILE_ALLOWED = ("docs/manual/media/", "tests/fixtures/")
# Newly added files with these extensions need an SPDX header (CONTRIBUTING.md)
SPDX_EXTENSIONS = {".cpp", ".h", ".ps1", ".py", ".iss", ".lua"}


class Report:
    """Collects findings and prints them, as GitHub annotations when running under Actions."""

    def __init__(self):
        self.errors = 0
        self.warnings = 0
        self.github = os.environ.get("GITHUB_ACTIONS") == "true"

    def _emit(self, level, message, path=None, line=None):
        where = ""
        if path is not None:
            rel = pathlib.Path(path).resolve().relative_to(ROOT).as_posix() if pathlib.Path(path).is_absolute() else str(path)
            where = f"{rel}:{line}: " if line else f"{rel}: "
            if self.github:
                loc = f"file={rel}" + (f",line={line}" if line else "")
                print(f"::{level} {loc}::{message}")
                return
        print(f"{level.upper()}: {where}{message}")

    def error(self, message, path=None, line=None):
        self.errors += 1
        self._emit("error", message, path, line)

    def warning(self, message, path=None, line=None):
        self.warnings += 1
        self._emit("warning", message, path, line)


# ---------------------------------------------------------------------------------------------------------------------
# C++ source helpers
# ---------------------------------------------------------------------------------------------------------------------

def read_source(path):
    """Read a source file whatever its encoding (the tree mixes UTF-8 and Windows-1252)."""
    return path.read_bytes().decode("latin-1")


def blank_comments_and_strings(text):
    """Replace comments and string/char literals with spaces, keeping every offset and newline in place."""
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            for k in range(i, j):
                out[k] = " "
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                if out[k] != "\n":
                    out[k] = " "
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            for k in range(i + 1, min(j, n)):
                out[k] = " "
            i = j + 1
        else:
            i += 1
    return "".join(out)


def matching_brace(code, open_index, opening="{", closing="}"):
    """Index just past the bracket that closes the one at open_index."""
    depth = 0
    for i in range(open_index, len(code)):
        if code[i] == opening:
            depth += 1
        elif code[i] == closing:
            depth -= 1
            if depth == 0:
                return i + 1
    return len(code)


SWITCH_RE = re.compile(r"\bswitch\s*\(([^()]*(?:\([^()]*\)[^()]*)*)\)\s*\{")
FUNCTION_RE = re.compile(r"^[A-Za-z_][\w:<>,\*&\s]*?\b(\w+)\s*\([^;{}]*\)\s*\{", re.M)


def switches(code, selector_re):
    """Yield (start, body_start, body_end, selector) for each switch whose selector matches selector_re."""
    for m in SWITCH_RE.finditer(code):
        if re.search(selector_re, m.group(1)):
            body_start = m.end() - 1
            yield m.start(), body_start, matching_brace(code, body_start), m.group(1).strip()


CASE_LABEL_RE = re.compile(r"\bcase\s+(\w+)\s*:(?!:)|\bdefault\s*:(?!:)")


def case_groups(code, body_start, body_end):
    """The top-level case groups of a switch body, in order: [(labels, body_text)].

    Labels that follow each other with nothing between them share a group; the default label is None. Only labels at
    brace depth 0 of the body count, so a switch nested inside a case is left alone.
    """
    text = code[body_start + 1:body_end - 1]
    labels, depth, last = [], 0, 0
    for m in CASE_LABEL_RE.finditer(text):
        segment = text[last:m.start()]
        depth += segment.count("{") - segment.count("}")
        last = m.start()
        if depth == 0:
            labels.append((m.start(), m.end(), m.group(1)))
    groups = []
    for k, (_, end, label) in enumerate(labels):
        body = text[end:labels[k + 1][0] if k + 1 < len(labels) else len(text)]
        if groups and not groups[-1][1].strip():
            groups[-1] = (groups[-1][0] + [label], body)
        else:
            groups.append(([label], body))
    return groups


def top_level_cases(code, body_start, body_end):
    """Case labels of a switch body (ignoring nested switches), and whether it has a default."""
    labels = [label for group, _ in case_groups(code, body_start, body_end) for label in group]
    return [label for label in labels if label], None in labels


def enclosing_function(code, index):
    name = None
    for m in FUNCTION_RE.finditer(code, 0, index):
        name = m.group(1)
    return name or "?"


def line_of(text, index):
    return text.count("\n", 0, index) + 1


def annotation(raw, start, body_end, key):
    """Tokens listed after 'key: skip' in comments on the line before the switch or inside it."""
    line_start = raw.rfind("\n", 0, raw.rfind("\n", 0, start)) + 1
    region = raw[line_start:body_end]
    tokens = set()
    for m in re.finditer(key + r"\s*:\s*skip\s+([\w\s,]+?)(?:\s+-|$)", region, re.M):
        tokens.update(t for t in re.split(r"[\s,]+", m.group(1)) if t)
    return tokens


# ---------------------------------------------------------------------------------------------------------------------
# Camera table
# ---------------------------------------------------------------------------------------------------------------------

class CameraTable:
    def __init__(self, path):
        self.path = path
        raw = read_source(path)
        self.raw = raw
        self.pools = sorted({int(n) for n in re.findall(r"#define\s+_SupportedThermalCameras_Pool_(\d+)\s+\d+", raw)})
        self.cameras = {}      # name -> (index, line)
        for m in re.finditer(r"^#define\s+_SupportedThermalCamera_(\w+?)\s+(\d+)\s*(?://.*)?$", raw, re.M):
            self.cameras[m.group(1)] = (int(m.group(2)), line_of(raw, m.start()))
        self.high_range = set(re.findall(r"#define\s+_SupportedThermalCamera_(\w+)_SupportsHighRange\b", raw))
        self.frame_rate = set(re.findall(r"#define\s+_SupportedThermalCamera_(\w+)_FrameRate\b", raw))
        m = re.search(r"SupportedCamerasModelNames\s*=\s*\{(.*?)\};", raw, re.S)
        self.model_names = re.findall(r'"([^"]*)"', m.group(1)) if m else []
        self.model_names_line = line_of(raw, m.start()) if m else None
        self.device_name_lists = set(re.findall(r"std::vector<std::string>\s+(\w+DeviceNames)\s*=", raw))
        self.camera_pool = {}   # camera name -> pool, filled from the connect routine by check_camera_switches


def check_camera_table(table, report):
    if not table.pools:
        report.error("no _SupportedThermalCameras_Pool_<n> macros found", table.path)
        return
    if table.pools != list(range(1, len(table.pools) + 1)):
        report.error(f"pool numbers are not 1..n without gaps: {table.pools}", table.path)

    indices = {}
    for name, (index, line) in table.cameras.items():
        if index in indices:
            report.error(f"camera index {index} used by both {indices[index]} and {name}", table.path, line)
        indices[index] = name
        if name not in table.high_range:
            report.error(f"camera {name} has no _SupportedThermalCamera_{name}_SupportsHighRange macro", table.path, line)
        if name not in table.frame_rate:
            report.error(f"camera {name} has no _SupportedThermalCamera_{name}_FrameRate macro", table.path, line)
    expected = list(range(2, 2 + len(table.cameras)))
    if sorted(indices) != expected:
        report.error(f"camera indices are not contiguous from 2: {sorted(indices)}", table.path)

    # Index 0 and 1 are the two analysis modes; the list entry at a camera's index must exist
    if len(table.model_names) != len(table.cameras) + 2:
        report.error(f"SupportedCamerasModelNames has {len(table.model_names)} entries, expected {len(table.cameras) + 2} "
                     f"(2 analysis modes + {len(table.cameras)} cameras)", table.path, table.model_names_line)
    if len(set(table.model_names)) != len(table.model_names):
        report.error("SupportedCamerasModelNames has duplicate entries", table.path, table.model_names_line)


def check_camera_switches(table, report):
    """Every switch on the selected camera index handles every camera, and maps it to a real pool and name list."""
    raw = read_source(CAMERA_LIBRARY)
    code = blank_comments_and_strings(raw)
    all_cameras = {f"_SupportedThermalCamera_{n}" for n in table.cameras}
    found = 0
    for start, body_start, body_end, selector in switches(code, r"SellectedCameraIndex"):
        found += 1
        cases, has_default = top_level_cases(code, body_start, body_end)
        skipped = {f"_SupportedThermalCamera_{n}" for n in annotation(raw, start, body_end, "camera-coverage")}
        missing = sorted(all_cameras - set(cases) - skipped)
        if missing and not has_default:
            report.error(f"switch ({selector}) in {enclosing_function(code, start)} has no case for "
                         + ", ".join(m.replace("_SupportedThermalCamera_", "") for m in missing),
                         CAMERA_LIBRARY, line_of(raw, start))

        # The connect routine's switch maps each camera to its pool and device-name list
        if "ThermalCameraSupportPool" not in code[body_start:body_end]:
            continue
        for labels, body in case_groups(code, body_start, body_end):
            names = ", ".join(l.replace("_SupportedThermalCamera_", "") for l in labels if l)
            pools = re.findall(r"ThermalCameraSupportPool\s*=\s*_SupportedThermalCameras_Pool_(\d+)", body)
            if len(set(pools)) != 1:
                report.error(f"camera {names} is not mapped to exactly one pool in {enclosing_function(code, start)}",
                             CAMERA_LIBRARY, line_of(raw, start))
                continue
            pool = int(pools[0])
            if pool not in table.pools:
                report.error(f"camera {names} is mapped to undefined pool {pool}", CAMERA_LIBRARY, line_of(raw, start))
            for label in labels:
                if label:
                    table.camera_pool[label.replace("_SupportedThermalCamera_", "")] = pool
            for name in re.findall(r"CameraDeviceNamesPointer\s*=\s*(\w+)", body):
                if name not in table.device_name_lists:
                    report.error(f"device-name list {name} is not defined in {CAMERA_TABLE.name}",
                                 CAMERA_LIBRARY, line_of(raw, start))
    if found == 0:
        report.error("no switch on SellectedCameraIndex found; the check no longer matches the source", CAMERA_LIBRARY)
    elif not table.camera_pool:
        report.error("could not find the switch that maps cameras to pools; the check no longer matches the source",
                     CAMERA_LIBRARY)


def check_pool_switches(table, report):
    """Every switch on a camera pool has a case (or an explicit skip) for every defined pool."""
    all_pools = set(table.pools)
    found = 0
    for path in sorted(SRC.glob("*.cpp")) + sorted(SRC.glob("*.h")):
        raw = read_source(path)
        if "Pool" not in raw:
            continue
        code = blank_comments_and_strings(raw)
        for start, body_start, body_end, selector in switches(code, r"Pool\s*$"):
            found += 1
            cases, has_default = top_level_cases(code, body_start, body_end)
            if has_default:
                continue
            covered = {int(c.rsplit("_", 1)[1]) for c in cases if c.startswith("_SupportedThermalCameras_Pool_")}
            skipped = {int(t) for t in annotation(raw, start, body_end, "pool-coverage") if t.isdigit()}
            missing = sorted(all_pools - covered - skipped)
            if missing:
                report.error(f"switch ({selector}) in {enclosing_function(code, start)} has no case for pool "
                             + ", ".join(map(str, missing))
                             + " (add the case, or '// pool-coverage: skip <n> - <reason>' if it needs none)",
                             path, line_of(raw, start))
    if found == 0:
        report.error("no switch on a camera pool found; the check no longer matches the source", CAMERA_LIBRARY)


def check_pools_used_and_documented(table, report):
    """Every pool has at least one camera, and the camera list in the docs mentions every pool."""
    if not table.camera_pool:
        return
    for pool in table.pools:
        if pool not in table.camera_pool.values():
            report.error(f"pool {pool} is defined but no camera is mapped to it", table.path)
    docs = ROOT / "docs" / "SUPPORTED-CAMERAS.md"
    documented = set()
    for line in docs.read_text(encoding="utf-8").splitlines():
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if line.startswith("|") and len(cells) >= 3:
            documented.update(int(n) for n in re.findall(r"\b\d+\b", cells[-1]))
    for pool in table.pools:
        if pool not in documented:
            report.warning(f"docs/SUPPORTED-CAMERAS.md lists no camera in pool {pool}", docs)


class PoolBehaviour:
    """Normalises code so that two pools' code at a branch point can be compared for what it does, not how it reads.

    Comments and string contents are dropped, whitespace collapsed, constant #define macros replaced by their values
    (so two pools using differently named macros with equal values compare equal), and each per-pool function
    (a name ending in Pool<n> or Pool_<n>) replaced by a fingerprint of its own normalised body (so two pools calling
    copies of the same routine compare equal).
    """

    CONSTANT = re.compile(r"-?(?:0x[0-9A-Fa-f]+|\d+\.?\d*(?:[eE][-+]?\d+)?)[fFuUlL]*|true|false")
    IDENTIFIER = re.compile(r"\b[A-Za-z_]\w*\b")

    def __init__(self):
        self.macros = {}
        for path in sorted(SRC.glob("*.h")):
            for m in re.finditer(r"^[ \t]*#define[ \t]+(\w+)[ \t]+(\S+)", read_source(path), re.M):
                if self.CONSTANT.fullmatch(m.group(2)):
                    self.macros[m.group(1)] = m.group(2)
        self.functions = {}
        for path in sorted(SRC.glob("*.cpp")):
            code = blank_comments_and_strings(read_source(path))
            for m in FUNCTION_RE.finditer(code):
                if re.search(r"Pool_?\d+$", m.group(1)):
                    body = code[m.end() - 1:matching_brace(code, m.end() - 1)]
                    self.functions[m.group(1)] = "fn#" + hashlib.sha1(self._flatten(body).encode()).hexdigest()[:12]

    def _flatten(self, text):
        text = self.IDENTIFIER.sub(lambda m: self.macros.get(m.group(0), m.group(0)), text)
        return re.sub(r"\s+", " ", text).strip()

    def normalise(self, text):
        text = self.IDENTIFIER.sub(lambda m: self.functions.get(m.group(0), m.group(0)), text)
        return self._flatten(text)


class BranchPoint:
    def __init__(self, path, line, where, keys):
        self.path, self.line, self.where, self.keys = path, line, where, keys


def pool_branch_points(pools):
    """Every place the program does something that depends on the pool, with a behaviour key per pool."""
    behaviour = PoolBehaviour()
    points = []
    for path in sorted(SRC.glob("*.cpp")) + sorted(SRC.glob("*.h")):
        raw = read_source(path)
        if "_SupportedThermalCameras_Pool_" not in raw:
            continue
        code = blank_comments_and_strings(raw)
        for start, body_start, body_end, selector in switches(code, r"Pool\s*$"):
            keys, default = {}, None
            for labels, body in case_groups(code, body_start, body_end):
                key = behaviour.normalise(body)
                for label in labels:
                    if label is None:
                        default = key
                    elif label.startswith("_SupportedThermalCameras_Pool_"):
                        keys[int(label.rsplit("_", 1)[1])] = key
            # A pool with no case and no default is an integration gap (reported by check_pool_switches), not a
            # difference in behaviour: leave it out of the comparison
            for pool in pools:
                keys.setdefault(pool, default)
            points.append(BranchPoint(path, line_of(raw, start), f"switch in {enclosing_function(code, start)}", keys))
        for m in re.finditer(r"\bif\s*\(", code):
            condition = code[m.end():matching_brace(code, m.end() - 1, "(", ")") - 1]
            named = {int(n) for n in re.findall(r"_SupportedThermalCameras_Pool_(\d+)", condition)}
            if named:
                keys = {pool: "taken" if pool in named else "not taken" for pool in pools}
                points.append(BranchPoint(path, line_of(raw, m.start()), f"if in {enclosing_function(code, m.start())}", keys))
    return points


def nearest_pool(pool, others, points):
    """The pool in `others` that behaves most like `pool`, and the branch points where they differ."""
    best = None
    for other in others:
        if other != pool:
            differing = [p for p in points
                         if p.keys[pool] is not None and p.keys[other] is not None and p.keys[pool] != p.keys[other]]
            if best is None or len(differing) < len(best[1]):
                best = (other, differing)
    return best


def describe(points, limit=10):
    places = []
    for p in points:
        if p.where not in places:
            places.append(p.where)
    return ", ".join(places[:limit]) + (f", ... {len(places) - limit} more" if len(places) > limit else "")


def check_pool_uniqueness(table, report, base_pools):
    """A pool must do something no other pool does; report how each pool differs from its nearest neighbour."""
    if len(table.pools) < 2:
        return
    points = pool_branch_points(table.pools)
    print(f"Pool uniqueness: the program branches on the pool at {len(points)} places")
    for pool in table.pools:
        other, differing = nearest_pool(pool, table.pools, points)
        print(f"  pool {pool}: nearest is pool {other}, differs at {len(differing)} place(s)")
        if not differing and other < pool:
            report.error(f"pool {pool} does the same as pool {other} at every one of the {len(points)} places the program "
                         f"branches on the pool, so it is not a separate protocol: map its cameras to pool {other} "
                         "instead of adding a pool", table.path)
    for pool in sorted(set(table.pools) - set(base_pools or table.pools)):
        existing = [p for p in table.pools if p in base_pools]
        if existing:
            other, differing = nearest_pool(pool, existing, points)
            print(f"  new pool {pool}: nearest existing pool is {other}; differs at {len(differing)} place(s): "
                  f"{describe(differing) or 'none'}")
    print()


def check_file_metadata_validator(table, report):
    """Snapshot and recording files store the pool; the loader must accept every pool the program can write."""
    raw = read_source(ANALYSIS_ROUTINES)
    upper = re.search(r"CameraPoolID\s*>\s*(\d+)", raw)
    lower = re.search(r"CameraPoolID\s*<\s*(\d+)", raw)
    if not upper or not lower:
        report.error("could not find the CameraPoolID range check in RMH_AnalysisMode_IsRAWMetaDataValid", ANALYSIS_ROUTINES)
        return
    if int(upper.group(1)) != max(table.pools):
        report.error(f"RMH_AnalysisMode_IsRAWMetaDataValid rejects pools above {upper.group(1)}, but pools go up to "
                     f"{max(table.pools)}: snapshots and recordings from those cameras will be refused as corrupt",
                     ANALYSIS_ROUTINES, line_of(raw, upper.start()))
    if int(lower.group(1)) != min(table.pools):
        report.error(f"RMH_AnalysisMode_IsRAWMetaDataValid rejects pools below {lower.group(1)}",
                     ANALYSIS_ROUTINES, line_of(raw, lower.start()))


def check_reference_decoder(table, report):
    """tests/tools/ircam_raw.py should decode every pool, or captures from the missing ones cannot be checked."""
    decoder = ROOT / "tests" / "tools" / "ircam_raw.py"
    m = re.search(r"^POOLS\s*=\s*\(([^)]*)\)", decoder.read_text(encoding="utf-8"), re.M)
    implemented = {int(n) for n in re.findall(r"\d+", m.group(1))} if m else set()
    missing = sorted(set(table.pools) - implemented)
    if missing:
        report.warning("the reference snapshot decoder does not implement pool " + ", ".join(map(str, missing))
                       + ": hardware captures from those cameras cannot be checked (docs/HARDWARE-VALIDATION.md)",
                       decoder)


def check_project_file(report):
    """Every .cpp in src/ is compiled, and every file the project names exists."""
    # (ClInclude entries are not checked: several name GLEW/GLFW headers that live in deps/.)
    raw = PROJECT_FILE.read_text(encoding="utf-8-sig")
    compiled = set()
    for m in re.finditer(r'<ClCompile\s+Include="([^"]+)"', raw):
        rel = m.group(1).replace("\\", "/")
        compiled.add(rel)
        if not (SRC / rel).exists():
            report.error(f"project file compiles {rel}, which does not exist", PROJECT_FILE, line_of(raw, m.start()))
    for cpp in sorted(SRC.glob("*.cpp")):
        # A form's .cpp that only includes its header is a Visual Studio stub and is not built; that is fine
        if cpp.name not in compiled and not re.fullmatch(r'\s*#include\s+"[^"]*"\s*', read_source(cpp)):
            report.error(f"{cpp.name} is not compiled by the project (add <ClCompile Include=\"{cpp.name}\" /> "
                         "to the .vcxproj and .vcxproj.filters)", PROJECT_FILE)


# ---------------------------------------------------------------------------------------------------------------------
# Repository hygiene
# ---------------------------------------------------------------------------------------------------------------------

def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, check=True).stdout


def pools_at(ref):
    """The pool numbers defined at a git ref (None if the ref is unknown)."""
    try:
        text = git("show", f"{ref}:{CAMERA_TABLE.relative_to(ROOT).as_posix()}").decode("latin-1")
    except subprocess.CalledProcessError:
        return None
    return sorted({int(n) for n in re.findall(r"#define\s+_SupportedThermalCameras_Pool_(\d+)\s+\d+", text)})


def check_tracked_files(report):
    for rel in git("ls-files", "-z").decode().split("\0"):
        if rel and pathlib.PurePosixPath(rel).suffix.lower() in FORBIDDEN_EXTENSIONS:
            report.error("binary or build output is tracked; third-party files come from scripts/fetch-deps.ps1", rel)


def check_changes_since(base, report):
    """Checks on what changed since the base ref: size, encoding damage, and headers on new files."""
    try:
        git("rev-parse", "--verify", base + "^{commit}")
    except subprocess.CalledProcessError:
        report.warning(f"base ref '{base}' not found; skipping the checks on changed files")
        return
    merge_base = git("merge-base", base, "HEAD").decode().strip()
    changes = git("diff", "--name-status", "-z", "--no-renames", merge_base).decode().split("\0")
    for status, rel in zip(changes[0::2], changes[1::2]):
        path = ROOT / rel
        if status == "D" or not path.exists():
            continue
        new = path.read_bytes()
        # Size is checked on added or grown files only; a few of the original author's files are already large
        limit = MAX_FIXTURE_FILE_SIZE if rel.startswith(LARGE_FILE_ALLOWED) else MAX_FILE_SIZE
        if len(new) > limit and (status == "A" or len(new) > len(git("show", f"{merge_base}:{rel}"))):
            report.error(f"file is {len(new):,} bytes, over the {limit:,} byte limit for this location", rel)
        if b"\0" in new[:8000]:
            continue    # binary
        if status == "A":
            if pathlib.PurePosixPath(rel).suffix.lower() in SPDX_EXTENSIONS and b"SPDX-License-Identifier" not in new[:1000]:
                report.error("new file has no SPDX header (SPDX-License-Identifier: MIT and a copyright line)", rel, 1)
            continue
        try:
            old = git("show", f"{merge_base}:{rel}")
        except subprocess.CalledProcessError:
            continue
        # U+FFFD appears when a Windows-1252 file is opened as UTF-8 and saved: the original character is lost
        replacement = "�".encode()
        if new.count(replacement) > old.count(replacement):
            text = new.decode("utf-8", errors="replace")
            first = next((i + 1 for i, l in enumerate(text.splitlines()) if "�" in l), None)
            report.error("replacement characters (U+FFFD) introduced: the file was re-saved in the wrong encoding and "
                         "non-ASCII characters (e.g. the degree sign) were destroyed. Restore the original bytes.",
                         rel, first)
        elif not is_utf8(old) and is_utf8(new) and any(b > 0x7F for b in new):
            report.error("file converted from Windows-1252 to UTF-8; keep each file's encoding (CONTRIBUTING.md)", rel)


def is_utf8(data):
    try:
        data.decode("utf-8")
        return True
    except UnicodeDecodeError:
        return False


# ---------------------------------------------------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--base", help="git ref to compare against (e.g. origin/main); enables the changed-file checks")
    args = parser.parse_args()

    report = Report()
    table = CameraTable(CAMERA_TABLE)
    base_pools = pools_at(args.base) if args.base else None
    check_camera_table(table, report)
    check_camera_switches(table, report)
    check_pool_switches(table, report)
    check_pools_used_and_documented(table, report)
    check_pool_uniqueness(table, report, base_pools)
    check_file_metadata_validator(table, report)
    check_reference_decoder(table, report)
    check_project_file(report)
    check_tracked_files(report)
    if args.base:
        check_changes_since(args.base, report)

    print(f"\n{len(table.pools)} pools, {len(table.cameras)} cameras; {report.errors} error(s), {report.warnings} warning(s)")
    return 1 if report.errors else 0


if __name__ == "__main__":
    sys.exit(main())
