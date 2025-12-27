#include "PresetManager.h"
#include "Parameters.h"

namespace Aura
{

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts)
    : valueTreeState(apvts)
{
    createFactoryPresets();
}

juce::File PresetManager::getUserPresetsDirectory() const
{
    auto presetDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                         .getChildFile("SeshNx")
                         .getChildFile("Aura")
                         .getChildFile("Presets");

    if (!presetDir.exists())
    {
        presetDir.createDirectory();
    }

    return presetDir;
}

void PresetManager::createFactoryPresets()
{
    // 1. Init - Default settings
    {
        FactoryPreset preset;
        preset.name = "Init";
        preset.category = "Default";
        factoryPresets.push_back(std::move(preset));
    }

    // 2. Vocal Booth - Tight, intimate space
    {
        FactoryPreset preset;
        preset.name = "Vocal Booth";
        preset.category = "Vocals";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 0);      // Booth
        xml->setAttribute(ParamIDs::size, 30.0f);
        xml->setAttribute(ParamIDs::decay, 0.5f);
        xml->setAttribute(ParamIDs::damping, 60.0f);
        xml->setAttribute(ParamIDs::preDelay, 5.0f);
        xml->setAttribute(ParamIDs::width, 80.0f);
        xml->setAttribute(ParamIDs::mix, 20.0f);
        xml->setAttribute(ParamIDs::erLevel, 70.0f);
        xml->setAttribute(ParamIDs::erSize, 40.0f);
        xml->setAttribute(ParamIDs::highCut, 8000.0f);
        xml->setAttribute(ParamIDs::lowCut, 150.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }

    // 3. Warm Room - Natural room sound
    {
        FactoryPreset preset;
        preset.name = "Warm Room";
        preset.category = "Rooms";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 1);      // Room
        xml->setAttribute(ParamIDs::size, 50.0f);
        xml->setAttribute(ParamIDs::decay, 1.2f);
        xml->setAttribute(ParamIDs::damping, 55.0f);
        xml->setAttribute(ParamIDs::preDelay, 15.0f);
        xml->setAttribute(ParamIDs::width, 100.0f);
        xml->setAttribute(ParamIDs::mix, 30.0f);
        xml->setAttribute(ParamIDs::erLevel, 50.0f);
        xml->setAttribute(ParamIDs::erSize, 50.0f);
        xml->setAttribute(ParamIDs::highCut, 10000.0f);
        xml->setAttribute(ParamIDs::lowCut, 100.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }

    // 4. Live Room - Bright, lively space
    {
        FactoryPreset preset;
        preset.name = "Live Room";
        preset.category = "Rooms";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 1);      // Room
        xml->setAttribute(ParamIDs::size, 65.0f);
        xml->setAttribute(ParamIDs::decay, 1.8f);
        xml->setAttribute(ParamIDs::damping, 30.0f);
        xml->setAttribute(ParamIDs::preDelay, 20.0f);
        xml->setAttribute(ParamIDs::width, 100.0f);
        xml->setAttribute(ParamIDs::mix, 35.0f);
        xml->setAttribute(ParamIDs::erLevel, 60.0f);
        xml->setAttribute(ParamIDs::erSize, 55.0f);
        xml->setAttribute(ParamIDs::highCut, 14000.0f);
        xml->setAttribute(ParamIDs::lowCut, 80.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }

    // 5. Concert Hall - Large, smooth hall
    {
        FactoryPreset preset;
        preset.name = "Concert Hall";
        preset.category = "Halls";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 2);      // Hall
        xml->setAttribute(ParamIDs::size, 75.0f);
        xml->setAttribute(ParamIDs::decay, 2.5f);
        xml->setAttribute(ParamIDs::damping, 45.0f);
        xml->setAttribute(ParamIDs::preDelay, 35.0f);
        xml->setAttribute(ParamIDs::width, 100.0f);
        xml->setAttribute(ParamIDs::mix, 40.0f);
        xml->setAttribute(ParamIDs::erLevel, 45.0f);
        xml->setAttribute(ParamIDs::erSize, 70.0f);
        xml->setAttribute(ParamIDs::highCut, 12000.0f);
        xml->setAttribute(ParamIDs::lowCut, 60.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }

    // 6. Cathedral - Massive, ethereal space
    {
        FactoryPreset preset;
        preset.name = "Cathedral";
        preset.category = "Large Spaces";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 3);      // Cathedral
        xml->setAttribute(ParamIDs::size, 90.0f);
        xml->setAttribute(ParamIDs::decay, 4.5f);
        xml->setAttribute(ParamIDs::damping, 40.0f);
        xml->setAttribute(ParamIDs::preDelay, 50.0f);
        xml->setAttribute(ParamIDs::width, 100.0f);
        xml->setAttribute(ParamIDs::mix, 45.0f);
        xml->setAttribute(ParamIDs::erLevel, 35.0f);
        xml->setAttribute(ParamIDs::erSize, 85.0f);
        xml->setAttribute(ParamIDs::highCut, 10000.0f);
        xml->setAttribute(ParamIDs::lowCut, 50.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }

    // 7. Ambient Pad - Long, lush tail for pads
    {
        FactoryPreset preset;
        preset.name = "Ambient Pad";
        preset.category = "Creative";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 3);      // Cathedral
        xml->setAttribute(ParamIDs::size, 100.0f);
        xml->setAttribute(ParamIDs::decay, 7.0f);
        xml->setAttribute(ParamIDs::damping, 65.0f);
        xml->setAttribute(ParamIDs::preDelay, 80.0f);
        xml->setAttribute(ParamIDs::width, 100.0f);
        xml->setAttribute(ParamIDs::mix, 60.0f);
        xml->setAttribute(ParamIDs::erLevel, 20.0f);
        xml->setAttribute(ParamIDs::erSize, 90.0f);
        xml->setAttribute(ParamIDs::highCut, 8000.0f);
        xml->setAttribute(ParamIDs::lowCut, 100.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }

    // 8. Drum Room - Punchy room for drums
    {
        FactoryPreset preset;
        preset.name = "Drum Room";
        preset.category = "Drums";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 1);      // Room
        xml->setAttribute(ParamIDs::size, 55.0f);
        xml->setAttribute(ParamIDs::decay, 0.8f);
        xml->setAttribute(ParamIDs::damping, 50.0f);
        xml->setAttribute(ParamIDs::preDelay, 0.0f);
        xml->setAttribute(ParamIDs::width, 90.0f);
        xml->setAttribute(ParamIDs::mix, 25.0f);
        xml->setAttribute(ParamIDs::erLevel, 80.0f);
        xml->setAttribute(ParamIDs::erSize, 45.0f);
        xml->setAttribute(ParamIDs::highCut, 12000.0f);
        xml->setAttribute(ParamIDs::lowCut, 120.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }

    // 9. Snare Plate - Bright, short plate-like
    {
        FactoryPreset preset;
        preset.name = "Snare Plate";
        preset.category = "Drums";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 0);      // Booth
        xml->setAttribute(ParamIDs::size, 40.0f);
        xml->setAttribute(ParamIDs::decay, 1.5f);
        xml->setAttribute(ParamIDs::damping, 25.0f);
        xml->setAttribute(ParamIDs::preDelay, 0.0f);
        xml->setAttribute(ParamIDs::width, 70.0f);
        xml->setAttribute(ParamIDs::mix, 30.0f);
        xml->setAttribute(ParamIDs::erLevel, 30.0f);
        xml->setAttribute(ParamIDs::erSize, 30.0f);
        xml->setAttribute(ParamIDs::highCut, 16000.0f);
        xml->setAttribute(ParamIDs::lowCut, 200.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }

    // 10. Dark Chamber - Moody, dark reverb
    {
        FactoryPreset preset;
        preset.name = "Dark Chamber";
        preset.category = "Creative";
        auto xml = std::make_unique<juce::XmlElement>("Preset");
        xml->setAttribute("name", preset.name);
        xml->setAttribute(ParamIDs::roomType, 2);      // Hall
        xml->setAttribute(ParamIDs::size, 70.0f);
        xml->setAttribute(ParamIDs::decay, 3.0f);
        xml->setAttribute(ParamIDs::damping, 80.0f);
        xml->setAttribute(ParamIDs::preDelay, 40.0f);
        xml->setAttribute(ParamIDs::width, 100.0f);
        xml->setAttribute(ParamIDs::mix, 35.0f);
        xml->setAttribute(ParamIDs::erLevel, 40.0f);
        xml->setAttribute(ParamIDs::erSize, 60.0f);
        xml->setAttribute(ParamIDs::highCut, 4000.0f);
        xml->setAttribute(ParamIDs::lowCut, 80.0f);
        preset.state = std::move(xml);
        factoryPresets.push_back(std::move(preset));
    }
}

