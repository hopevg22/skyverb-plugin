#include "PluginEditor.h"

using APVTS = juce::AudioProcessorValueTreeState;

namespace
{
    // Two-column algorithm list flanking the encoder, top to bottom.
    // Only "ROOM" is implemented; the rest are a preview of future algorithms.
    const juce::StringArray kLeftAlgos  { "BLOOM", "SWELL", "SPRING", "PLATE", "HALL", "ROOM" };
    const juce::StringArray kRightAlgos { "CHORALE", "SHIMMER", "MAGNETO", "NONLINEAR", "REFLECTIONS" };
    constexpr int kSelectedLeftIndex = 5; // "ROOM"
}

BigSkyCloneAudioProcessorEditor::BigSkyCloneAudioProcessorEditor (BigSkyCloneAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&lookAndFeel);
    setSize (940, 640);

    // --- Value encoder (algorithm select stand-in; only Room exists for now)
    addAndMakeVisible (valueEncoder);
    valueEncoder.setRange (0.0, 1.0);
    valueEncoder.setValue (0.0);
    valueEncoder.setEnabled (false); // only one algorithm implemented so far
    valueLabel.setText ("VALUE", juce::dontSendNotification);
    valueLabel.setJustificationType (juce::Justification::centred);
    valueLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    valueLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (valueLabel);

    typeLabel.setText ("TYPE", juce::dontSendNotification);
    typeLabel.setJustificationType (juce::Justification::centred);
    typeLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    typeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (typeLabel);

    algoNameLabel.setText ("ROOM", juce::dontSendNotification);
    algoNameLabel.setJustificationType (juce::Justification::centred);
    algoNameLabel.setFont (juce::Font (22.0f, juce::Font::bold));
    algoNameLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (algoNameLabel);

    lcdLabel.setText ("BANK 01 - ROOM", juce::dontSendNotification);
    lcdLabel.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 26.0f, juce::Font::bold));
    lcdLabel.setColour (juce::Label::textColourId, juce::Colour (0xff6BFF7A));
    lcdLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (lcdLabel);

    // --- Row 1
    setupKnob (toneKnob,     "TONE",       BigSkyCloneAudioProcessor::idTone);
    setupKnob (decayKnob,    "DECAY",      BigSkyCloneAudioProcessor::idDecay);
    setupKnob (preDelayKnob, "PRE-DELAY",  BigSkyCloneAudioProcessor::idPreDelay);
    setupKnob (mixKnob,      "MIX",        BigSkyCloneAudioProcessor::idMix);

    // --- Row 2
    setupKnob (param1Knob, "PARAM 1", BigSkyCloneAudioProcessor::idParam1Size);
    setupKnob (param2Knob, "PARAM 2", BigSkyCloneAudioProcessor::idParam2Diff);
    setupKnob (modKnob,    "MOD",     BigSkyCloneAudioProcessor::idMod);

    // --- Footswitches: click = recall preset, Shift+click = save current
    // knob state into that slot. Recalling an empty slot does nothing.
    for (auto* b : { &footswitchA, &footswitchB, &footswitchC })
    {
        addAndMakeVisible (*b);
        b->setColour (juce::TextButton::textColourOffId, juce::Colours::transparentWhite);
        b->setColour (juce::TextButton::textColourOnId, juce::Colours::transparentWhite);
    }
    footswitchA.onClick = [this] { footswitchClicked (0, 'A'); };
    footswitchB.onClick = [this] { footswitchClicked (1, 'B'); };
    footswitchC.onClick = [this] { footswitchClicked (2, 'C'); };

    brandLabel.setText ("SkyVerb", juce::dontSendNotification);
    brandLabel.setFont (juce::Font (46.0f, juce::Font::italic | juce::Font::bold));
    brandLabel.setJustificationType (juce::Justification::centred);
    brandLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.92f));
    addAndMakeVisible (brandLabel);

    modelLabel.setText ("Multi-Dimensional Reverb", juce::dontSendNotification);
    modelLabel.setFont (juce::Font (16.0f));
    modelLabel.setJustificationType (juce::Justification::centred);
    modelLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.8f));
    addAndMakeVisible (modelLabel);
}

BigSkyCloneAudioProcessorEditor::~BigSkyCloneAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void BigSkyCloneAudioProcessorEditor::setupKnob (KnobWithLabel& k, const juce::String& text, const juce::String& paramId)
{
    addAndMakeVisible (k.slider);
    k.slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    k.attachment = std::make_unique<APVTS::SliderAttachment> (processor.apvts, paramId, k.slider);

    k.label.setText (text, juce::dontSendNotification);
    k.label.setJustificationType (juce::Justification::centred);
    k.label.setFont (juce::Font (13.0f, juce::Font::bold));
    k.label.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (k.label);
}

