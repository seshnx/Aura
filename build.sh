#!/bin/bash

# Build script for SeshNx Aura on macOS/Linux

echo "========================================"
echo "SeshNx Aura Build Script"
echo "========================================"
echo ""

# Change to script directory
cd "$(dirname "$0")"
echo "Working directory: $(pwd)"
echo ""

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found!"
    echo "Please install CMake (brew install cmake on macOS)"
    exit 1
fi

echo "Found CMake: $(which cmake)"
echo ""

# Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Platform: macOS"
    GENERATOR="Xcode"
else
    echo "Platform: Linux"
    GENERATOR="Unix Makefiles"
fi

echo "Using generator: $GENERATOR"
echo ""

# Create build directory
mkdir -p build

# Configure CMake
echo "========================================"
echo "Configuring CMake..."
echo "========================================"

cmake -S . -B build -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

echo ""
echo "CMake configuration successful!"
echo ""

# Build
echo "========================================"
echo "Building SeshNx Aura (Release)..."
echo "========================================"

cmake --build build --config Release

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: Build failed!"
    exit 1
fi

echo ""
echo "========================================"
echo "Build Complete!"
echo "========================================"
echo ""

# Show output locations
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "VST3 plugin location:"
    VST3_PATH="build/Aura_artefacts/Release/VST3/SeshNx Aura.vst3"
    if [ -d "$VST3_PATH" ]; then
        echo "  $VST3_PATH (exists)"
    else
        echo "  $VST3_PATH (not found)"
    fi

    echo ""
    echo "AU plugin location:"
    AU_PATH="build/Aura_artefacts/Release/AU/SeshNx Aura.component"
    if [ -d "$AU_PATH" ]; then
        echo "  $AU_PATH (exists)"
    else
        echo "  $AU_PATH (not found)"
    fi

    echo ""
    echo "Standalone location:"
    STANDALONE_PATH="build/Aura_artefacts/Release/Standalone/SeshNx Aura.app"
    if [ -d "$STANDALONE_PATH" ]; then
        echo "  $STANDALONE_PATH (exists)"
    else
        echo "  $STANDALONE_PATH (not found)"
    fi
else
    echo "VST3 plugin location:"
    VST3_PATH="build/Aura_artefacts/Release/VST3/SeshNx Aura.vst3"
    if [ -d "$VST3_PATH" ]; then
        echo "  $VST3_PATH (exists)"
    else
        echo "  $VST3_PATH (not found)"
    fi

    echo ""
    echo "Standalone location:"
    STANDALONE_PATH="build/Aura_artefacts/Release/Standalone/SeshNx Aura"
    if [ -f "$STANDALONE_PATH" ]; then
        echo "  $STANDALONE_PATH (exists)"
    else
        echo "  $STANDALONE_PATH (not found)"
    fi
fi

echo ""
echo "Full path: $(pwd)/build/Aura_artefacts/Release"
echo ""
