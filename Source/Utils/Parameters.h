#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace Aura
{

//==============================================================================
// Room Presets
//==============================================================================
enum class RoomType
{
    Booth = 0,
    Room,
    Hall,
    Cathedral
};

namespace RoomPresets
{
    inline const juce::StringArray names = { "BOOTH", "ROOM", "HALL", "CATHEDRAL" };

    // Returns base size multiplier for each room type
    inline float getSizeMultiplier(RoomType type)
    {
        switch (type)
        {
            case RoomType::Booth:     return 0.2f;
            case RoomType::Room:      return 0.5f;
            case RoomType::Hall:      return 0.8f;
            case RoomType::Cathedral: return 1.0f;
            default:                  return 0.5f;
        }
    }

    // Returns base decay multiplier
    inline float getDecayMultiplier(RoomType type)
    {
        switch (type)
        {
            case RoomType::Booth:     return 0.3f;
            case RoomType::Room:      return 0.5f;
            case RoomType::Hall:      return 0.75f;
            case RoomType::Cathedral: return 1.0f;
            default:                  return 0.5f;
        }
    }
}

//==============================================================================
// Parameter IDs
//==============================================================================
namespace ParamIDs
{
    // Room preset
    inline const juce::String roomType { "roomType" };

    // Main controls
    inline const juce::String size { "size" };
    inline const juce::String decay { "decay" };
    inline const juce::String damping { "damping" };
    inline const juce::String preDelay { "preDelay" };
    inline const juce::String width { "width" };
    inline const juce::String mix { "mix" };

    // Early reflections
    inline const juce::String erLevel { "erLevel" };
    inline const juce::String erSize { "erSize" };

    // Filters
    inline const juce::String highCut { "highCut" };
    inline const juce::String lowCut { "lowCut" };

    // I/O
    inline const juce::String inputGain { "inputGain" };
    inline const juce::String outputGain { "outputGain" };

    // Modulation
    inline const juce::String modDepth { "modDepth" };
    inline const juce::String modRate { "modRate" };

    // Multi-band decay
    inline const juce::String lowDecay { "lowDecay" };
    inline const juce::String midDecay { "midDecay" };
    inline const juce::String highDecay { "highDecay" };
    inline const juce::String crossoverLow { "crossoverLow" };
    inline const juce::String crossoverHigh { "crossoverHigh" };
}

//==============================================================================
// Default Values
//==============================================================================
namespace Defaults
{
    constexpr int roomType = 1;          // Room
    constexpr float size = 50.0f;        // %
    constexpr float decay = 2.0f;        // seconds
    constexpr float damping = 50.0f;     // %
    constexpr float preDelay = 10.0f;    // ms
    constexpr float width = 100.0f;      // %
    constexpr float mix = 30.0f;         // %
    constexpr float erLevel = 50.0f;     // %
    constexpr float erSize = 50.0f;      // %
    constexpr float highCut = 12000.0f;  // Hz
    constexpr float lowCut = 80.0f;      // Hz
    constexpr float inputGain = 0.0f;    // dB
    constexpr float outputGain = 0.0f;   // dB

    // Modulation
    constexpr float modDepth = 30.0f;    // %
    constexpr float modRate = 50.0f;     // %

    // Multi-band decay
    constexpr float lowDecay = 100.0f;   // % (1.0x multiplier)
    constexpr float midDecay = 100.0f;   // %
    constexpr float highDecay = 100.0f;  // %
    constexpr float crossoverLow = 200.0f;   // Hz
    constexpr float crossoverHigh = 4000.0f; // Hz
}

//==============================================================================
// Parameter Ranges
//==============================================================================
namespace Ranges
{
    constexpr float decayMin = 0.1f;
    constexpr float decayMax = 10.0f;

    constexpr float preDelayMin = 0.0f;
    constexpr float preDelayMax = 200.0f;

    constexpr float highCutMin = 1000.0f;
    constexpr float highCutMax = 20000.0f;

    constexpr float lowCutMin = 20.0f;
    constexpr float lowCutMax = 500.0f;

    constexpr float gainMin = -24.0f;
    constexpr float gainMax = 12.0f;
}

//==============================================================================
// Parameter Layout
//==============================================================================
inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Room Type
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::roomType, 1 },
        "Room Type",
        RoomPresets::names,
        Defaults::roomType));

    // Size
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::size, 1 },
        "Size",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        Defaults::size,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Decay
    auto decayRange = juce::NormalisableRange<float>(Ranges::decayMin, Ranges::decayMax, 0.01f);
    decayRange.setSkewForCentre(2.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::decay, 1 },
        "Decay",
        decayRange,
        Defaults::decay,
        juce::AudioParameterFloatAttributes().withLabel("s")));

    // Damping
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::damping, 1 },
        "Damping",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        Defaults::damping,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Pre-delay
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::preDelay, 1 },
        "Pre-Delay",
        juce::NormalisableRange<float>(Ranges::preDelayMin, Ranges::preDelayMax, 0.1f),
        Defaults::preDelay,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    // Width
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::width, 1 },
        "Width",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        Defaults::width,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Mix
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::mix, 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        Defaults::mix,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Early Reflections Level
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::erLevel, 1 },
        "ER Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        Defaults::erLevel,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Early Reflections Size
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::erSize, 1 },
        "ER Size",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        Defaults::erSize,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // High Cut
    auto highCutRange = juce::NormalisableRange<float>(Ranges::highCutMin, Ranges::highCutMax, 1.0f);
    highCutRange.setSkewForCentre(5000.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::highCut, 1 },
        "High Cut",
        highCutRange,
        Defaults::highCut,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    // Low Cut
    auto lowCutRange = juce::NormalisableRange<float>(Ranges::lowCutMin, Ranges::lowCutMax, 1.0f);
    lowCutRange.setSkewForCentre(100.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::lowCut, 1 },
        "Low Cut",
        lowCutRange,
        Defaults::lowCut,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    // Input Gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::inputGain, 1 },
        "Input",
        juce::NormalisableRange<float>(Ranges::gainMin, Ranges::gainMax, 0.1f),
        Defaults::inputGain,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Output Gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::outputGain, 1 },
        "Output",
        juce::NormalisableRange<float>(Ranges::gainMin, Ranges::gainMax, 0.1f),
        Defaults::outputGain,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Modulation Depth
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::modDepth, 1 },
        "Mod Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        Defaults::modDepth,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Modulation Rate
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::modRate, 1 },
        "Mod Rate",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        Defaults::modRate,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Low Decay Multiplier
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::lowDecay, 1 },
        "Low Decay",
        juce::NormalisableRange<float>(50.0f, 200.0f, 1.0f),
        Defaults::lowDecay,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Mid Decay Multiplier
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::midDecay, 1 },
        "Mid Decay",
        juce::NormalisableRange<float>(50.0f, 200.0f, 1.0f),
        Defaults::midDecay,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // High Decay Multiplier
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::highDecay, 1 },
        "High Decay",
        juce::NormalisableRange<float>(50.0f, 200.0f, 1.0f),
        Defaults::highDecay,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Crossover Low Frequency
    auto xoverLowRange = juce::NormalisableRange<float>(80.0f, 400.0f, 1.0f);
    xoverLowRange.setSkewForCentre(200.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::crossoverLow, 1 },
        "X-Over Low",
        xoverLowRange,
        Defaults::crossoverLow,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    // Crossover High Frequency
    auto xoverHighRange = juce::NormalisableRange<float>(2000.0f, 8000.0f, 1.0f);
    xoverHighRange.setSkewForCentre(4000.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::crossoverHigh, 1 },
        "X-Over High",
        xoverHighRange,
        Defaults::crossoverHigh,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    return { params.begin(), params.end() };
}

} // namespace Aura