void BigSkyCloneAudioProcessorEditor::footswitchClicked (int slot, char letter)
{
    if (juce::ModifierKeys::getCurrentModifiers().isShiftDown())
    {
        processor.savePreset (slot);
        lcdLabel.setText (juce::String ("SAVED TO ") + juce::String::charToString ((juce::juce_wchar) letter),
                           juce::dontSendNotification);
    }
    else if (processor.isPresetSlotFilled (slot))
    {
        processor.loadPreset (slot);
        lcdLabel.setText (juce::String ("PRESET ") + juce::String::charToString ((juce::juce_wchar) letter),
                           juce::dontSendNotification);
    }
    else
    {
        lcdLabel.setText (juce::String::charToString ((juce::juce_wchar) letter) + juce::String (" IS EMPTY - SHIFT+CLICK TO SAVE"),
                           juce::dontSendNotification);
    }
    repaint();
}

void BigSkyCloneAudioProcessorEditor::drawAlgorithmRing (juce::Graphics& g)
{
    auto encBounds = valueEncoder.getBounds().toFloat();
    auto centre = encBounds.getCentre();
    const float rowSpacing = 34.0f;
    const float firstRowY = encBounds.getY() + 6.0f;
    const float leftDotX  = encBounds.getX() - 22.0f;
    const float rightDotX = encBounds.getRight() + 22.0f;
    const float dotSize = 11.0f;

    g.setFont (juce::Font (13.0f, juce::Font::bold));

    for (int i = 0; i < kLeftAlgos.size(); ++i)
    {
        float y = firstRowY + (float) i * rowSpacing;
        bool selected = (i == kSelectedLeftIndex);

        g.setColour (selected ? juce::Colour (0xff6BFF6E) : juce::Colours::black.withAlpha (0.35f));
        g.fillEllipse (leftDotX - dotSize * 0.5f, y - dotSize * 0.5f, dotSize, dotSize);
        if (selected)
        {
            g.setColour (juce::Colour (0xff6BFF6E).withAlpha (0.35f));
            g.fillEllipse (leftDotX - dotSize, y - dotSize, dotSize * 2.0f, dotSize * 2.0f);
        }

        g.setColour (juce::Colours::white.withAlpha (selected ? 1.0f : 0.75f));
        g.drawText (kLeftAlgos[i], juce::Rectangle<float> (leftDotX - 170.0f, y - 10.0f, 150.0f, 20.0f),
                    juce::Justification::centredRight);
    }

    for (int i = 0; i < kRightAlgos.size(); ++i)
    {
        float y = firstRowY + (float) i * rowSpacing;

        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.fillEllipse (rightDotX - dotSize * 0.5f, y - dotSize * 0.5f, dotSize, dotSize);

        g.setColour (juce::Colours::white.withAlpha (0.75f));
        g.drawText (kRightAlgos[i], juce::Rectangle<float> (rightDotX + 20.0f, y - 10.0f, 150.0f, 20.0f),
                    juce::Justification::centredLeft);
    }

    // unlabeled dot at the top of the ring, centered above the encoder
    float topDotY = firstRowY - rowSpacing;
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillEllipse (centre.x - dotSize * 0.5f, topDotY - dotSize * 0.5f, dotSize, dotSize);
}

void BigSkyCloneAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Big Sky's signature turquoise brushed-aluminium look
    juce::ColourGradient bg (juce::Colour (0xff3FC7D6), bounds.getX(), bounds.getY(),
                              juce::Colour (0xff1B8598), bounds.getRight(), bounds.getBottom(), false);
    g.setGradientFill (bg);
    g.fillAll();

    // brushed-metal horizontal streaks
    juce::Random rnd (42);
    for (int i = 0; i < 220; ++i)
    {
        float yy = rnd.nextFloat() * bounds.getHeight();
        float alpha = 0.02f + rnd.nextFloat() * 0.035f;
        g.setColour (juce::Colours::white.withAlpha (alpha));
        g.drawLine (0.0f, yy, bounds.getWidth(), yy, 1.0f);
    }

    // subtle vignette for depth
    juce::ColourGradient vignette (juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getCentreY(),
                                    juce::Colours::black.withAlpha (0.18f), 0.0f, 0.0f, true);
    vignette.addColour (0.85, juce::Colours::transparentBlack);
    g.setGradientFill (vignette);
    g.fillAll();

    // LCD panel
    auto lcdArea = juce::Rectangle<float> (32, 26, 400, 78);
    g.setColour (juce::Colours::black.withAlpha (0.75f));
    g.fillRoundedRectangle (lcdArea, 6.0f);
    g.setColour (juce::Colour (0xff6BFF7A).withAlpha (0.3f));
    g.drawRoundedRectangle (lcdArea, 6.0f, 1.5f);

    // algorithm selector ring around the encoder
    drawAlgorithmRing (g);

    // decorative rule flanking the subtitle, like the mockup
    auto subBounds = modelLabel.getBounds().toFloat();
    float lineY = subBounds.getCentreY();
    float gap = 14.0f;
    float lineLen = 90.0f;
    g.setColour (juce::Colours::white.withAlpha (0.5f));
    g.drawLine (subBounds.getCentreX() - subBounds.getWidth() * 0.5f - gap - lineLen, lineY,
                subBounds.getCentreX() - subBounds.getWidth() * 0.5f - gap, lineY, 1.0f);
    g.drawLine (subBounds.getCentreX() + subBounds.getWidth() * 0.5f + gap, lineY,
                subBounds.getCentreX() + subBounds.getWidth() * 0.5f + gap + lineLen, lineY, 1.0f);

    // footswitch LED indicators — lit green when that slot has a saved preset
    const float dotSize = 16.0f;
    const juce::TextButton* switches[3] = { &footswitchA, &footswitchB, &footswitchC };
    for (int i = 0; i < 3; ++i)
    {
        auto fsB = switches[i]->getBounds().toFloat();
        float dx = fsB.getCentreX() - dotSize * 0.5f;
        float dy = fsB.getY() - dotSize - 14.0f;
        bool filled = processor.isPresetSlotFilled (i);
        if (filled)
        {
            g.setColour (juce::Colour (0xff6BFF6E).withAlpha (0.4f));
            g.fillEllipse (dx - 5.0f, dy - 5.0f, dotSize + 10.0f, dotSize + 10.0f);
        }
        g.setColour (filled ? juce::Colour (0xff6BFF6E) : juce::Colours::black.withAlpha (0.3f));
        g.fillEllipse (dx, dy, dotSize, dotSize);
    }

    // BANK DOWN / BANK UP labels with flanking rules, between the footswitches
    auto drawBankLabel = [&] (juce::String text, float centreX, float y)
    {
        g.setFont (juce::Font (13.0f));
        g.setColour (juce::Colours::white.withAlpha (0.8f));
        float textWidth = 90.0f;
        g.drawText (text, juce::Rectangle<float> (centreX - textWidth * 0.5f, y - 9.0f, textWidth, 18.0f),
                    juce::Justification::centred);
        float lineGap = 50.0f;
        g.drawLine (centreX - textWidth * 0.5f - 60.0f, y, centreX - lineGap, y, 1.0f);
        g.drawLine (centreX + lineGap, y, centreX + textWidth * 0.5f + 60.0f, y, 1.0f);
    };

    float fsRowY = footswitchA.getBounds().getCentreY();
    drawBankLabel ("BANK DOWN", (footswitchA.getBounds().getCentreX() + footswitchB.getBounds().getCentreX()) * 0.5f, fsRowY);
    drawBankLabel ("BANK UP",   (footswitchB.getBounds().getCentreX() + footswitchC.getBounds().getCentreX()) * 0.5f, fsRowY);
}

void BigSkyCloneAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    int w = getWidth();

    lcdLabel.setBounds (32, 26, 400, 78);

    // Encoder sits left-of-centre with room for the two label columns
    juce::Rectangle<int> encoderArea (200, 210, 130, 130);
    valueEncoder.setBounds (encoderArea);
    valueLabel.setBounds (encoderArea.getX(), encoderArea.getBottom() + 12, encoderArea.getWidth(), 18);
    typeLabel.setBounds (encoderArea.getX(), encoderArea.getBottom() + 32, encoderArea.getWidth(), 16);
    algoNameLabel.setBounds (encoderArea.getX() - 60, encoderArea.getY() - 90, encoderArea.getWidth() + 120, 30);

    // Knob grid (right side), 4 columns
    const int gridLeft = 480;
    const int knobSize = 78;
    const int colGap = 32;
    const int col = knobSize + colGap;
    const int row1Y = 130;
    const int row2Y = 270;
    const int labelH = 18;

    auto placeKnob = [&] (KnobWithLabel& k, int colIndex, int rowY)
    {
        int x = gridLeft + colIndex * col;
        k.slider.setBounds (x, rowY, knobSize, knobSize);
        k.label.setBounds (x - 10, rowY + knobSize + 4, knobSize + 20, labelH);
    };

    placeKnob (toneKnob,     0, row1Y);
    placeKnob (decayKnob,    1, row1Y);
    placeKnob (preDelayKnob, 2, row1Y);
    placeKnob (mixKnob,      3, row1Y);

    placeKnob (param1Knob, 1, row2Y);
    placeKnob (param2Knob, 2, row2Y);
    placeKnob (modKnob,    3, row2Y);

    brandLabel.setBounds (0, 420, w, 60);
    modelLabel.setBounds (0, 480, w, 24);

    // Footswitches along the bottom edge
    int fsSize = 60;
    int fsY = getHeight() - 110;
    footswitchA.setBounds (140,          fsY, fsSize, fsSize);
    footswitchB.setBounds (w / 2 - fsSize / 2, fsY, fsSize, fsSize);
    footswitchC.setBounds (w - 140 - fsSize, fsY, fsSize, fsSize);
}