void PresetManager::savePreset(const juce::String& presetName)
{
    auto presetFile = getUserPresetsDirectory().getChildFile(presetName + ".xml");

    auto state = valueTreeState.copyState();
    auto xml = state.createXml();

    if (xml)
    {
        xml->setAttribute("presetName", presetName);
        xml->writeTo(presetFile);
    }

    currentPresetName = presetName;
    presetModified = false;
}

void PresetManager::loadPreset(const juce::String& presetName)
{
    // First check factory presets
    for (size_t i = 0; i < factoryPresets.size(); ++i)
    {
        if (factoryPresets[i].name == presetName)
        {
            loadFactoryPreset(static_cast<int>(i));
            return;
        }
    }

    // Then check user presets
    auto presetFile = getUserPresetsDirectory().getChildFile(presetName + ".xml");

    if (presetFile.existsAsFile())
    {
        auto xml = juce::XmlDocument::parse(presetFile);
        if (xml)
        {
            valueTreeState.replaceState(juce::ValueTree::fromXml(*xml));
            currentPresetName = presetName;
            currentPresetIndex = -1; // User preset
            presetModified = false;
        }
    }
}

void PresetManager::deletePreset(const juce::String& presetName)
{
    auto presetFile = getUserPresetsDirectory().getChildFile(presetName + ".xml");

    if (presetFile.existsAsFile())
    {
        presetFile.deleteFile();
    }
}

