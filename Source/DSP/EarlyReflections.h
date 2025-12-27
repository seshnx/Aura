#pragma once

#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>

namespace Aura
{

//==============================================================================
/**
 * Early Reflections Processor
 *
 * Simulates discrete early reflections from room surfaces
 * using a multi-tap delay line with configurable timing.
 */
class EarlyReflections
{
public:
    static constexpr int NumTaps = 12;

    EarlyReflections() = default;

    void prepare(double sr, int maxBlockSize)
    {
        sampleRate = sr;

        // Max 200ms of delay for ER
        int maxSamples = static_cast<int>(0.2 * sampleRate);
        for (int ch = 0; ch < 2; ++ch)
        {
            delayBuffer[ch].resize(static_cast<size_t>(maxSamples));
            std::fill(delayBuffer[ch].begin(), delayBuffer[ch].end(), 0.0f);
        }
        writeIndex = 0;

        updateTapTimes();
    }

    void reset()
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            std::fill(delayBuffer[ch].begin(), delayBuffer[ch].end(), 0.0f);
        }
        writeIndex = 0;
    }

    // Set room size (0-1) affects tap spacing
    void setSize(float s)
    {
        size = juce::jlimit(0.0f, 1.0f, s);
        updateTapTimes();
    }

    // Set level (0-1)
    void setLevel(float l)
    {
        level = juce::jlimit(0.0f, 1.0f, l);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        if (level < 0.001f)
            return;

        int numSamples = buffer.getNumSamples();
        int numChannels = juce::jmin(buffer.getNumChannels(), 2);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Write to delay buffer
            for (int ch = 0; ch < numChannels; ++ch)
            {
                delayBuffer[ch][static_cast<size_t>(writeIndex)] = buffer.getSample(ch, sample);
            }

            // Sum taps
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float erSum = 0.0f;

                for (int tap = 0; tap < NumTaps; ++tap)
                {
                    int readIndex = writeIndex - tapDelays[tap];
                    if (readIndex < 0)
                        readIndex += static_cast<int>(delayBuffer[ch].size());

                    // Alternate between channels for stereo spread
                    int srcChannel = (tap + ch) % numChannels;
                    erSum += delayBuffer[srcChannel][static_cast<size_t>(readIndex)] * tapGains[tap];
                }

                // Add ER to signal
                float dry = buffer.getSample(ch, sample);
                buffer.setSample(ch, sample, dry + erSum * level);
            }

            writeIndex = (writeIndex + 1) % static_cast<int>(delayBuffer[0].size());
        }
    }

private:
    void updateTapTimes()
    {
        // Base tap times in ms (simulating room reflections)
        const std::array<float, NumTaps> baseTimes = {
            5.0f, 8.0f, 12.0f, 17.0f, 23.0f, 31.0f,
            41.0f, 53.0f, 67.0f, 83.0f, 101.0f, 121.0f
        };

        // Base gains (decreasing with distance)
        const std::array<float, NumTaps> baseGains = {
            0.8f, 0.7f, 0.6f, 0.55f, 0.5f, 0.45f,
            0.4f, 0.35f, 0.3f, 0.25f, 0.2f, 0.15f
        };

        float sizeScale = 0.3f + size * 1.4f; // 0.3x to 1.7x scaling

        for (int i = 0; i < NumTaps; ++i)
        {
            float timeMs = baseTimes[i] * sizeScale;
            tapDelays[i] = static_cast<int>(timeMs * sampleRate / 1000.0);
            tapDelays[i] = juce::jlimit(1, static_cast<int>(delayBuffer[0].size()) - 1, tapDelays[i]);
            tapGains[i] = baseGains[i];
        }
    }

    double sampleRate = 44100.0;
    float size = 0.5f;
    float level = 0.5f;

    std::array<std::vector<float>, 2> delayBuffer;
    int writeIndex = 0;

    std::array<int, NumTaps> tapDelays = {};
    std::array<float, NumTaps> tapGains = {};
};

} // namespace Aura
