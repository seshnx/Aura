#pragma once

#include "AuraLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace Aura
{

//==============================================================================
/**
 * Section Panel Component
 *
 * A titled container panel for grouping related controls
 */
class SectionPanel : public juce::Component
{
public:
    SectionPanel(const juce::String& title, bool hasGlow = false)
        : titleText(title), showGlow(hasGlow)
    {
        // Don't intercept mouse clicks - this is just a visual background
        setInterceptsMouseClicks(false, false);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        const float cornerRadius = 10.0f;
        const float headerHeight = 24.0f;

        // Panel background with subtle gradient
        juce::ColourGradient bgGradient(
            AuraLookAndFeel::Colors::bgLight.withAlpha(0.6f),
            0, bounds.getY(),
            AuraLookAndFeel::Colors::bgMid.withAlpha(0.4f),
            0, bounds.getBottom(), false);
        g.setGradientFill(bgGradient);
        g.fillRoundedRectangle(bounds, cornerRadius);

        // Border
        g.setColour(AuraLookAndFeel::Colors::knobRing.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, 1.0f);

        // Header area
        auto headerBounds = bounds.removeFromTop(headerHeight);

        // Title
        g.setColour(AuraLookAndFeel::Colors::textDim);
        g.setFont(juce::Font(juce::FontOptions(10.0f).withStyle("Bold")));
        g.drawText(titleText, headerBounds.reduced(12, 0),
                   juce::Justification::centredLeft, true);

        // Glow effect for primary panels
        if (showGlow)
        {
            juce::ColourGradient glowGradient(
                AuraLookAndFeel::Colors::glow.withAlpha(0.05f),
                bounds.getCentreX(), bounds.getCentreY(),
                juce::Colours::transparentBlack,
                bounds.getCentreX() + bounds.getWidth() * 0.5f, bounds.getCentreY(), true);
            g.setGradientFill(glowGradient);
            g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRadius);
        }
    }

    // Returns content bounds in LOCAL coordinates (for child components)
    juce::Rectangle<int> getLocalContentBounds() const
    {
        return getLocalBounds().withTrimmedTop(28).reduced(8, 4);
    }

    // Returns content bounds in PARENT coordinates (for sibling components)
    juce::Rectangle<int> getContentBounds() const
    {
        auto local = getLocalBounds().withTrimmedTop(28).reduced(8, 4);
        return local.translated(getX(), getY());
    }

private:
    juce::String titleText;
    bool showGlow;
};

// Forward declaration
} // namespace Aura

#include "../Utils/PresetManager.h"

namespace Aura
{

//==============================================================================
/**
 * Preset Selector Component
 *
 * Dropdown for selecting presets with save functionality
 */
class PresetSelector : public juce::Component,
                       public juce::ComboBox::Listener
{
public:
    PresetSelector(PresetManager& pm)
        : presetManager(pm)
    {
        presetBox.setTextWhenNoChoicesAvailable("No Presets");
        presetBox.setTextWhenNothingSelected("Select Preset...");
        presetBox.addListener(this);
        addAndMakeVisible(presetBox);

        // Navigation buttons
        prevButton.setButtonText("<");
        prevButton.onClick = [this]() { navigatePreset(-1); };
        addAndMakeVisible(prevButton);

        nextButton.setButtonText(">");
        nextButton.onClick = [this]() { navigatePreset(1); };
        addAndMakeVisible(nextButton);

        // Populate with presets from PresetManager
        refreshPresetList();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        const int buttonWidth = 28;
        const int spacing = 4;

        prevButton.setBounds(bounds.removeFromLeft(buttonWidth));
        bounds.removeFromLeft(spacing);

        nextButton.setBounds(bounds.removeFromRight(buttonWidth));
        bounds.removeFromRight(spacing);

        presetBox.setBounds(bounds);
    }

    void comboBoxChanged(juce::ComboBox*) override
    {
        int selectedIndex = presetBox.getSelectedId() - 1; // ComboBox IDs are 1-based
        if (selectedIndex >= 0)
        {
            presetManager.loadFactoryPreset(selectedIndex);
        }
    }

    void refreshPresetList()
    {
        presetBox.clear();

        // Get factory presets from PresetManager
        auto presetNames = presetManager.getFactoryPresetNames();
        for (int i = 0; i < presetNames.size(); ++i)
        {
            presetBox.addItem(presetNames[i], i + 1); // ComboBox IDs are 1-based
        }

        // Select current preset
        int currentIndex = presetManager.getCurrentPresetIndex();
        presetBox.setSelectedId(currentIndex + 1, juce::dontSendNotification);
    }

private:
    void navigatePreset(int direction)
    {
        int current = presetBox.getSelectedId();
        int next = current + direction;
        int numItems = presetBox.getNumItems();

        if (next < 1) next = numItems;
        if (next > numItems) next = 1;

        presetBox.setSelectedId(next, juce::sendNotification);
    }

    PresetManager& presetManager;
    juce::ComboBox presetBox;
    juce::TextButton prevButton, nextButton;
};

} // namespace Aura
