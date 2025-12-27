#pragma once

#include "EarlyReflections.h"
#include "DampingFilter.h"
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>

namespace Aura
{

//==============================================================================
/**
 * Room Reverb Engine
 *
 * Schroeder-style algorithmic reverb with:
 * - Configurable room size
 * - Frequency-dependent decay (damping)
 * - Pre-delay
 * - Stereo width control
 * - High/Low cut filters
 */
class RoomReverb
{
public:
    static constexpr int NumAllpass = 4;
    static constexpr int NumComb = 8;

    RoomReverb() = default;

    void prepare(double sr, int maxBlockSize)
    {
        sampleRate = sr;

        // Pre-delay: up to 200ms
        int maxPreDelay = static_cast<int>(0.2 * sampleRate);
        for (int ch = 0; ch < 2; ++ch)
        {
            preDelayBuffer[ch].resize(static_cast<size_t>(maxPreDelay));
            std::fill(preDelayBuffer[ch].begin(), preDelayBuffer[ch].end(), 0.0f);
        }
        preDelayWriteIndex = 0;

        // Initialize comb filters
        const std::array<float, NumComb> combTimesMs = {
            25.3f, 26.9f, 28.9f, 30.7f, 32.7f, 34.4f, 36.1f, 38.6f
        };

        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < NumComb; ++i)
            {
                float offset = (ch == 0) ? 0.0f : 0.5f;
                int samples = static_cast<int>((combTimesMs[i] + offset) * sampleRate / 1000.0);
                combBuffers[ch][i].resize(static_cast<size_t>(samples + 500));
                std::fill(combBuffers[ch][i].begin(), combBuffers[ch][i].end(), 0.0f);
                combDelays[ch][i] = samples;
            }
            combWriteIndex[ch].fill(0);
        }

        // Initialize allpass filters
        const std::array<float, NumAllpass> allpassTimesMs = { 5.0f, 1.7f, 0.6f, 0.2f };

        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < NumAllpass; ++i)
            {
                float offset = (ch == 0) ? 0.0f : 0.1f;
                int samples = static_cast<int>((allpassTimesMs[i] + offset) * sampleRate / 1000.0);
                allpassBuffers[ch][i].resize(static_cast<size_t>(samples + 50));
                std::fill(allpassBuffers[ch][i].begin(), allpassBuffers[ch][i].end(), 0.0f);
                allpassDelays[ch][i] = samples;
            }
            allpassWriteIndex[ch].fill(0);
        }

        // Damping filters
        for (int i = 0; i < NumComb; ++i)
        {
            dampingFilters[0][i].prepare(sampleRate);
            dampingFilters[1][i].prepare(sampleRate);
        }

        // Output filters
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(maxBlockSize);
        spec.numChannels = 2;

        highCutFilter.prepare(spec);
        lowCutFilter.prepare(spec);

        updateFilters();
        updateFeedback();
    }

    void reset()
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            std::fill(preDelayBuffer[ch].begin(), preDelayBuffer[ch].end(), 0.0f);
            for (int i = 0; i < NumComb; ++i)
            {
                std::fill(combBuffers[ch][i].begin(), combBuffers[ch][i].end(), 0.0f);
                dampingFilters[ch][i].reset();
            }
            for (int i = 0; i < NumAllpass; ++i)
            {
                std::fill(allpassBuffers[ch][i].begin(), allpassBuffers[ch][i].end(), 0.0f);
            }
        }
        preDelayWriteIndex = 0;
        highCutFilter.reset();
        lowCutFilter.reset();
    }

    void setSize(float s)
    {
        size = juce::jlimit(0.0f, 1.0f, s);
        updateDelayTimes();
    }

    void setDecay(float decaySeconds)
    {
        decay = juce::jlimit(0.1f, 10.0f, decaySeconds);
        updateFeedback();
    }

    void setDamping(float d)
    {
        damping = juce::jlimit(0.0f, 1.0f, d);
        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < NumComb; ++i)
            {
                dampingFilters[ch][i].setDamping(damping * 0.7f);
            }
        }
    }

    void setPreDelay(float ms)
    {
        preDelaySamples = static_cast<int>(ms * sampleRate / 1000.0);
        preDelaySamples = juce::jlimit(0, static_cast<int>(preDelayBuffer[0].size()) - 1, preDelaySamples);
    }

    void setWidth(float w)
    {
        width = juce::jlimit(0.0f, 1.0f, w);
    }

    void setHighCut(float freq)
    {
        highCutFreq = juce::jlimit(1000.0f, 20000.0f, freq);
        updateFilters();
    }

    void setLowCut(float freq)
    {
        lowCutFreq = juce::jlimit(20.0f, 500.0f, freq);
        updateFilters();
    }

    float getDecayEnvelope() const { return decayEnvelope; }

    void process(juce::AudioBuffer<float>& buffer)
    {
        int numSamples = buffer.getNumSamples();
        int numChannels = juce::jmin(buffer.getNumChannels(), 2);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float leftIn = buffer.getSample(0, sample);
            float rightIn = (numChannels > 1) ? buffer.getSample(1, sample) : leftIn;

            // Pre-delay
            preDelayBuffer[0][preDelayWriteIndex] = leftIn;
            preDelayBuffer[1][preDelayWriteIndex] = rightIn;

            int readIndex = preDelayWriteIndex - preDelaySamples;
            if (readIndex < 0) readIndex += static_cast<int>(preDelayBuffer[0].size());

            float leftDelayed = preDelayBuffer[0][readIndex];
            float rightDelayed = preDelayBuffer[1][readIndex];

            preDelayWriteIndex = (preDelayWriteIndex + 1) % static_cast<int>(preDelayBuffer[0].size());

            // Process comb filters in parallel
            float leftComb = 0.0f;
            float rightComb = 0.0f;

            for (int i = 0; i < NumComb; ++i)
            {
                // Left channel
                {
                    int delay = combDelays[0][i];
                    int rIdx = combWriteIndex[0][i] - delay;
                    if (rIdx < 0) rIdx += static_cast<int>(combBuffers[0][i].size());

                    float delayed = combBuffers[0][i][rIdx];
                    float filtered = dampingFilters[0][i].process(delayed);
                    combBuffers[0][i][combWriteIndex[0][i]] = leftDelayed + filtered * feedback;
                    combWriteIndex[0][i] = (combWriteIndex[0][i] + 1) % static_cast<int>(combBuffers[0][i].size());

                    leftComb += delayed;
                }

                // Right channel
                {
                    int delay = combDelays[1][i];
                    int rIdx = combWriteIndex[1][i] - delay;
                    if (rIdx < 0) rIdx += static_cast<int>(combBuffers[1][i].size());

                    float delayed = combBuffers[1][i][rIdx];
                    float filtered = dampingFilters[1][i].process(delayed);
                    combBuffers[1][i][combWriteIndex[1][i]] = rightDelayed + filtered * feedback;
                    combWriteIndex[1][i] = (combWriteIndex[1][i] + 1) % static_cast<int>(combBuffers[1][i].size());

                    rightComb += delayed;
                }
            }

            leftComb /= NumComb;
            rightComb /= NumComb;

            // Process allpass filters in series
            float leftOut = leftComb;
            float rightOut = rightComb;

            for (int i = 0; i < NumAllpass; ++i)
            {
                leftOut = processAllpass(0, i, leftOut);
                rightOut = processAllpass(1, i, rightOut);
            }

            // Apply width
            float mid = (leftOut + rightOut) * 0.5f;
            float side = (leftOut - rightOut) * 0.5f * width;
            leftOut = mid + side;
            rightOut = mid - side;

            buffer.setSample(0, sample, leftOut);
            if (numChannels > 1)
                buffer.setSample(1, sample, rightOut);
        }

        // Apply output filters
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        highCutFilter.process(context);
        lowCutFilter.process(context);

        // Update decay envelope for visualization
        float maxLevel = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                maxLevel = juce::jmax(maxLevel, std::abs(buffer.getSample(ch, i)));
            }
        }
        decayEnvelope = decayEnvelope * 0.95f + maxLevel * 0.05f;
    }

