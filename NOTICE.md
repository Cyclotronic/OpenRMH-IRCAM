# Notice: authorship, licence status, third-party components

## Authorship

The application source in `src/` was written by **Rune Mark Hansen** (about 33 files carry the header
`Author: Rune Mark Hansen`, first dated 2022–2023). Copyright in that code belongs to the author unless
and until it is licensed to others. **No licence file accompanied the source we received.**

## Licence

On 24-25 September 2026 the original author announced, on the project's community Discord server, that the complete
source of IRCAM Thermal Viewer is released for everyone to use, copy, modify and own for free, and shared it publicly
(OneDrive). He did not name a licence. This repository applies the **MIT License** (see [`LICENSE`](LICENSE)) as the standard licence that
best matches that intent, with **Rune Mark Hansen** as copyright holder. New contributions are accepted under the same terms.

The MIT licence covers the author's original code. It does **not** relicense the third-party components below, which keep their own
licences, and it does not cover files whose origin is still to be confirmed (first table).

If the author prefers a different licence, this can be changed by him; contact the maintainers.

## Components in this repository that were not written by the author — provenance to confirm

| Path | What it is | Note |
|---|---|---|
| `src/ds_camera*.{h,cpp}`, `src/abstract_ds_camera.*`, `src/camera_device.*`, `src/uvc_camera*.{h,cpp}`, `src/cv_mat_convertor.*`, `src/ds_*.h`, `src/ds_libs_setting.h` | DirectShow/UVC camera access layer | Files carry no author or licence header. `cv_mat_convertor.h` cites OpenCV's `cap_dshow.cpp` as a reference. Confirm origin and licence before publishing. |
| `src/qedit.h` | Legacy Microsoft DirectShow header (removed from the modern Windows SDK) | Widely re-distributed workaround; origin noted in its first lines. |
| `src/res/MainIcon.ico`, `MaxTrackS.png`, `MinTrackS.png` | Application icon and two UI images | Author's artwork. Confirm no third-party marks. |
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
