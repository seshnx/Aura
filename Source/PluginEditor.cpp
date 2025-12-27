#include "PluginEditor.h"

namespace Aura
{

AuraEditor::AuraEditor(AuraProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      roomSelector(p.getAPVTS()),
      // Main controls - Large
      sizeKnob("SIZE", p.getAPVTS(), ParamIDs::size, LabeledKnob::Size::Large),
      decayKnob("DECAY", p.getAPVTS(), ParamIDs::decay, LabeledKnob::Size::Large),
      mixKnob("MIX", p.getAPVTS(), ParamIDs::mix, LabeledKnob::Size::Large),
      // Secondary controls - Medium
      dampingKnob("DAMPING", p.getAPVTS(), ParamIDs::damping, LabeledKnob::Size::Medium),
      preDelayKnob("PRE-DELAY", p.getAPVTS(), ParamIDs::preDelay, LabeledKnob::Size::Medium),
      widthKnob("WIDTH", p.getAPVTS(), ParamIDs::width, LabeledKnob::Size::Medium),
      // ER controls - Small
      erLevelKnob("LEVEL", p.getAPVTS(), ParamIDs::erLevel, LabeledKnob::Size::Small),
      erSizeKnob("SIZE", p.getAPVTS(), ParamIDs::erSize, LabeledKnob::Size::Small),
      // Filter controls - Small
      highCutKnob("HIGH", p.getAPVTS(), ParamIDs::highCut, LabeledKnob::Size::Small),
      lowCutKnob("LOW", p.getAPVTS(), ParamIDs::lowCut, LabeledKnob::Size::Small),
      // I/O controls - Small
      inputKnob("IN", p.getAPVTS(), ParamIDs::inputGain, LabeledKnob::Size::Small),
      outputKnob("OUT", p.getAPVTS(), ParamIDs::outputGain, LabeledKnob::Size::Small)
{
    // Load company logo
    if (BinaryData::company_logo_png != nullptr && BinaryData::company_logo_pngSize > 0)
    {
        companyLogo = juce::ImageCache::getFromMemory(BinaryData::company_logo_png,
                                                       BinaryData::company_logo_pngSize);
    }

    setLookAndFeel(&lookAndFeel);

    // Title
    titleLabel.setText("AURA", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(juce::FontOptions(28.0f).withStyle("Bold")));
    titleLabel.setColour(juce::Label::textColourId, AuraLookAndFeel::Colors::textBright);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("Algorithmic Reverb", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
    subtitleLabel.setColour(juce::Label::textColourId, AuraLookAndFeel::Colors::textDim);
    subtitleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(subtitleLabel);

    // Preset selector
    addAndMakeVisible(presetSelector);

    // Room selector
    addAndMakeVisible(roomSelector);

    // Visualizer
    addAndMakeVisible(visualizer);

    // Section panels
    addAndMakeVisible(mainSection);
    addAndMakeVisible(erSection);
    addAndMakeVisible(filterSection);
    addAndMakeVisible(ioSection);

    // Main controls
    addAndMakeVisible(sizeKnob);
    addAndMakeVisible(decayKnob);
    addAndMakeVisible(mixKnob);

    // Secondary controls
    addAndMakeVisible(dampingKnob);
    addAndMakeVisible(preDelayKnob);
    addAndMakeVisible(widthKnob);

    // ER controls
    addAndMakeVisible(erLevelKnob);
    addAndMakeVisible(erSizeKnob);

    // Filter controls
    addAndMakeVisible(highCutKnob);
    addAndMakeVisible(lowCutKnob);

    // I/O controls
    addAndMakeVisible(inputKnob);
    addAndMakeVisible(outputKnob);

    setSize(750, 520);
    startTimerHz(30);
}

AuraEditor::~AuraEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void AuraEditor::paint(juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient bgGradient(
        AuraLookAndFeel::Colors::bgDark, 0, 0,
        AuraLookAndFeel::Colors::bgMid, 0, static_cast<float>(getHeight()), false);
    g.setGradientFill(bgGradient);
    g.fillAll();

    // Subtle purple glow from center-top
    juce::ColourGradient glowGradient(
        AuraLookAndFeel::Colors::glow.withAlpha(0.06f),
        getWidth() * 0.5f, getHeight() * 0.2f,
        juce::Colours::transparentBlack,
        getWidth() * 0.5f + 350, getHeight() * 0.2f, true);
    g.setGradientFill(glowGradient);
    g.fillAll();

    // Header bar
    const int headerHeight = 60;
    auto headerArea = getLocalBounds().removeFromTop(headerHeight).toFloat();
    g.setColour(AuraLookAndFeel::Colors::bgDark.withAlpha(0.9f));
    g.fillRect(headerArea);

    // Header bottom accent line
    juce::ColourGradient lineGradient(
        juce::Colours::transparentBlack, 0, static_cast<float>(headerHeight),
        AuraLookAndFeel::Colors::primary.withAlpha(0.6f), getWidth() * 0.5f, static_cast<float>(headerHeight), false);
    lineGradient.addColour(1.0, juce::Colours::transparentBlack);
    g.setGradientFill(lineGradient);
    g.fillRect(0.0f, static_cast<float>(headerHeight - 2), static_cast<float>(getWidth()), 2.0f);

    // Draw company logo in center of header
    if (companyLogo.isValid())
    {
        const float logoHeight = 32.0f;
        const float logoAspect = static_cast<float>(companyLogo.getWidth()) /
                                 static_cast<float>(companyLogo.getHeight());
        const float logoWidth = logoHeight * logoAspect;

        const float logoX = (getWidth() - logoWidth) * 0.5f;
        const float logoY = (headerHeight - logoHeight) * 0.5f;

        juce::Rectangle<float> logoBounds(logoX, logoY, logoWidth, logoHeight);
        g.setOpacity(0.9f);
        g.drawImage(companyLogo, logoBounds, juce::RectanglePlacement::centred);
        g.setOpacity(1.0f);
    }
}

void AuraEditor::resized()
{
    const int margin = 12;
    const int headerHeight = 60;
    const int spacing = 10;

    auto bounds = getLocalBounds();

    // ===== HEADER =====
    auto headerArea = bounds.removeFromTop(headerHeight);

    // Title on left
    auto titleSection = headerArea.removeFromLeft(160).reduced(16, 8);
    titleLabel.setBounds(titleSection.removeFromTop(28));
    subtitleLabel.setBounds(titleSection);

    // Preset selector on right
    auto presetArea = headerArea.removeFromRight(220).reduced(16, 14);
    presetSelector.setBounds(presetArea);

    bounds.removeFromTop(spacing);

    // ===== MAIN CONTENT =====
    auto contentArea = bounds.reduced(margin, 0);

    // Layout: Left side has main controls, right side has visualizer and secondary panels

    // Top row - Main reverb section with visualizer
    auto topRow = contentArea.removeFromTop(260);

    // Main section panel (left - 58%)
    int mainWidth = static_cast<int>(topRow.getWidth() * 0.58f);
    auto mainArea = topRow.removeFromLeft(mainWidth);
    mainSection.setBounds(mainArea);

    // Position components inside panel
    auto mainContent = mainSection.getContentBounds();
    int largeKnobSize = 85;
    int mediumKnobSize = 65;
    int knobSpacing = 8;

    // Room selector at top of panel
    int roomSelectorWidth = 300;
    int roomSelectorX = mainContent.getX() + (mainContent.getWidth() - roomSelectorWidth) / 2;
    roomSelector.setBounds(roomSelectorX, mainContent.getY(), roomSelectorWidth, 30);

    // Primary controls row (Size, Decay, Mix) - centered and larger
    int primaryRowWidth = largeKnobSize * 3 + knobSpacing * 2;
    int primaryX = mainContent.getX() + (mainContent.getWidth() - primaryRowWidth) / 2;
    int primaryY = mainContent.getY() + 36;

    sizeKnob.setBounds(primaryX, primaryY, largeKnobSize, largeKnobSize + 18);
    decayKnob.setBounds(primaryX + largeKnobSize + knobSpacing, primaryY, largeKnobSize, largeKnobSize + 18);
    mixKnob.setBounds(primaryX + (largeKnobSize + knobSpacing) * 2, primaryY, largeKnobSize, largeKnobSize + 18);

    // Secondary controls row (Damping, Pre-delay, Width) - below, smaller
    int secondaryRowWidth = mediumKnobSize * 3 + knobSpacing * 2;
    int secondaryX = mainContent.getX() + (mainContent.getWidth() - secondaryRowWidth) / 2;
    int secondaryY = primaryY + largeKnobSize + 22;

    dampingKnob.setBounds(secondaryX, secondaryY, mediumKnobSize, mediumKnobSize + 16);
    preDelayKnob.setBounds(secondaryX + mediumKnobSize + knobSpacing, secondaryY, mediumKnobSize, mediumKnobSize + 16);
    widthKnob.setBounds(secondaryX + (mediumKnobSize + knobSpacing) * 2, secondaryY, mediumKnobSize, mediumKnobSize + 16);

    // Visualizer panel (right side of top row)
    topRow.removeFromLeft(spacing);
    visualizer.setBounds(topRow);

    contentArea.removeFromTop(spacing);

    // ===== BOTTOM ROW =====
    auto bottomRow = contentArea.removeFromTop(130);

    // Calculate widths for bottom panels
    int totalWidth = bottomRow.getWidth();
    int erWidth = static_cast<int>(totalWidth * 0.33f);
    int filterWidth = static_cast<int>(totalWidth * 0.33f);

    // Early Reflections panel
    auto erArea = bottomRow.removeFromLeft(erWidth);
    erSection.setBounds(erArea);

    auto erContent = erSection.getContentBounds();
    int smallKnobSize = 60;
    int erKnobX = erContent.getX() + (erContent.getWidth() - smallKnobSize * 2 - knobSpacing) / 2;
    erLevelKnob.setBounds(erKnobX, erContent.getY(), smallKnobSize, smallKnobSize + 14);
    erSizeKnob.setBounds(erKnobX + smallKnobSize + knobSpacing, erContent.getY(), smallKnobSize, smallKnobSize + 14);

    bottomRow.removeFromLeft(spacing);

    // Tone/Filter panel
    auto filterArea = bottomRow.removeFromLeft(filterWidth);
    filterSection.setBounds(filterArea);

    auto filterContent = filterSection.getContentBounds();
    int filterKnobX = filterContent.getX() + (filterContent.getWidth() - smallKnobSize * 2 - knobSpacing) / 2;
    lowCutKnob.setBounds(filterKnobX, filterContent.getY(), smallKnobSize, smallKnobSize + 14);
    highCutKnob.setBounds(filterKnobX + smallKnobSize + knobSpacing, filterContent.getY(), smallKnobSize, smallKnobSize + 14);

    bottomRow.removeFromLeft(spacing);

    // I/O panel
    ioSection.setBounds(bottomRow);

    auto ioContent = ioSection.getContentBounds();
    int ioKnobX = ioContent.getX() + (ioContent.getWidth() - smallKnobSize * 2 - knobSpacing) / 2;
    inputKnob.setBounds(ioKnobX, ioContent.getY(), smallKnobSize, smallKnobSize + 14);
    outputKnob.setBounds(ioKnobX + smallKnobSize + knobSpacing, ioContent.getY(), smallKnobSize, smallKnobSize + 14);
}

void AuraEditor::timerCallback()
{
    auto& apvts = processor.getAPVTS();
    float decayVal = apvts.getRawParameterValue(ParamIDs::decay)->load();
    int roomType = static_cast<int>(apvts.getRawParameterValue(ParamIDs::roomType)->load());

    visualizer.setDecayLevel(processor.getDecayEnvelope());
    visualizer.setDecayTime(decayVal);
    visualizer.setRoomType(roomType);
}

} // namespace Aura