private:
    float processAllpass(int ch, int index, float input)
    {
        int delay = allpassDelays[ch][index];
        int rIdx = allpassWriteIndex[ch][index] - delay;
        if (rIdx < 0) rIdx += static_cast<int>(allpassBuffers[ch][index].size());

        float delayed = allpassBuffers[ch][index][rIdx];
        float output = -allpassFeedback * input + delayed;
        allpassBuffers[ch][index][allpassWriteIndex[ch][index]] = input + allpassFeedback * delayed;
        allpassWriteIndex[ch][index] = (allpassWriteIndex[ch][index] + 1) %
                                        static_cast<int>(allpassBuffers[ch][index].size());

        return output;
    }

    void updateDelayTimes()
    {
        float sizeScale = 0.5f + size * 1.0f;

        const std::array<float, NumComb> baseCombMs = {
            25.3f, 26.9f, 28.9f, 30.7f, 32.7f, 34.4f, 36.1f, 38.6f
        };

        for (int ch = 0; ch < 2; ++ch)
        {
            for (int i = 0; i < NumComb; ++i)
            {
                float offset = (ch == 0) ? 0.0f : 0.5f;
                int newDelay = static_cast<int>((baseCombMs[i] + offset) * sizeScale * sampleRate / 1000.0);
                newDelay = juce::jlimit(1, static_cast<int>(combBuffers[ch][i].size()) - 1, newDelay);
                combDelays[ch][i] = newDelay;
            }
        }
    }

    void updateFeedback()
    {
        // Calculate feedback for desired RT60
        float avgDelaySec = 0.030f * (0.5f + size);
        feedback = std::pow(10.0f, -3.0f * avgDelaySec / decay);
        feedback = juce::jlimit(0.0f, 0.98f, feedback);
    }

    void updateFilters()
    {
        *highCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
            sampleRate, highCutFreq, 0.707f);
        *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
            sampleRate, lowCutFreq, 0.707f);
    }

    double sampleRate = 44100.0;

    // Parameters
    float size = 0.5f;
    float decay = 2.0f;
    float damping = 0.5f;
    float width = 1.0f;
    float highCutFreq = 12000.0f;
    float lowCutFreq = 80.0f;
    float feedback = 0.7f;
    int preDelaySamples = 0;

    static constexpr float allpassFeedback = 0.5f;

    // Pre-delay
    std::array<std::vector<float>, 2> preDelayBuffer;
    int preDelayWriteIndex = 0;

    // Comb filters
    std::array<std::array<std::vector<float>, NumComb>, 2> combBuffers;
    std::array<std::array<int, NumComb>, 2> combDelays = {};
    std::array<std::array<int, NumComb>, 2> combWriteIndex = {};
    std::array<std::array<DampingFilter, NumComb>, 2> dampingFilters;

    // Allpass filters
    std::array<std::array<std::vector<float>, NumAllpass>, 2> allpassBuffers;
    std::array<std::array<int, NumAllpass>, 2> allpassDelays = {};
    std::array<std::array<int, NumAllpass>, 2> allpassWriteIndex = {};

    // Output filters
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> highCutFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> lowCutFilter;

    float decayEnvelope = 0.0f;
};

} // namespace Aura
