# Notice: authorship, licence status, third-party components

## Authorship

The application source in `src/` was written by **Rune Mark Hansen** (about 33 files carry the header
`Author: Rune Mark Hansen`, first dated 2022–2023). Copyright in that code belongs to the author unless
and until it is licensed to others. **No licence file accompanied the source we received.**

Name note: about 33 source headers read `Author: Rune Mark Hansen`, while the program's title bar text and one header
(`RMH_LiveView_ZoomWindow.h`) read `Rune Mark Glendorf`. Both are in the author's own release; this repository has not
changed either. The `LICENSE` copyright line uses the name in the majority of headers and will be updated if the author
asks for a different form.

## Licence

On 24-25 September 2026 the original author announced, on the project's community Discord server, that the complete
source of IRCAM Thermal Viewer is released for everyone to use, copy, modify and own for free, and shared it publicly
(OneDrive). He did not name a licence. This repository applies the **MIT License** (see [`LICENSE`](LICENSE)) as the standard licence that
best matches that intent, with **Rune Mark Hansen** as copyright holder. New contributions are accepted under the same terms.

The MIT licence covers the source released by the author. It does **not** relicense the third-party libraries listed below, which
keep their own licences. All source files in this repository come from the author's release and are attributed to him as the
releasing party; where a file carries a notice naming someone else, that notice governs that file.

If the author prefers a different licence, this can be changed by him; contact the maintainers.

## Who holds the copyright

Everything is offered under the MIT licence in [`LICENSE`](LICENSE); the table says whose copyright each part is.
Git history is the detailed record (`git log`, `git blame`).

| Part | Copyright |
|---|---|
| `src/*.h`, `src/*.cpp`, `src/*.resx`, solution and project, `examples/matlab/` (the original program) | Rune Mark Hansen. Files carry `Author: Rune Mark Hansen` where the author added a header. |
| Changes made for this repository to those files: `src/*.vcxproj` (dependency paths, no TurboActivate, runtime-DLL copy step), `src/*.rc` (relative icon path), `examples/matlab/` (removed personal paths) | Cyclotronic, contributed under the same MIT licence. The files remain principally the author's. |
| `scripts/`, `.github/`, `src/OpenRMH-IRCAM.deps.props`, `docs/`, `README.md`, `NOTICE.md` | Cyclotronic (files created for this repository carry an SPDX header). |
| DirectShow camera layer and the other files noted below | Released by Rune Mark Hansen as part of his source release. The files carry no other author or licence notice. |
| Libraries downloaded by `scripts/fetch-deps.ps1` (not in this repository) | Their own authors and licences - see the table further below. |

## Comment translation

The source comments were originally written in Danish. They were translated to English (about 5,500 comment lines in 53
files) without changing any code: for every file, the source with comments stripped is identical before and after, line
counts are unchanged, and each file keeps its original text encoding and line endings. The translation is a plain reading of
the author's comments, not a rewrite; comments that were commented-out code were left as they were. Translation errors are
possible - please open an issue or a pull request. The original Danish text remains in git history.

## Files whose exact authorship is not stated in the files

Everything below was released by the author with the rest of the source, and is treated as part of that release. The files
themselves do not say who first wrote them. If an upstream origin or a different copyright holder is identified, attribution
will be added here and in the files; please open an issue.

| Path | What it is | Note |
|---|---|---|
| `src/ds_camera*.{h,cpp}`, `src/abstract_ds_camera.*`, `src/camera_device.*`, `src/uvc_camera*.{h,cpp}`, `src/cv_mat_convertor.*`, `src/ds_*.h`, `src/ds_libs_setting.h` | DirectShow/UVC camera access layer | Files carry no author or licence header. `cv_mat_convertor.h` cites OpenCV's `cap_dshow.cpp` as a reference. Origin not stated; released with the author's source. |
| `src/qedit.h` | Legacy Microsoft DirectShow header (removed from the modern Windows SDK) | Widely re-distributed workaround; origin noted in its first lines. |
| `src/res/MainIcon.ico`, `MaxTrackS.png`, `MinTrackS.png` | Application icon and two UI images | Released by the author with the source. |
| `examples/matlab/*.m` | MATLAB examples for the CSV exports | Author's scripts. |

## Third-party libraries (fetched by `scripts/fetch-deps.ps1`, not stored in this repository)

| Library | Version | Licence | Used for |
|---|---|---|---|
| OpenCV | 4.9.0 | Apache-2.0 (bundled FFmpeg DLL: LGPL-2.1+ — see OpenCV's `LICENSE_FFMPEG.txt`) | image processing, video handling |
| GLFW | 3.4 | zlib/libpng | OpenGL windows/contexts |
| GLEW | 2.3.1 | Modified BSD / MIT (Mesa, Khronos) | OpenGL extension loading |
| Microsoft.Windows.CppWinRT | 2.0.240405.15 | MIT | build-time C++/WinRT support |

If you distribute a built program (an installer or zip), ship each library's licence text with it, and note that the
OpenCV FFmpeg DLL is LGPL — keep it as a separate DLL.

## Deliberately NOT in this repository

- **LimeLM TurboActivate** (`TurboActivate.dll/.lib/.h`): proprietary licensing SDK. The published source does not call it
  (the licence check is commented out in `RMH_ThermalCameraSupport_Library.cpp`), so the link dependency was removed.
- Microsoft VC++ redistributables, prebuilt executables, the original installer, and the author's Visual Studio
  build output, `.vs` cache, `.user` files and `.aps` resource cache.
- Adobe Acrobat / VBA COM interop assemblies (not referenced by the code).
- Third-party PDFs and logos from the original file share (image-processing textbook chapters, partner/Discord/Windows logos).
- The author's user manual (`.docx`), installer script, sample data, and website/Discord links beyond what is compiled into source —
  add them only with the author's approval.

## Personal / identifying content to review before publishing

- `src/RMH_Application_Information.h` — a Discord invite link.
- `src/WelcomeScreen.h` — "Visit My Website: https://rmg-engineering.com/" and a contact prompt.
- File headers naming the author. (These are attribution and should normally stay.)
