#pragma once

#include <juce_dsp/juce_dsp.h>

namespace Aura
{

//==============================================================================
/**
 * Damping Filter for reverb tail
 * One-pole lowpass filter for frequency-dependent decay
 */
class DampingFilter
{
public:
    DampingFilter() = default;

    void prepare(double sr)
    {
        sampleRate = sr;
        reset();
    }

    void reset()
    {
        state = 0.0f;
    }

    // Set damping amount (0 = no damping, 1 = full damping)
    void setDamping(float damp)
    {
        damping = juce::jlimit(0.0f, 0.99f, damp);
    }

    float process(float input)
    {
        state = input * (1.0f - damping) + state * damping;
        return state;
    }

private:
    double sampleRate = 44100.0;
    float damping = 0.5f;
    float state = 0.0f;
};

} // namespace Aura
