#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class BigSkyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BigSkyLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colours::white);
        setColour (juce::Label::textColourId, juce::Colours::white);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                            juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (4.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // knurled outer ring
        g.setColour (juce::Colour (0xff1a1a1a));
        g.fillEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

        // glossy black knob body
        juce::ColourGradient grad (juce::Colour (0xff3a3a3a), centre.x, centre.y - radius,
                                    juce::Colour (0xff0a0a0a), centre.x, centre.y + radius, false);
        g.setGradientFill (grad);
        float bodyR = radius * 0.86f;
        g.fillEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f);

        // pointer line
        juce::Path pointer;
        float pointerLen = bodyR * 0.75f;
        float pointerThick = juce::jmax (2.0f, radius * 0.12f);
        pointer.addRoundedRectangle (-pointerThick * 0.5f, -bodyR, pointerThick, pointerLen, pointerThick * 0.5f);
        pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
        g.setColour (juce::Colours::white);
        g.fillPath (pointer);

        // subtle highlight ring
        g.setColour (juce::Colours::white.withAlpha (0.06f));
        g.drawEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f, 1.5f);

        // thin brass/gold accent ring at the base of the knob
        float goldR = radius * 0.92f;
        g.setColour (juce::Colour (0xffB8965A).withAlpha (0.6f));
        g.drawEllipse (centre.x - goldR, centre.y - goldR, goldR * 2.0f, goldR * 2.0f, 1.2f);
    }

    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.setColour (label.findColour (juce::Label::textColourId));
        g.setFont (label.getFont());
        g.drawFittedText (label.getText(), label.getLocalBounds(),
                           label.getJustificationType(), 1);
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                                bool isHighlighted, bool isDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (2.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();

        g.setColour (juce::Colour (0xff222222));
        g.fillEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

        auto innerR = radius * 0.8f;
        juce::Colour top = isDown ? juce::Colour (0xff555555) : juce::Colour (0xffcfcfcf);
        juce::Colour bot = isDown ? juce::Colour (0xff2a2a2a) : juce::Colour (0xff8c8c8c);
        juce::ColourGradient grad (top, centre.x, centre.y - innerR, bot, centre.x, centre.y + innerR, false);
        g.setGradientFill (grad);
        g.fillEllipse (centre.x - innerR, centre.y - innerR, innerR * 2.0f, innerR * 2.0f);

        if (isHighlighted)
        {
            g.setColour (juce::Colours::white.withAlpha (0.15f));
            g.fillEllipse (centre.x - innerR, centre.y - innerR, innerR * 2.0f, innerR * 2.0f);
        }
    }
};
