# SeshNx Aura

**Algorithmic Room Reverb Plugin**

Aura is a professional-grade algorithmic reverb plugin that delivers lush, natural-sounding room ambience. Built on a classic Schroeder-style architecture with modern enhancements, Aura offers intuitive controls for crafting everything from intimate vocal booths to expansive cathedral spaces.

## Features

### Room Simulation
- **4 Room Types**: Booth, Room, Hall, and Cathedral presets with optimized size and decay characteristics
- **Schroeder Architecture**: 8 parallel comb filters + 4 series allpass filters for rich, dense reverb tails
- **Frequency-Dependent Decay**: Natural high-frequency damping for realistic room simulation

### Main Controls
- **Size** (0-100%): Controls the perceived room dimensions
- **Decay** (0.1-10s): Reverb tail length with RT60-based feedback calculation
- **Damping** (0-100%): High-frequency absorption for warmer tails
- **Pre-Delay** (0-200ms): Time before reverb onset for source clarity
- **Width** (0-100%): Stereo image control from mono to wide
- **Mix** (0-100%): Dry/wet balance

### Early Reflections
- **ER Level**: Independent control of early reflection intensity
- **ER Size**: Early reflection pattern scaling

### Tone Shaping
- **High Cut** (1kHz-20kHz): Low-pass filter on reverb output
- **Low Cut** (20Hz-500Hz): High-pass filter to prevent low-end buildup

### I/O
- **Input Gain** (-24dB to +12dB): Pre-reverb level adjustment
- **Output Gain** (-24dB to +12dB): Final output level

### Visualization
- Real-time decay envelope display for visual feedback

## Technical Specifications

- **Formats**: VST3, AU, Standalone
- **Platforms**: Windows, macOS
- **Sample Rates**: 44.1kHz - 192kHz
- **Latency**: Zero latency (algorithmic processing)
- **CPU**: Optimized DSP with denormal protection

## Building

### Prerequisites
- CMake 3.22 or higher
- C++17 compatible compiler
- Visual Studio 2019+ (Windows) or Xcode 12+ (macOS)

### Build Commands

**Windows (Command Prompt):**
```batch
build.bat
```

**Windows (PowerShell):**
```powershell
.\build.ps1
```

**macOS/Linux:**
```bash
./build.sh
```

### Manual Build
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Output Locations

After building:
- **VST3**: `build/Aura_artefacts/Release/VST3/SeshNx Aura.vst3`
- **Standalone**: `build/Aura_artefacts/Release/Standalone/SeshNx Aura.exe`
- **AU** (macOS): `build/Aura_artefacts/Release/AU/SeshNx Aura.component`

## Architecture

```
Source/
├── PluginProcessor.cpp/h    # Audio processing core
├── PluginEditor.cpp/h       # GUI implementation
├── DSP/
│   ├── RoomReverb.cpp/h     # Main reverb engine
│   ├── EarlyReflections.cpp/h # ER processor
│   └── DampingFilter.cpp/h  # Frequency-dependent damping
├── UI/
│   ├── AuraLookAndFeel.h    # Custom visual styling
│   └── RoomSelector.h       # Room type selector
└── Utils/
    ├── Parameters.cpp/h     # Parameter definitions
    └── PresetManager.cpp/h  # Preset management
```

## License

Copyright (c) 2024 Amalia Media LLC. All rights reserved.

See [LICENSE](LICENSE) for details.

---

**SeshNx** - Professional Audio Tools
