#pragma once

#include "../Utils/Parameters.h"
#include "AuraLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace Aura
{

//==============================================================================
/**
 * Room Selector Component
 *
 * Four preset buttons for quick room selection:
 * Booth, Room, Hall, Cathedral
 */
class RoomSelector : public juce::Component,
                     public juce::AudioProcessorValueTreeState::Listener
{
public:
    RoomSelector(juce::AudioProcessorValueTreeState& apvts)
        : valueTreeState(apvts)
    {
        for (int i = 0; i < 4; ++i)
        {
            auto& btn = buttons[i];
            btn.setButtonText(RoomPresets::names[i]);
            btn.setClickingTogglesState(false); // We handle toggle manually
            btn.onClick = [this, i]()
            {
                selectRoom(i);
            };
            addAndMakeVisible(btn);
        }

        // Listen for parameter changes
        valueTreeState.addParameterListener(ParamIDs::roomType, this);

        // Initialize selection from current parameter value
        auto* param = valueTreeState.getRawParameterValue(ParamIDs::roomType);
        if (param != nullptr)
            updateSelection(static_cast<int>(param->load()));
    }

    ~RoomSelector() override
    {
        valueTreeState.removeParameterListener(ParamIDs::roomType, this);
    }

    void parameterChanged(const juce::String& parameterID, float newValue) override
    {
        if (parameterID == ParamIDs::roomType)
        {
            // Update UI on message thread
            juce::MessageManager::callAsync([this, newValue]()
            {
                updateSelection(static_cast<int>(newValue));
            });
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        int buttonWidth = bounds.getWidth() / 4;

        for (int i = 0; i < 4; ++i)
        {
            buttons[i].setBounds(i * buttonWidth, 0, buttonWidth, bounds.getHeight());
        }
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(AuraLookAndFeel::Colors::bgLight);
        g.fillRoundedRectangle(bounds, 6.0f);
    }

private:
    void selectRoom(int index)
    {
        if (auto* param = valueTreeState.getParameter(ParamIDs::roomType))
        {
            // For AudioParameterChoice, the normalized value maps 0-1 to 0-(numChoices-1)
            float normalizedValue = param->convertTo0to1(static_cast<float>(index));
            param->beginChangeGesture();
            param->setValueNotifyingHost(normalizedValue);
            param->endChangeGesture();
        }
    }

    void updateSelection(int index)
    {
        for (int i = 0; i < 4; ++i)
        {
            buttons[i].setToggleState(i == index, juce::dontSendNotification);
        }
    }

    juce::AudioProcessorValueTreeState& valueTreeState;
    std::array<juce::TextButton, 4> buttons;
};

//==============================================================================
/**
 * Decay Visualizer
 *
 * Shows reverb tail envelope visualization
 */
class DecayVisualizer : public juce::Component,
                        public juce::Timer
{
public:
    DecayVisualizer()
    {
        startTimerHz(30);
    }

    void setDecayLevel(float level)
    {
        currentLevel = level;
    }

    void setDecayTime(float seconds)
    {
        decayTime = seconds;
    }

    void timerCallback() override
    {
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);

        // Background
        g.setColour(AuraLookAndFeel::Colors::bgDark);
        g.fillRoundedRectangle(bounds, 4.0f);

        // Decay curve
        juce::Path curve;
        float w = bounds.getWidth();
        float h = bounds.getHeight();

        curve.startNewSubPath(bounds.getX(), bounds.getBottom());

        float decayFactor = juce::jmax(0.1f, decayTime / 10.0f);

        for (float x = 0; x <= w; x += 2.0f)
        {
            float t = x / w;
            float amplitude = std::exp(-3.0f * t / decayFactor) * currentLevel;
            float y = bounds.getBottom() - (h * 0.9f * amplitude);
            curve.lineTo(bounds.getX() + x, y);
        }

        curve.lineTo(bounds.getRight(), bounds.getBottom());
        curve.closeSubPath();

        // Gradient fill
        juce::ColourGradient gradient(
            AuraLookAndFeel::Colors::primary.withAlpha(0.7f),
            bounds.getX(), bounds.getY(),
            AuraLookAndFeel::Colors::primaryDark.withAlpha(0.3f),
            bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill(gradient);
        g.fillPath(curve);

        // Curve outline
        g.setColour(AuraLookAndFeel::Colors::primaryLight);
        g.strokePath(curve, juce::PathStrokeType(1.5f));

        // Border
        g.setColour(AuraLookAndFeel::Colors::knobRing);
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }

private:
    float currentLevel = 0.0f;
    float decayTime = 2.0f;
};

} // namespace Aura
