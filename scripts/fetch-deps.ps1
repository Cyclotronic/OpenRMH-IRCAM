# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Cyclotronic

<#
.SYNOPSIS
  Downloads the third-party dependencies OpenRMH-IRCAM builds against.
  Nothing downloaded here is stored in the repository (deps\ and src\packages\ are git-ignored).

.DESCRIPTION
  Populates:
    deps\opencv\build\...           OpenCV 4.9.0 prebuilt for Windows (Apache-2.0)
    deps\glfw\...                   GLFW 3.4 Windows binaries (zlib licence)
    deps\glew\...                   GLEW 2.3.1 Windows binaries (Modified BSD / MIT)
    src\packages\Microsoft.Windows.CppWinRT.2.0.240405.15\   (MIT, NuGet)

  Re-running skips anything already present. Use -Force to re-download.
  Needs only PowerShell 5.1+ and network access; no NuGet client is required.
#>
[CmdletBinding()]
param([switch]$Force)

$ErrorActionPreference = 'Stop'
$ProgressPreference    = 'SilentlyContinue'   # Invoke-WebRequest is far faster without the progress bar
$repo = Split-Path $PSScriptRoot -Parent
$deps = Join-Path $repo 'deps'
$tmp  = Join-Path $env:TEMP 'openrmh-ircam-deps'
New-Item -ItemType Directory -Force $deps, $tmp | Out-Null

function Get-File($url, $name) {
    $dest = Join-Path $tmp $name
    if ($Force -or -not (Test-Path $dest)) {
        Write-Host "  downloading $name"
        Invoke-WebRequest -Uri $url -OutFile $dest -UseBasicParsing
    }
    return $dest
}

# ---- OpenCV 4.9.0 (self-extracting 7-Zip archive; ~180 MB) -------------------------------------------
if ($Force -or -not (Test-Path "$deps\opencv\build\include\opencv2\opencv.hpp")) {
    Write-Host 'OpenCV 4.9.0'
    $exe = Get-File 'https://github.com/opencv/opencv/releases/download/4.9.0/opencv-4.9.0-windows.exe' 'opencv-4.9.0-windows.exe'
    & $exe "-o$deps" -y | Out-Null            # extracts to deps\opencv\{build,sources}
    if (-not (Test-Path "$deps\opencv\build\include\opencv2\opencv.hpp")) { throw 'OpenCV extraction failed' }
} else { Write-Host 'OpenCV already present' }

# ---- GLFW 3.4 ----------------------------------------------------------------------------------------
if ($Force -or -not (Test-Path "$deps\glfw\include\GLFW\glfw3.h")) {
    Write-Host 'GLFW 3.4'
    $zip = Get-File 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip' 'glfw-3.4.bin.WIN64.zip'
    Remove-Item "$deps\glfw" -Recurse -Force -ErrorAction SilentlyContinue
    Expand-Archive $zip -DestinationPath $tmp -Force
    Move-Item "$tmp\glfw-3.4.bin.WIN64" "$deps\glfw" -Force
} else { Write-Host 'GLFW already present' }

# ---- GLEW 2.3.1 --------------------------------------------------------------------------------------
if ($Force -or -not (Test-Path "$deps\glew\include\GL\glew.h")) {
    Write-Host 'GLEW 2.3.1'
    $zip = Get-File 'https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1-win32.zip' 'glew-2.3.1-win32.zip'
    Remove-Item "$deps\glew" -Recurse -Force -ErrorAction SilentlyContinue
    Expand-Archive $zip -DestinationPath $tmp -Force
    Move-Item "$tmp\glew-2.3.1" "$deps\glew" -Force
} else { Write-Host 'GLEW already present' }

# ---- C++/WinRT NuGet package (the project imports it from src\packages) ------------------------------
$pkgName = 'Microsoft.Windows.CppWinRT.2.0.240405.15'
$pkgDir  = Join-Path $repo "src\packages\$pkgName"
if ($Force -or -not (Test-Path "$pkgDir\build\native\Microsoft.Windows.CppWinRT.props")) {
    Write-Host $pkgName
    $nupkg = Get-File 'https://api.nuget.org/v3-flatcontainer/microsoft.windows.cppwinrt/2.0.240405.15/microsoft.windows.cppwinrt.2.0.240405.15.nupkg' "$pkgName.zip"
    Remove-Item $pkgDir -Recurse -Force -ErrorAction SilentlyContinue
    Expand-Archive $nupkg -DestinationPath $pkgDir -Force
} else { Write-Host 'C++/WinRT package already present' }

Write-Host "`nDependencies ready. Build with scripts\build.ps1"
