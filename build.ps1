# Build script for SeshNx Aura VST3 on Windows
# PowerShell version for local builds

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "SeshNx Aura Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Change to script directory
Set-Location $PSScriptRoot
Write-Host "Working directory: $PWD" -ForegroundColor Green
Write-Host ""

# Find CMake
$cmakePath = $null
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    $cmakePath = "cmake"
    Write-Host "Found CMake in PATH" -ForegroundColor Green
} elseif (Test-Path "C:\Program Files\CMake\bin\cmake.exe") {
    $cmakePath = "C:\Program Files\CMake\bin\cmake.exe"
    Write-Host "Found CMake in Program Files" -ForegroundColor Green
} elseif (Test-Path "C:\Program Files (x86)\CMake\bin\cmake.exe") {
    $cmakePath = "C:\Program Files (x86)\CMake\bin\cmake.exe"
    Write-Host "Found CMake in Program Files (x86)" -ForegroundColor Green
} else {
    Write-Host "ERROR: CMake not found!" -ForegroundColor Red
    Write-Host "Please install CMake or add it to your PATH" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "Using CMake: $cmakePath" -ForegroundColor Green
Write-Host ""

# Find Visual Studio
$vsPath = $null
$vsGenerator = $null

if (Test-Path "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat") {
    $vsPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    $vsGenerator = "Visual Studio 17 2022"
    Write-Host "Found Visual Studio 2022" -ForegroundColor Green
} elseif (Test-Path "C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat") {
    $vsPath = "C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    $vsGenerator = "Visual Studio 16 2019"
    Write-Host "Found Visual Studio 2019" -ForegroundColor Green
} else {
    Write-Host "WARNING: Visual Studio not found in standard locations" -ForegroundColor Yellow
    Write-Host "Attempting to use CMake's auto-detection..." -ForegroundColor Yellow
    $vsGenerator = "Visual Studio 17 2022"
}

Write-Host "Using generator: $vsGenerator" -ForegroundColor Green
Write-Host ""

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Configure CMake
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Configuring CMake..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

$cmakeArgs = @(
    "-S", "."
    "-B", "build"
    "-G", $vsGenerator
    "-A", "x64"
)

& $cmakePath $cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: CMake configuration failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "CMake configuration successful!" -ForegroundColor Green
Write-Host ""

# Build
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Building SeshNx Aura (Release)..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

$buildArgs = @(
    "--build", "build"
    "--config", "Release"
)

& $cmakePath $buildArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: Build failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Build Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$vst3Path = "build\Aura_artefacts\Release\VST3\SeshNx Aura.vst3\Contents\x86_64-win\SeshNx Aura.vst3"
$standalonePath = "build\Aura_artefacts\Release\Standalone\SeshNx Aura.exe"

Write-Host "VST3 plugin location:" -ForegroundColor Cyan
if (Test-Path (Join-Path $PWD $vst3Path)) {
    Write-Host "  $vst3Path" -ForegroundColor Green
    Write-Host "  (File exists)" -ForegroundColor Green
} else {
    Write-Host "  $vst3Path" -ForegroundColor Yellow
    Write-Host "  (WARNING: File not found - check build output)" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Standalone location:" -ForegroundColor Cyan
if (Test-Path (Join-Path $PWD $standalonePath)) {
    Write-Host "  $standalonePath" -ForegroundColor Green
    Write-Host "  (File exists)" -ForegroundColor Green
} else {
    Write-Host "  $standalonePath" -ForegroundColor Yellow
    Write-Host "  (WARNING: File not found - check build output)" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Full path: $(Join-Path $PWD 'build\Aura_artefacts\Release')" -ForegroundColor Cyan
Write-Host ""
Read-Host "Press Enter to exit"
