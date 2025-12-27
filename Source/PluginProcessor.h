#pragma once

#include "Utils/Parameters.h"
#include "Utils/PresetManager.h"
#include "DSP/RoomReverb.h"
#include "DSP/EarlyReflections.h"
#include <juce_audio_processors/juce_audio_processors.h>

namespace Aura
{

class AuraProcessor : public juce::AudioProcessor
{
public:
    AuraProcessor();
    ~AuraProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    PresetManager& getPresetManager() { return presetManager; }

    float getDecayEnvelope() const { return reverb.getDecayEnvelope(); }

private:
    juce::AudioProcessorValueTreeState apvts;
    PresetManager presetManager;

    // DSP
    RoomReverb reverb;
    EarlyReflections earlyReflections;
    juce::AudioBuffer<float> wetBuffer;

    // Parameter pointers
    std::atomic<float>* roomTypeParam = nullptr;
    std::atomic<float>* sizeParam = nullptr;
    std::atomic<float>* decayParam = nullptr;
    std::atomic<float>* dampingParam = nullptr;
    std::atomic<float>* preDelayParam = nullptr;
    std::atomic<float>* widthParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* erLevelParam = nullptr;
    std::atomic<float>* erSizeParam = nullptr;
    std::atomic<float>* highCutParam = nullptr;
    std::atomic<float>* lowCutParam = nullptr;
    std::atomic<float>* inputGainParam = nullptr;
    std::atomic<float>* outputGainParam = nullptr;

    float lastInputGain = 1.0f;
    float lastOutputGain = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AuraProcessor)
};

} // namespace Aura
