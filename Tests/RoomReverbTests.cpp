#include <gtest/gtest.h>
#include "../Source/DSP/RoomReverb.h"
#include <cmath>

namespace Aura
{
namespace Tests
{

class RoomReverbTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        reverb.prepare(44100.0, 512);
    }

    void TearDown() override
    {
        reverb.reset();
    }

    RoomReverb reverb;
};

// Test that reverb initializes correctly
TEST_F(RoomReverbTest, InitializesCorrectly)
{
    // After prepare, reverb should be in a valid state
    EXPECT_FLOAT_EQ(reverb.getDecayEnvelope(), 0.0f);
}

// Test that silence in produces silence out (after tail decays)
TEST_F(RoomReverbTest, SilenceInSilenceOut)
{
    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();

    // Process multiple blocks to let any internal state decay
    for (int i = 0; i < 100; ++i)
    {
        reverb.process(buffer);
    }

    // Check output is essentially silent
    float maxLevel = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            maxLevel = std::max(maxLevel, std::abs(buffer.getSample(ch, i)));
        }
    }

    EXPECT_LT(maxLevel, 0.0001f);
}

// Test that impulse produces reverb tail
TEST_F(RoomReverbTest, ImpulseProducesReverb)
{
    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();

    // Create an impulse
    buffer.setSample(0, 0, 1.0f);
    buffer.setSample(1, 0, 1.0f);

    reverb.process(buffer);

    // Check that we have some output
    float maxLevel = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            maxLevel = std::max(maxLevel, std::abs(buffer.getSample(ch, i)));
        }
    }

    EXPECT_GT(maxLevel, 0.0f);
}

// Test that reset clears internal state
TEST_F(RoomReverbTest, ResetClearsState)
{
    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();

    // Create an impulse and process
    buffer.setSample(0, 0, 1.0f);
    buffer.setSample(1, 0, 1.0f);
    reverb.process(buffer);

    // Reset
    reverb.reset();

    // Process silence
    buffer.clear();
    reverb.process(buffer);

    // Output should be silent after reset
    float maxLevel = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            maxLevel = std::max(maxLevel, std::abs(buffer.getSample(ch, i)));
        }
    }

    EXPECT_LT(maxLevel, 0.0001f);
}

// Test size parameter affects output
TEST_F(RoomReverbTest, SizeAffectsOutput)
{
    juce::AudioBuffer<float> buffer1(2, 512);
    juce::AudioBuffer<float> buffer2(2, 512);

    // Small room
    reverb.setSize(0.2f);
    buffer1.clear();
    buffer1.setSample(0, 0, 1.0f);
    buffer1.setSample(1, 0, 1.0f);
    reverb.process(buffer1);
    reverb.reset();

    // Large room
    reverb.setSize(0.9f);
    buffer2.clear();
    buffer2.setSample(0, 0, 1.0f);
    buffer2.setSample(1, 0, 1.0f);
    reverb.process(buffer2);

    // The outputs should be different
    bool different = false;
    for (int i = 100; i < 400; ++i)
    {
        if (std::abs(buffer1.getSample(0, i) - buffer2.getSample(0, i)) > 0.001f)
        {
            different = true;
            break;
        }
    }

    EXPECT_TRUE(different);
}

// Test decay parameter
TEST_F(RoomReverbTest, DecayParameterValid)
{
    // Should accept valid decay values
    reverb.setDecay(0.1f);  // Minimum
    reverb.setDecay(5.0f);  // Mid-range
    reverb.setDecay(10.0f); // Maximum

    // Process should still work
    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();
    buffer.setSample(0, 0, 1.0f);

    EXPECT_NO_THROW(reverb.process(buffer));
}

// Test damping parameter
TEST_F(RoomReverbTest, DampingParameterValid)
{
    reverb.setDamping(0.0f);   // No damping
    reverb.setDamping(0.5f);   // Medium damping
    reverb.setDamping(1.0f);   // Full damping

    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();
    buffer.setSample(0, 0, 1.0f);

    EXPECT_NO_THROW(reverb.process(buffer));
}

// Test pre-delay parameter
TEST_F(RoomReverbTest, PreDelayParameterValid)
{
    reverb.setPreDelay(0.0f);    // No pre-delay
    reverb.setPreDelay(100.0f);  // 100ms
    reverb.setPreDelay(200.0f);  // Maximum

    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();
    buffer.setSample(0, 0, 1.0f);

    EXPECT_NO_THROW(reverb.process(buffer));
}

// Test width parameter
TEST_F(RoomReverbTest, WidthParameterValid)
{
    reverb.setWidth(0.0f);   // Mono
    reverb.setWidth(0.5f);   // 50% width
    reverb.setWidth(1.0f);   // Full stereo

    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();
    buffer.setSample(0, 0, 1.0f);

    EXPECT_NO_THROW(reverb.process(buffer));
}

// Test filter parameters
TEST_F(RoomReverbTest, FilterParametersValid)
{
    reverb.setHighCut(1000.0f);   // Low cutoff
    reverb.setHighCut(20000.0f);  // High cutoff
    reverb.setLowCut(20.0f);      // Low cutoff
    reverb.setLowCut(500.0f);     // High cutoff

    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();
    buffer.setSample(0, 0, 1.0f);

    EXPECT_NO_THROW(reverb.process(buffer));
}

// Test mono input handling
TEST_F(RoomReverbTest, MonoInputHandling)
{
    juce::AudioBuffer<float> monoBuffer(1, 512);
    monoBuffer.clear();
    monoBuffer.setSample(0, 0, 1.0f);

    EXPECT_NO_THROW(reverb.process(monoBuffer));

    // Check that output is valid
    bool hasOutput = false;
    for (int i = 0; i < monoBuffer.getNumSamples(); ++i)
    {
        if (std::abs(monoBuffer.getSample(0, i)) > 0.0001f)
        {
            hasOutput = true;
            break;
        }
    }

    EXPECT_TRUE(hasOutput);
}

// Test that output doesn't clip with reasonable input
TEST_F(RoomReverbTest, NoClippingWithReasonableInput)
{
    juce::AudioBuffer<float> buffer(2, 512);

    // Fill with moderate sine wave
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float sample = 0.5f * std::sin(2.0f * 3.14159f * 440.0f * i / 44100.0f);
        buffer.setSample(0, i, sample);
        buffer.setSample(1, i, sample);
    }

    reverb.process(buffer);

    // Check output doesn't exceed ±1.5 (some headroom for reverb)
    float maxLevel = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            maxLevel = std::max(maxLevel, std::abs(buffer.getSample(ch, i)));
        }
    }

    EXPECT_LT(maxLevel, 1.5f);
}

} // namespace Tests
} // namespace Aura
