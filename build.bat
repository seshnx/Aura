@echo off
setlocal enabledelayedexpansion
REM Build script for SeshNx Aura VST3 on Windows

echo ========================================
echo SeshNx Aura Build Script
echo ========================================
echo.

REM Change to script directory
cd /d "%~dp0"
echo Working directory: %CD%
echo.

REM Check for CMake in PATH first
where cmake >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set "CMAKE_PATH=cmake"
    echo Found CMake in PATH
    goto :cmake_found
)

REM Try to find CMake in standard locations
set "CMAKE_PATH="
if exist "C:\Program Files\CMake\bin\cmake.exe" (
    set "CMAKE_PATH=C:\Program Files\CMake\bin\cmake.exe"
    echo Found CMake in Program Files
    goto :cmake_found
)
if exist "C:\Program Files (x86)\CMake\bin\cmake.exe" (
    set "CMAKE_PATH=C:\Program Files (x86)\CMake\bin\cmake.exe"
    echo Found CMake in Program Files (x86)
    goto :cmake_found
)

echo ERROR: CMake not found!
echo Please install CMake or add it to your PATH
pause
exit /b 1

:cmake_found
echo Using CMake: %CMAKE_PATH%
echo.

REM Find Visual Studio
set "VS_PATH="
set "VS_GENERATOR="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_GENERATOR=Visual Studio 17 2022"
    echo Found Visual Studio 2022
) else if exist "C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    set "VS_GENERATOR=Visual Studio 16 2019"
    echo Found Visual Studio 2019
) else (
    echo WARNING: Visual Studio not found in standard locations
    echo Attempting to use CMake's auto-detection...
    set "VS_GENERATOR=Visual Studio 17 2022"
)

echo Using generator: %VS_GENERATOR%
echo.

REM Create build directory
if not exist build mkdir build
echo.

REM Configure CMake
echo ========================================
echo Configuring CMake...
echo ========================================
if defined VS_PATH (
    call "%VS_PATH%" >nul 2>&1
)
"%CMAKE_PATH%" -S . -B build -G "%VS_GENERATOR%" -A x64

if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo CMake configuration successful!
echo.

REM Build
echo ========================================
echo Building SeshNx Aura (Release)...
echo ========================================
if defined VS_PATH (
    call "%VS_PATH%" >nul 2>&1
)
"%CMAKE_PATH%" --build build --config Release

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.
echo VST3 plugin location:
if exist "build\Aura_artefacts\Release\VST3\SeshNx Aura.vst3\Contents\x86_64-win\SeshNx Aura.vst3" (
    echo   build\Aura_artefacts\Release\VST3\SeshNx Aura.vst3\Contents\x86_64-win\SeshNx Aura.vst3
    echo   ^(File exists^)
) else (
    echo   build\Aura_artefacts\Release\VST3\SeshNx Aura.vst3\Contents\x86_64-win\SeshNx Aura.vst3
    echo   ^(WARNING: File not found - check build output^)
)
echo.
echo Standalone location:
if exist "build\Aura_artefacts\Release\Standalone\SeshNx Aura.exe" (
    echo   build\Aura_artefacts\Release\Standalone\SeshNx Aura.exe
    echo   ^(File exists^)
) else (
    echo   build\Aura_artefacts\Release\Standalone\SeshNx Aura.exe
    echo   ^(WARNING: File not found - check build output^)
)
echo.
echo Full path: %CD%\build\Aura_artefacts\Release
echo.
pause
