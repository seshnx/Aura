#pragma once

#include "PluginProcessor.h"
#include "UI/AuraLookAndFeel.h"
#include "UI/RoomSelector.h"
#include "UI/SectionPanel.h"
#include "BinaryData.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace Aura
{

//==============================================================================
/**
 * Enhanced Labeled Knob Component
 *
 * Rotary knob with label, supports different sizes for visual hierarchy
 */
class LabeledKnob : public juce::Component
{
public:
    enum class Size { Small, Medium, Large };

    LabeledKnob(const juce::String& name,
                juce::AudioProcessorValueTreeState& apvts,
                const juce::String& paramId,
                Size knobSize = Size::Medium)
        : attachment(apvts, paramId, slider), size(knobSize)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false,
                               size == Size::Large ? 70 : (size == Size::Medium ? 60 : 50),
                               size == Size::Large ? 20 : 16);
        addAndMakeVisible(slider);

        label.setText(name, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::Font(juce::FontOptions(size == Size::Large ? 12.0f : 10.0f)));
        label.setColour(juce::Label::textColourId, AuraLookAndFeel::Colors::textDim);
        addAndMakeVisible(label);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        int labelHeight = size == Size::Large ? 18 : 14;
        label.setBounds(bounds.removeFromTop(labelHeight));
        slider.setBounds(bounds);
    }

    void setLabelOnBottom(bool onBottom)
    {
        labelOnBottom = onBottom;
    }

private:
    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    Size size;
    bool labelOnBottom = false;
};

//==============================================================================
/**
 * Enhanced Decay Visualizer
 *
 * Larger, more prominent visualization with decay curve and level meter
 */
class EnhancedVisualizer : public juce::Component,
                           public juce::Timer
{
public:
    EnhancedVisualizer()
    {
        startTimerHz(30);
    }

    void setDecayLevel(float level) { currentLevel = level; }
    void setDecayTime(float seconds) { decayTime = seconds; }
    void setRoomType(int type) { roomType = type; }

    void timerCallback() override { repaint(); }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        const float cornerRadius = 8.0f;

        // Background
        juce::ColourGradient bgGradient(
            AuraLookAndFeel::Colors::bgDark,
            bounds.getX(), bounds.getY(),
            AuraLookAndFeel::Colors::bgMid.darker(0.3f),
            bounds.getX(), bounds.getBottom(), false);
        g.setGradientFill(bgGradient);
        g.fillRoundedRectangle(bounds, cornerRadius);

        // Grid lines
        g.setColour(AuraLookAndFeel::Colors::knobRing.withAlpha(0.3f));
        for (int i = 1; i < 4; ++i)
        {
            float y = bounds.getY() + bounds.getHeight() * i / 4.0f;
            g.drawHorizontalLine(static_cast<int>(y), bounds.getX() + 4, bounds.getRight() - 4);
        }

        // Time markers
        g.setFont(juce::Font(juce::FontOptions(9.0f)));
        g.setColour(AuraLookAndFeel::Colors::textDim.withAlpha(0.6f));
        g.drawText("0s", static_cast<int>(bounds.getX() + 4), static_cast<int>(bounds.getBottom() - 14), 20, 12, juce::Justification::left);
        g.drawText(juce::String(decayTime, 1) + "s", static_cast<int>(bounds.getRight() - 30), static_cast<int>(bounds.getBottom() - 14), 28, 12, juce::Justification::right);

        // Decay curve
        juce::Path curve;
        float w = bounds.getWidth() - 8;
        float h = bounds.getHeight() - 20;
        float startX = bounds.getX() + 4;
        float startY = bounds.getY() + 4;

        curve.startNewSubPath(startX, startY + h);

        float decayFactor = juce::jmax(0.1f, decayTime / 10.0f);
        float levelScale = juce::jmin(1.0f, currentLevel * 1.2f + 0.3f);

        for (float x = 0; x <= w; x += 1.5f)
        {
            float t = x / w;
            float amplitude = std::exp(-3.0f * t / decayFactor) * levelScale;
            float y = startY + h - (h * 0.85f * amplitude);
            curve.lineTo(startX + x, y);
        }

        curve.lineTo(startX + w, startY + h);
        curve.closeSubPath();

        // Gradient fill based on room type
        juce::Colour fillColor1, fillColor2;
        switch (roomType)
        {
            case 0: // Booth
                fillColor1 = AuraLookAndFeel::Colors::warm.withAlpha(0.6f);
                fillColor2 = AuraLookAndFeel::Colors::warm.withAlpha(0.1f);
                break;
            case 3: // Cathedral
                fillColor1 = AuraLookAndFeel::Colors::secondary.withAlpha(0.6f);
                fillColor2 = AuraLookAndFeel::Colors::secondary.withAlpha(0.1f);
                break;
            default: // Room, Hall
                fillColor1 = AuraLookAndFeel::Colors::primary.withAlpha(0.6f);
                fillColor2 = AuraLookAndFeel::Colors::primaryDark.withAlpha(0.1f);
                break;
        }

        juce::ColourGradient gradient(fillColor1, startX, startY, fillColor2, startX, startY + h, false);
        g.setGradientFill(gradient);
        g.fillPath(curve);

        // Curve outline with glow
        g.setColour(AuraLookAndFeel::Colors::primaryLight.withAlpha(0.8f));
        g.strokePath(curve, juce::PathStrokeType(2.0f));

        // Border
        g.setColour(AuraLookAndFeel::Colors::knobRing.withAlpha(0.6f));
        g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);

        // Label
        g.setColour(AuraLookAndFeel::Colors::textDim);
        g.setFont(juce::Font(juce::FontOptions(9.0f)));
        g.drawText("DECAY ENVELOPE", bounds.reduced(8, 4).removeFromTop(12),
                   juce::Justification::centredLeft);
    }

private:
    float currentLevel = 0.0f;
    float decayTime = 2.0f;
    int roomType = 1;
};

//==============================================================================
class AuraEditor : public juce::AudioProcessorEditor,
                   public juce::Timer
{
public:
    explicit AuraEditor(AuraProcessor& p);
    ~AuraEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AuraProcessor& processor;
    AuraLookAndFeel lookAndFeel;

    // Company logo
    juce::Image companyLogo;

    // Header components
    juce::Label titleLabel;
    juce::Label subtitleLabel;
    PresetSelector presetSelector { processor.getPresetManager() };

    // Room selector
    RoomSelector roomSelector;

    // Visualizer
    EnhancedVisualizer visualizer;

    // Section panels
    SectionPanel mainSection { "REVERB", true };
    SectionPanel erSection { "EARLY REFLECTIONS" };
    SectionPanel filterSection { "TONE" };
    SectionPanel ioSection { "I/O" };

    // Main controls (larger)
    LabeledKnob sizeKnob;
    LabeledKnob decayKnob;
    LabeledKnob mixKnob;

    // Secondary controls (medium)
    LabeledKnob dampingKnob;
    LabeledKnob preDelayKnob;
    LabeledKnob widthKnob;

    // Early reflections (small)
    LabeledKnob erLevelKnob;
    LabeledKnob erSizeKnob;

    // Filters (small)
    LabeledKnob highCutKnob;
    LabeledKnob lowCutKnob;

    // I/O (small)
    LabeledKnob inputKnob;
    LabeledKnob outputKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AuraEditor)
};

} // namespace Aura
