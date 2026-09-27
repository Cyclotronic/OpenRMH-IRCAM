<#
.SYNOPSIS
  Builds OpenRMH-IRCAM (Release|x64) with MSBuild from Visual Studio 2022 (or newer).

.DESCRIPTION
  Requires Visual Studio / Build Tools with:
    - Desktop development with C++  (MSVC x64/x86 build tools, Windows 10/11 SDK)
    - C++/CLI support for build tools
    - C++ ATL for latest build tools
    - .NET Framework 4.7.2 targeting pack
  Run scripts\fetch-deps.ps1 first.

  The project file asks for platform toolset v145 (Visual Studio 2026). On Visual Studio 2022 this script
  passes v143 instead. Override with -PlatformToolset.

  Tip: build from a local drive. MSBuild on a network share is slow and can fail on file ownership checks.
#>
[CmdletBinding()]
param(
    [string]$Configuration   = 'Release',
    [string]$PlatformToolset = '',
    [string]$WindowsSdk      = ''
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent

if (-not (Test-Path "$repo\deps\opencv\build\include\opencv2\opencv.hpp")) {
    throw 'Dependencies missing - run scripts\fetch-deps.ps1 first.'
}

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { throw 'Visual Studio not found (vswhere.exe missing).' }
$vs = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
if (-not $vs) { throw 'No Visual Studio with MSBuild found.' }
$msbuild = Join-Path $vs 'MSBuild\Current\Bin\amd64\MSBuild.exe'

if (-not $PlatformToolset) {
    $ver = & $vswhere -latest -products * -property installationVersion
    $PlatformToolset = if ([version]$ver -ge [version]'18.0') { 'v145' } else { 'v143' }
}

$args = @("$repo\src\IRCAM Thermal Viewer.sln", '/t:Build', "/p:Configuration=$Configuration", '/p:Platform=x64',
          "/p:PlatformToolset=$PlatformToolset", '/m', '/nologo', '/v:m')
if ($WindowsSdk) { $args += "/p:WindowsTargetPlatformVersion=$WindowsSdk" }

Write-Host "MSBuild: $msbuild`nToolset : $PlatformToolset"
& $msbuild @args
if ($LASTEXITCODE -ne 0) { throw "Build failed (exit $LASTEXITCODE)" }
Write-Host "`nBuilt: $repo\src\x64\$Configuration\IRCAM Thermal Viewer.exe"