void PresetManager::loadFactoryPreset(int index)
{
    if (index < 0 || index >= static_cast<int>(factoryPresets.size()))
    {
        return;
    }

    const auto& preset = factoryPresets[static_cast<size_t>(index)];

    if (index == 0)
    {
        // Init preset - reset to defaults
        initializeDefaultPreset();
    }
    else if (preset.state)
    {
        // Apply preset-specific settings
        initializeDefaultPreset();

        // Apply the preset settings
        auto setParam = [this](const juce::String& paramId, float value) {
            if (auto* param = valueTreeState.getParameter(paramId))
            {
                param->setValueNotifyingHost(param->convertTo0to1(value));
            }
        };

        if (preset.state->hasAttribute(ParamIDs::roomType))
            setParam(ParamIDs::roomType, static_cast<float>(preset.state->getIntAttribute(ParamIDs::roomType)));
        if (preset.state->hasAttribute(ParamIDs::size))
            setParam(ParamIDs::size, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::size)));
        if (preset.state->hasAttribute(ParamIDs::decay))
            setParam(ParamIDs::decay, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::decay)));
        if (preset.state->hasAttribute(ParamIDs::damping))
            setParam(ParamIDs::damping, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::damping)));
        if (preset.state->hasAttribute(ParamIDs::preDelay))
            setParam(ParamIDs::preDelay, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::preDelay)));
        if (preset.state->hasAttribute(ParamIDs::width))
            setParam(ParamIDs::width, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::width)));
        if (preset.state->hasAttribute(ParamIDs::mix))
            setParam(ParamIDs::mix, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::mix)));
        if (preset.state->hasAttribute(ParamIDs::erLevel))
            setParam(ParamIDs::erLevel, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::erLevel)));
        if (preset.state->hasAttribute(ParamIDs::erSize))
            setParam(ParamIDs::erSize, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::erSize)));
        if (preset.state->hasAttribute(ParamIDs::highCut))
            setParam(ParamIDs::highCut, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::highCut)));
        if (preset.state->hasAttribute(ParamIDs::lowCut))
            setParam(ParamIDs::lowCut, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::lowCut)));
        if (preset.state->hasAttribute(ParamIDs::inputGain))
            setParam(ParamIDs::inputGain, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::inputGain)));
        if (preset.state->hasAttribute(ParamIDs::outputGain))
            setParam(ParamIDs::outputGain, static_cast<float>(preset.state->getDoubleAttribute(ParamIDs::outputGain)));
    }

    currentPresetName = preset.name;
    currentPresetIndex = index;
    presetModified = false;
}

void PresetManager::initializeDefaultPreset()
{
    auto setParam = [this](const juce::String& paramId, float value) {
        if (auto* param = valueTreeState.getParameter(paramId))
        {
            param->setValueNotifyingHost(param->convertTo0to1(value));
        }
    };

    // Reset all parameters to their default values
    setParam(ParamIDs::roomType, static_cast<float>(Defaults::roomType));
    setParam(ParamIDs::size, Defaults::size);
    setParam(ParamIDs::decay, Defaults::decay);
    setParam(ParamIDs::damping, Defaults::damping);
    setParam(ParamIDs::preDelay, Defaults::preDelay);
    setParam(ParamIDs::width, Defaults::width);
    setParam(ParamIDs::mix, Defaults::mix);
    setParam(ParamIDs::erLevel, Defaults::erLevel);
    setParam(ParamIDs::erSize, Defaults::erSize);
    setParam(ParamIDs::highCut, Defaults::highCut);
    setParam(ParamIDs::lowCut, Defaults::lowCut);
    setParam(ParamIDs::inputGain, Defaults::inputGain);
    setParam(ParamIDs::outputGain, Defaults::outputGain);

    currentPresetName = "Init";
    currentPresetIndex = 0;
    presetModified = false;
}

juce::StringArray PresetManager::getFactoryPresetNames() const
{
    juce::StringArray names;
    for (const auto& preset : factoryPresets)
    {
        names.add(preset.name);
    }
    return names;
}

int PresetManager::getNumFactoryPresets() const
{
    return static_cast<int>(factoryPresets.size());
}

juce::StringArray PresetManager::getUserPresetNames() const
{
    juce::StringArray names;
    auto presetDir = getUserPresetsDirectory();

    for (const auto& file : presetDir.findChildFiles(juce::File::findFiles, false, "*.xml"))
    {
        names.add(file.getFileNameWithoutExtension());
    }

    names.sort(true);
    return names;
}

int PresetManager::getNumUserPresets() const
{
    return getUserPresetNames().size();
}

juce::StringArray PresetManager::getAllPresetNames() const
{
    juce::StringArray names;

    // Add factory presets first
    names.addArray(getFactoryPresetNames());

    // Add separator if there are user presets
    auto userPresets = getUserPresetNames();
    if (userPresets.size() > 0)
    {
        names.add("---"); // Separator
        names.addArray(userPresets);
    }

    return names;
}

} // namespace Aura
