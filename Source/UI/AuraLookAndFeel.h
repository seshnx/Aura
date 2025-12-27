#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Aura
{

//==============================================================================
/**
 * Modern/Sleek Look and Feel for SeshNx Aura
 *
 * Theme: Purple/Violet accents, gradient backgrounds, smooth animations
 */
class AuraLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //==========================================================================
    // Color Palette
    //==========================================================================
    struct Colors
    {
        // Backgrounds
        static inline const juce::Colour bgDark { 0xff121218 };
        static inline const juce::Colour bgMid { 0xff1a1a24 };
        static inline const juce::Colour bgLight { 0xff252532 };

        // Purple/Violet accents
        static inline const juce::Colour primary { 0xff8b5cf6 };       // Vibrant violet
        static inline const juce::Colour primaryDark { 0xff6d28d9 };   // Deep purple
        static inline const juce::Colour primaryLight { 0xffa78bfa };  // Light violet
        static inline const juce::Colour glow { 0xff7c3aed };          // Glow purple

        // Secondary
        static inline const juce::Colour secondary { 0xff38bdf8 };     // Cyan accent
        static inline const juce::Colour warm { 0xfff472b6 };          // Pink accent

        // Text
        static inline const juce::Colour textBright { 0xffffffff };
        static inline const juce::Colour textNormal { 0xffc4b5fd };    // Light purple text
        static inline const juce::Colour textDim { 0xff6b7280 };

        // UI elements
        static inline const juce::Colour knobBg { 0xff1e1e2e };
        static inline const juce::Colour knobRing { 0xff3f3f5a };
    };

    //==========================================================================
    AuraLookAndFeel()
    {
        setColour(juce::Slider::backgroundColourId, Colors::knobBg);
        setColour(juce::Slider::thumbColourId, Colors::primary);
        setColour(juce::Slider::rotarySliderFillColourId, Colors::primary);
        setColour(juce::Slider::rotarySliderOutlineColourId, Colors::knobRing);
        setColour(juce::Slider::textBoxTextColourId, Colors::textNormal);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

        setColour(juce::Label::textColourId, Colors::textNormal);

        setColour(juce::TextButton::buttonColourId, Colors::bgLight);
        setColour(juce::TextButton::buttonOnColourId, Colors::primaryDark);
        setColour(juce::TextButton::textColourOffId, Colors::textNormal);
        setColour(juce::TextButton::textColourOnId, Colors::textBright);

        setColour(juce::ComboBox::backgroundColourId, Colors::bgLight);
        setColour(juce::ComboBox::textColourId, Colors::textNormal);
        setColour(juce::ComboBox::outlineColourId, Colors::knobRing);

        setColour(juce::PopupMenu::backgroundColourId, Colors::bgMid);
        setColour(juce::PopupMenu::textColourId, Colors::textNormal);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, Colors::primaryDark);
        setColour(juce::PopupMenu::highlightedTextColourId, Colors::textBright);
    }

    //==========================================================================
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Outer glow
        juce::ColourGradient glowGradient(
            Colors::glow.withAlpha(0.3f * sliderPosProportional), centreX, centreY,
            juce::Colours::transparentBlack, centreX + radius * 1.3f, centreY, true);
        g.setGradientFill(glowGradient);
        g.fillEllipse(rx - 8, ry - 8, rw + 16, rw + 16);

        // Background with gradient
        juce::ColourGradient bgGradient(
            Colors::bgLight, centreX, centreY - radius,
            Colors::knobBg, centreX, centreY + radius, false);
        g.setGradientFill(bgGradient);
        g.fillEllipse(rx, ry, rw, rw);

        // Outer ring
        g.setColour(Colors::knobRing);
        g.drawEllipse(rx, ry, rw, rw, 2.0f);

        // Value arc background
        juce::Path arcBg;
        arcBg.addCentredArc(centreX, centreY, radius - 8.0f, radius - 8.0f, 0.0f,
                            rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(Colors::bgDark);
        g.strokePath(arcBg, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));

        // Value arc with gradient
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius - 8.0f, radius - 8.0f, 0.0f,
                               rotaryStartAngle, angle, true);

        juce::ColourGradient arcGradient(
            Colors::primaryLight, centreX - radius, centreY,
            Colors::primary, centreX + radius, centreY, false);
        g.setGradientFill(arcGradient);
        g.strokePath(valueArc, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));

        // Pointer
        juce::Path pointer;
        auto pointerLength = radius * 0.5f;
        auto pointerThickness = 3.0f;
        pointer.addRoundedRectangle(-pointerThickness / 2, -pointerLength,
                                     pointerThickness, pointerLength * 0.6f, 1.5f);

        g.setColour(Colors::textBright);
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // Center with gradient
        float centerRadius = radius * 0.25f;
        juce::ColourGradient centerGradient(
            Colors::bgLight.brighter(0.1f), centreX, centreY - centerRadius,
            Colors::bgDark, centreX, centreY + centerRadius, false);
        g.setGradientFill(centerGradient);
        g.fillEllipse(centreX - centerRadius, centreY - centerRadius,
                      centerRadius * 2, centerRadius * 2);
    }

    //==========================================================================
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour&, bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        auto cornerSize = 8.0f;

        juce::Colour baseColor = Colors::bgLight;

        if (button.getToggleState())
        {
            // Active state with gradient
            juce::ColourGradient activeGradient(
                Colors::primary, 0, bounds.getY(),
                Colors::primaryDark, 0, bounds.getBottom(), false);
            g.setGradientFill(activeGradient);
            g.fillRoundedRectangle(bounds, cornerSize);

            // Glow
            g.setColour(Colors::glow.withAlpha(0.3f));
            g.drawRoundedRectangle(bounds.expanded(1), cornerSize + 1, 2.0f);
        }
        else
        {
            if (shouldDrawButtonAsHighlighted)
                baseColor = baseColor.brighter(0.1f);
            if (shouldDrawButtonAsDown)
                baseColor = baseColor.darker(0.1f);

            g.setColour(baseColor);
            g.fillRoundedRectangle(bounds, cornerSize);

            g.setColour(Colors::knobRing);
            g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
        }
    }

    //==========================================================================
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float, float,
                          juce::Slider::SliderStyle style, juce::Slider&) override
    {
        if (style == juce::Slider::LinearHorizontal)
        {
            auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
            auto trackY = bounds.getCentreY();

            // Track background
            g.setColour(Colors::bgDark);
            g.fillRoundedRectangle(bounds.getX(), trackY - 3, bounds.getWidth(), 6, 3);

            // Value fill
            float fillWidth = sliderPos - bounds.getX();
            juce::ColourGradient fillGradient(
                Colors::primaryLight, bounds.getX(), trackY,
                Colors::primary, sliderPos, trackY, false);
            g.setGradientFill(fillGradient);
            g.fillRoundedRectangle(bounds.getX(), trackY - 3, fillWidth, 6, 3);

            // Thumb
            float thumbRadius = 8.0f;
            g.setColour(Colors::textBright);
            g.fillEllipse(sliderPos - thumbRadius, trackY - thumbRadius,
                          thumbRadius * 2, thumbRadius * 2);
        }
    }

    //==========================================================================
    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::Font(juce::FontOptions(13.0f));
    }

    juce::Font getTextButtonFont(juce::TextButton&, int) override
    {
        return juce::Font(juce::FontOptions(13.0f).withStyle("Bold"));
    }
};

} // namespace Aura
