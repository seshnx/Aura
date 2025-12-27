#include <gtest/gtest.h>
#include "../Source/DSP/DampingFilter.h"
#include <cmath>

namespace Aura
{
namespace Tests
{

class DampingFilterTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        filter.prepare(44100.0);
    }

    void TearDown() override
    {
        filter.reset();
    }

    DampingFilter filter;
};

// Test that filter initializes correctly
TEST_F(DampingFilterTest, InitializesCorrectly)
{
    // After prepare, filter should be in a valid state
    // Process a sample to verify
    float result = filter.process(0.0f);
    EXPECT_FLOAT_EQ(result, 0.0f);
}

// Test that silence in produces silence out
TEST_F(DampingFilterTest, SilenceInSilenceOut)
{
    for (int i = 0; i < 100; ++i)
    {
        float result = filter.process(0.0f);
        EXPECT_FLOAT_EQ(result, 0.0f);
    }
}

// Test that filter affects signal
TEST_F(DampingFilterTest, FilterAffectsSignal)
{
    filter.setDamping(0.7f);

    // Send a series of impulses
    float sum = 0.0f;
    for (int i = 0; i < 100; ++i)
    {
        float input = (i == 0) ? 1.0f : 0.0f;
        float result = filter.process(input);
        sum += std::abs(result);
    }

    // Filter should produce output from the impulse
    EXPECT_GT(sum, 0.0f);
}

// Test damping parameter range
TEST_F(DampingFilterTest, DampingParameterRange)
{
    // All values should be accepted without throwing
    EXPECT_NO_THROW(filter.setDamping(0.0f));
    EXPECT_NO_THROW(filter.setDamping(0.5f));
    EXPECT_NO_THROW(filter.setDamping(1.0f));
}

// Test reset clears state
TEST_F(DampingFilterTest, ResetClearsState)
{
    filter.setDamping(0.5f);

    // Process an impulse
    filter.process(1.0f);
    filter.process(0.0f);
    filter.process(0.0f);

    // Reset
    filter.reset();

    // Next sample should be unaffected by previous state
    float result = filter.process(0.0f);
    EXPECT_FLOAT_EQ(result, 0.0f);
}

// Test different damping values produce different outputs
TEST_F(DampingFilterTest, DampingValuesProduceDifferentOutputs)
{
    DampingFilter filter1, filter2;
    filter1.prepare(44100.0);
    filter2.prepare(44100.0);

    filter1.setDamping(0.1f);
    filter2.setDamping(0.9f);

    float result1 = 0.0f, result2 = 0.0f;

    // Process same signal through both
    for (int i = 0; i < 50; ++i)
    {
        float input = (i < 10) ? 1.0f : 0.0f;
        result1 = filter1.process(input);
        result2 = filter2.process(input);
    }

    // Results should differ
    EXPECT_NE(result1, result2);
}

// Test filter doesn't produce NaN or Inf
TEST_F(DampingFilterTest, NoNaNOrInf)
{
    filter.setDamping(0.5f);

    // Process various inputs
    std::vector<float> inputs = {0.0f, 1.0f, -1.0f, 0.5f, -0.5f, 0.001f, -0.001f};

    for (float input : inputs)
    {
        for (int i = 0; i < 10; ++i)
        {
            float result = filter.process(input);
            EXPECT_FALSE(std::isnan(result));
            EXPECT_FALSE(std::isinf(result));
        }
        filter.reset();
    }
}

// Test filter stability with high damping
TEST_F(DampingFilterTest, StabilityWithHighDamping)
{
    filter.setDamping(0.99f);

    // Process a long sequence
    float maxOutput = 0.0f;
    for (int i = 0; i < 10000; ++i)
    {
        float input = (i % 100 == 0) ? 1.0f : 0.0f;
        float result = filter.process(input);
        maxOutput = std::max(maxOutput, std::abs(result));
    }

    // Output should remain bounded
    EXPECT_LT(maxOutput, 10.0f);
}

} // namespace Tests
} // namespace Aura
