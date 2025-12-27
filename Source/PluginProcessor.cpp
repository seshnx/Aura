#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace Aura
{

AuraProcessor::AuraProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout()),
      presetManager(apvts)
{
    roomTypeParam = apvts.getRawParameterValue(ParamIDs::roomType);
    sizeParam = apvts.getRawParameterValue(ParamIDs::size);
    decayParam = apvts.getRawParameterValue(ParamIDs::decay);
    dampingParam = apvts.getRawParameterValue(ParamIDs::damping);
    preDelayParam = apvts.getRawParameterValue(ParamIDs::preDelay);
    widthParam = apvts.getRawParameterValue(ParamIDs::width);
    mixParam = apvts.getRawParameterValue(ParamIDs::mix);
    erLevelParam = apvts.getRawParameterValue(ParamIDs::erLevel);
    erSizeParam = apvts.getRawParameterValue(ParamIDs::erSize);
    highCutParam = apvts.getRawParameterValue(ParamIDs::highCut);
    lowCutParam = apvts.getRawParameterValue(ParamIDs::lowCut);
    inputGainParam = apvts.getRawParameterValue(ParamIDs::inputGain);
    outputGainParam = apvts.getRawParameterValue(ParamIDs::outputGain);
}

AuraProcessor::~AuraProcessor() = default;

void AuraProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    reverb.prepare(sampleRate, samplesPerBlock);
    earlyReflections.prepare(sampleRate, samplesPerBlock);

    wetBuffer.setSize(2, samplesPerBlock);
}

void AuraProcessor::releaseResources()
{
    reverb.reset();
    earlyReflections.reset();
}

bool AuraProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void AuraProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    // Get parameters
    int roomType = static_cast<int>(roomTypeParam->load());
    float sizeVal = sizeParam->load() / 100.0f;
    float decayVal = decayParam->load();
    float dampingVal = dampingParam->load() / 100.0f;
    float preDelayVal = preDelayParam->load();
    float widthVal = widthParam->load() / 100.0f;
    float mixVal = mixParam->load() / 100.0f;
    float erLevelVal = erLevelParam->load() / 100.0f;
    float erSizeVal = erSizeParam->load() / 100.0f;
    float highCutVal = highCutParam->load();
    float lowCutVal = lowCutParam->load();

    float inputGainLinear = juce::Decibels::decibelsToGain(inputGainParam->load());
    float outputGainLinear = juce::Decibels::decibelsToGain(outputGainParam->load());

    // Apply room type multipliers
    float roomSizeMultiplier = RoomPresets::getSizeMultiplier(static_cast<RoomType>(roomType));
    float roomDecayMultiplier = RoomPresets::getDecayMultiplier(static_cast<RoomType>(roomType));

    float effectiveSize = sizeVal * roomSizeMultiplier;
    float effectiveDecay = decayVal * roomDecayMultiplier;

    // Update DSP parameters
    reverb.setSize(effectiveSize);
    reverb.setDecay(effectiveDecay);
    reverb.setDamping(dampingVal);
    reverb.setPreDelay(preDelayVal);
    reverb.setWidth(widthVal);
    reverb.setHighCut(highCutVal);
    reverb.setLowCut(lowCutVal);

    earlyReflections.setSize(erSizeVal * roomSizeMultiplier);
    earlyReflections.setLevel(erLevelVal);

    // Apply input gain
    buffer.applyGainRamp(0, numSamples, lastInputGain, inputGainLinear);
    lastInputGain = inputGainLinear;

    // Copy to wet buffer
    wetBuffer.makeCopyOf(buffer, true);

    // Process early reflections on wet signal
    earlyReflections.process(wetBuffer);

    // Process reverb on wet signal
    reverb.process(wetBuffer);

    // Mix dry and wet
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* dry = buffer.getWritePointer(ch);
        const float* wet = wetBuffer.getReadPointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            dry[i] = dry[i] * (1.0f - mixVal) + wet[i] * mixVal;
        }
    }

    // Apply output gain
    buffer.applyGainRamp(0, numSamples, lastOutputGain, outputGainLinear);
    lastOutputGain = outputGainLinear;
}

juce::AudioProcessorEditor* AuraProcessor::createEditor()
{
    return new AuraEditor(*this);
}

void AuraProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AuraProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

} // namespace Aura

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Aura::AuraProcessor();
}
