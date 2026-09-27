# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Cyclotronic
"""Build the user manual PDF from docs/manual/manual.md.

    python -m pip install -r docs/manual/requirements.txt
    python scripts/build-manual.py

Pandoc (from pypandoc_binary) converts the Markdown to Typst; the typst package compiles that to PDF. Both are pinned in
docs/manual/requirements.txt, so the local build and the GitHub Actions build use the same tools.

Output: build/manual/IRCAMSoftwareManual.pdf - the file name the program's User Guide button opens from the program
folder.
"""
import pathlib
import subprocess
import sys

import pypandoc
import typst

ROOT = pathlib.Path(__file__).resolve().parents[1]
DOC = ROOT / "docs" / "manual"
OUT = ROOT / "build" / "manual"
PDF_NAME = "IRCAMSoftwareManual.pdf"


def main() -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    # The Typst file is written beside manual.md so that the image paths (media/...) resolve.
    typ = DOC / "manual.typ"
    pdf = OUT / PDF_NAME
    try:
        subprocess.run(
            [
                pypandoc.get_pandoc_path(),
                str(DOC / "manual.md"),
                "--from=gfm",
                "--to=typst",
                "--standalone",
                f"--metadata-file={DOC / 'metadata.yaml'}",
                f"--lua-filter={DOC / 'pdf.lua'}",
                f"--output={typ}",
            ],
            check=True,
        )
        typst.compile(str(typ), output=str(pdf), root=str(DOC))
    finally:
        typ.unlink(missing_ok=True)
    print(f"wrote {pdf.relative_to(ROOT)} ({pdf.stat().st_size // 1024} KB)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
